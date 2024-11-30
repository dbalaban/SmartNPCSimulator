#include "smart_actor.hpp"
#include <torch/torch.h>
#include <iostream>

#include "param_reader.hpp"
#include "data_writer.hpp"

using namespace rl;
using namespace data_management;

SmartActor::SmartActor() :
    world(GridWorld::getInstance()),
    v(StateValueEstimator()),
    fomap(FOMAP()),
    randomSeed(data_management::ParamReader::getInstance().getParam<size_t>("GridWorld", "randomSeed", 42)),
    randomEngine(randomSeed),
    last_action_prob(torch::tensor(0.0)),
    last_state_value(torch::tensor(0.0)),
    discounting_factor(data_management::ParamReader::getInstance().getParam<double>("SmartActor", "discounting_factor", 0.99)),
    learning_rate_actor(data_management::ParamReader::getInstance().getParam<double>("SmartActor", "learning_rate_actor", 0.01)),
    learning_rate_critic(data_management::ParamReader::getInstance().getParam<double>("SmartActor", "learning_rate_critic", 0.01)),
    decay_factor(1.0),
    optimizer_actor(fomap.parameters(), torch::optim::AdamOptions(learning_rate_actor)),
    optimizer_critic(v.parameters(), torch::optim::RMSpropOptions(learning_rate_critic)) {}

ActionDesc SmartActor::selectAction(const std::vector<ActionDesc>& actions) {
  // Get the current state
  std::unique_ptr<double[]> grid_state = world.getFeatures();
  std::unique_ptr<double[]> tile_state = world.getTileFeatures();
  std::unique_ptr<double[]> character_state = world.getCharacterFeatures();

  // Convert to tensors
  auto grid_tensor = torch::from_blob(grid_state.get(), {1, GridWorld::FeatureSize}, torch::kDouble).to(torch::kFloat);
  auto tile_tensor = torch::from_blob(tile_state.get(), {static_cast<long int>(world.getTileCount()), Tile::FeatureSize}, torch::kDouble).to(torch::kFloat);
  auto character_tensor = torch::from_blob(character_state.get(), {static_cast<long int>(world.getCharacterCount()), Character::FeatureSize}, torch::kDouble).to(torch::kFloat);

  auto actions_tensor = torch::zeros({static_cast<long int>(actions.size()), ActionDesc::actionSize});
  for (size_t i = 0; i < actions.size(); i++) {
    auto action_features = actions[i].getFeatures();
    for (size_t j = 0; j < ActionDesc::actionSize; j++) {
      actions_tensor[i][j] = action_features[j];
    }
  }

  // Forward pass through the value estimator
  last_state_value = v.forward(grid_tensor, tile_tensor, character_tensor);

  // Forward pass through the FOMAP
  auto action_probs = fomap.forward(grid_tensor, tile_tensor, character_tensor, actions_tensor);
  std::vector<double> action_probs_vec;
  for (size_t i = 0; i < action_probs.size(0); i++) {
    action_probs_vec.push_back(action_probs[i].item<double>());
  }

  DataWriter& writer = DataWriter::getInstance();
  writer.writeData<double>("State Value", DataType::DOUBLE, last_state_value.item<double>());
  writer.writeData<std::vector<double>>("Action Probabilities", DataType::VECTOR, action_probs_vec);

  // weighted random selection of index
  std::discrete_distribution<size_t> distribution(action_probs.data_ptr<float>(), action_probs.data_ptr<float>() + action_probs.size(0));
  size_t action_index = distribution(randomEngine);
  writer.writeData<size_t>("Selected Action Index", DataType::SIZE, action_index);
  writer.writeData<size_t>("Selected Action ID", DataType::SIZE, actions[action_index].ActionID);

  last_action_prob = action_probs[action_index];

  return actions[action_index];
}

void SmartActor::update(double reward) {
  // Get the current state
  std::unique_ptr<double[]> grid_state = world.getFeatures();
  std::unique_ptr<double[]> tile_state = world.getTileFeatures();
  std::unique_ptr<double[]> character_state = world.getCharacterFeatures();

  // Convert to tensors
  auto grid_tensor = torch::from_blob(grid_state.get(), {1, GridWorld::FeatureSize}, torch::kDouble).to(torch::kFloat);
  auto tile_tensor = torch::from_blob(tile_state.get(), {static_cast<long int>(world.getTileCount()), Tile::FeatureSize}, torch::kDouble).to(torch::kFloat);
  auto character_tensor = torch::from_blob(character_state.get(), {static_cast<long int>(world.getCharacterCount()), Character::FeatureSize}, torch::kDouble).to(torch::kFloat);

  // Forward pass through the value estimator
  torch::Tensor current_value;
  {
    torch::NoGradGuard no_grad;
    current_value = v.forward(grid_tensor, tile_tensor, character_tensor);
  }
  // Calculate the TD error
  auto td_error = reward + discounting_factor * current_value - last_state_value;
  auto v_loss = td_error.pow(2);

  DataWriter& writer = DataWriter::getInstance();
  writer.writeData<double>("Estimated Current Value", DataType::DOUBLE, current_value.item<double>());
  writer.writeData<double>("Reward", DataType::DOUBLE, reward);
  writer.writeData<double>("TD Error", DataType::DOUBLE, td_error.item<double>());

  // Update the value estimator
  v.zero_grad();
  v_loss.backward();
  optimizer_critic.step();

  // Update the FOMAP
  auto advantage = td_error.detach();
  auto action_loss = -torch::log(last_action_prob) * advantage * decay_factor;
  fomap.zero_grad();
  action_loss.backward();
  optimizer_actor.step();
  decay_factor *= discounting_factor;
  decay_factor = std::max(1e-5, decay_factor);
}