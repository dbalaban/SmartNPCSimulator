#include "smart_actor.hpp"
#include <torch/torch.h>

SmartActor::SmartActor(Character* character,
                         GridWorld* world,
                         StateValueEstimator* v,
                         FOMAP* fomap,
                         size_t randomSeed) : 
    character(character),
    world(world),
    v(v),
    fomap(fomap),
    randomSeed(randomSeed),
    randomEngine(randomSeed),
    last_action_prob(torch::tensor(0.0)),
    last_state_value(torch::tensor(0.0)),
    discounting_factor(.999),
    learning_rate_actor(0.001),
    learning_rate_critic(0.01),
    decay_factor(1.0),
    optimizer_actor(fomap->parameters(), torch::optim::AdamOptions(learning_rate_actor)),
    optimizer_critic(v->parameters(), torch::optim::RMSpropOptions(learning_rate_critic)) {}

ActionDesc SmartActor::selectAction(const std::vector<ActionDesc>& actions) {
  // Get the current state
  double* grid_state = world->getFeatures();
  double* tile_state = world->getTileFeatures();
  double* character_state = world->getCharacterFeatures();

  // Convert to tensors
  auto grid_tensor = torch::from_blob(grid_state, {GridWorld::FeatureSize, 1});
  auto tile_tensor = torch::from_blob(tile_state, {Tile::FeatureSize, static_cast<long int>(world->getTileCount())});
  auto character_tensor = torch::from_blob(character_state, {Character::FeatureSize, static_cast<long int>(world->getCharacterCount())});

  // Forward pass through the value estimator
  auto value = v->forward(grid_tensor, tile_tensor, character_tensor);

  // Forward pass through the FOMAP
  auto action_probs = fomap->forward(grid_tensor, tile_tensor, character_tensor, value);

  // weighted random selection of index
  std::discrete_distribution<size_t> distribution(action_probs.data_ptr<float>(), action_probs.data_ptr<float>() + action_probs.size(0));
  size_t action_index = distribution(randomEngine);

  last_action_prob = action_probs[action_index];

  return actions[action_index];
}

void SmartActor::update(double reward) {
  // Get the current state
  double* grid_state = world->getFeatures();
  double* tile_state = world->getTileFeatures();
  double* character_state = world->getCharacterFeatures();

  // Convert to tensors
  auto grid_tensor = torch::from_blob(grid_state, {GridWorld::FeatureSize, 1});
  auto tile_tensor = torch::from_blob(tile_state, {Tile::FeatureSize, static_cast<long int>(world->getTileCount())});
  auto character_tensor = torch::from_blob(character_state, {Character::FeatureSize, static_cast<long int>(world->getCharacterCount())});

  // Forward pass through the value estimator
  torch::Tensor current_value;
  {
    torch::NoGradGuard no_grad;
    current_value = v->forward(grid_tensor, tile_tensor, character_tensor);
  }
  // Calculate the TD error
  auto td_error = reward + discounting_factor * current_value - last_state_value;
  auto v_loss = td_error.pow(2);

  // Update the value estimator
  v->zero_grad();
  v_loss.backward();
  optimizer_critic.step();

  last_state_value = v->forward(grid_tensor, tile_tensor, character_tensor);

  // Update the FOMAP
  auto advantage = td_error.detach();
  auto action_loss = -torch::log(last_action_prob) * advantage * decay_factor;
  fomap->zero_grad();
  action_loss.backward();
  optimizer_actor.step();
  decay_factor *= discounting_factor;
  decay_factor = std::max(1e-5, decay_factor);
}