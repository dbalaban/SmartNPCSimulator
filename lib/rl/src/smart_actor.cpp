#include "smart_actor.hpp"
#include <torch/torch.h>

#include <iostream>

SmartActor::SmartActor(GridWorld* world,
                      StateValueEstimator* v,
                      FOMAP* fomap,
                      size_t randomSeed) : 
    world(world),
    v(v),
    fomap(fomap),
    randomSeed(randomSeed),
    randomEngine(randomSeed),
    last_action_prob(torch::tensor(0.0)),
    last_state_value(torch::tensor(0.0)),
    discounting_factor(0.9),
    learning_rate_actor(10),
    learning_rate_critic(.01),
    decay_factor(1.0),
    optimizer_actor(fomap->parameters(), torch::optim::AdamOptions(learning_rate_actor)),
    optimizer_critic(v->parameters(), torch::optim::RMSpropOptions(learning_rate_critic)) {}

ActionDesc SmartActor::selectAction(const std::vector<ActionDesc>& actions) {
  // Get the current state
  std::unique_ptr<double[]> grid_state(world->getFeatures());
  std::unique_ptr<double[]> tile_state = world->getTileFeatures();
  std::unique_ptr<double[]> character_state = world->getCharacterFeatures();

  // Convert to tensors
  auto grid_tensor = torch::from_blob(grid_state.get(), {1, GridWorld::FeatureSize}, torch::kDouble).to(torch::kFloat);
  auto tile_tensor = torch::from_blob(tile_state.get(), {static_cast<long int>(world->getTileCount()), Tile::FeatureSize}, torch::kDouble).to(torch::kFloat);
  auto character_tensor = torch::from_blob(character_state.get(), {static_cast<long int>(world->getCharacterCount()), Character::FeatureSize}, torch::kDouble).to(torch::kFloat);

  auto actions_tensor = torch::zeros({static_cast<long int>(actions.size()), ActionDesc::actionSize});
  for (size_t i = 0; i < actions.size(); i++) {
    auto action_features = actions[i].getFeatures();
    for (size_t j = 0; j < ActionDesc::actionSize; j++) {
      actions_tensor[i][j] = action_features[j];
    }
  }

  // Forward pass through the value estimator
  last_state_value = v->forward(grid_tensor, tile_tensor, character_tensor);

  // Forward pass through the FOMAP
  auto action_probs = fomap->forward(grid_tensor, tile_tensor, character_tensor, actions_tensor);

  std::cout << "state value: " << last_state_value << std::endl;
  std::cout << "action probs: " << action_probs.transpose(1,0) << std::endl;

  // weighted random selection of index
  std::discrete_distribution<size_t> distribution(action_probs.data_ptr<float>(), action_probs.data_ptr<float>() + action_probs.size(0));
  size_t action_index = distribution(randomEngine);
  std::cout << "selected action: " << action_index+1 << " / " << actions.size() << std::endl;

  last_action_prob = action_probs[action_index];

  return actions[action_index];
}

void SmartActor::update(double reward) {
  // Get the current state
  std::unique_ptr<double[]> grid_state(world->getFeatures());
  std::unique_ptr<double[]> tile_state = world->getTileFeatures();
  std::unique_ptr<double[]> character_state = world->getCharacterFeatures();

  // Convert to tensors
  auto grid_tensor = torch::from_blob(grid_state.get(), {1, GridWorld::FeatureSize}, torch::kDouble).to(torch::kFloat);
  auto tile_tensor = torch::from_blob(tile_state.get(), {static_cast<long int>(world->getTileCount()), Tile::FeatureSize}, torch::kDouble).to(torch::kFloat);
  auto character_tensor = torch::from_blob(character_state.get(), {static_cast<long int>(world->getCharacterCount()), Character::FeatureSize}, torch::kDouble).to(torch::kFloat);

  // Forward pass through the value estimator
  torch::Tensor current_value;
  {
    torch::NoGradGuard no_grad;
    current_value = v->forward(grid_tensor, tile_tensor, character_tensor);
  }
  // Calculate the TD error
  auto td_error = reward + discounting_factor * current_value - last_state_value;
  auto v_loss = td_error.pow(2);

  std::cout << "reward: " << reward << std::endl;
  std::cout << "td error: " << td_error << std::endl;

  // Update the value estimator
  v->zero_grad();
  v_loss.backward();
  optimizer_critic.step();

  // Update the FOMAP
  auto advantage = td_error.detach();
  auto action_loss = -torch::log(last_action_prob) * advantage * decay_factor;
  fomap->zero_grad();
  action_loss.backward();
  optimizer_actor.step();
  decay_factor *= discounting_factor;
  decay_factor = std::max(1e-5, decay_factor);
}