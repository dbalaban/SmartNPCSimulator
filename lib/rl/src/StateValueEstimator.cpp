#include "StateValueEstimator.hpp"

#include "tile.hpp"
#include <torch/torch.h>
#include "character.hpp"
#include "abstract_action.hpp"
#include "gridworld.hpp"

StateValueEstimator::StateValueEstimator(size_t projection_size) :
    projection_size(projection_size),
    grid_state_size(GridWorld::FeatureSize),
    tile_state_size(Tile::FeatureSize),
    char_state_size(Character::FeatureSize),
    grid_state_projection(torch::nn::Linear(torch::nn::LinearOptions(grid_state_size, projection_size))),
    tile_state_projection(torch::nn::Linear(torch::nn::LinearOptions(tile_state_size, projection_size))),
    char_state_projection(torch::nn::Linear(torch::nn::LinearOptions(char_state_size, projection_size))),
    query_grid_weights(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    query_tile_weights(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    query_char_weights(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    key_grid_weights(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    key_tile_weights(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    key_char_weights(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    value_grid_weights(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    value_tile_weights(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    value_char_weights(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    grid_weight(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    tile_weight(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    char_weight(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    output_projection(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    output_layer(torch::nn::Linear(torch::nn::LinearOptions(projection_size, 1))) {
  register_module("grid_state_projection", grid_state_projection);
  register_module("tile_state_projection", tile_state_projection);
  register_module("char_state_projection", char_state_projection);
  register_module("query_grid_weights", query_grid_weights);
  register_module("query_tile_weights", query_tile_weights);
  register_module("query_char_weights", query_char_weights);
  register_module("key_grid_weights", key_grid_weights);
  register_module("key_tile_weights", key_tile_weights);
  register_module("key_char_weights", key_char_weights);
  register_module("value_grid_weights", value_grid_weights);
  register_module("value_tile_weights", value_tile_weights);
  register_module("value_char_weights", value_char_weights);
  register_module("grid_weight", grid_weight);
  register_module("tile_weight", tile_weight);
  register_module("char_weight", char_weight);
  register_module("output_projection", output_projection);
  register_module("output_layer", output_layer);
}

StateValueEstimator::~StateValueEstimator() {}

torch::Tensor StateValueEstimator::forward(torch::Tensor grid_state,
                                           torch::Tensor tile_state,
                                           torch::Tensor character_state) {
  // project into shared space
  auto grid_proj = this->grid_state_projection(grid_state);
  auto tile_proj = this->tile_state_projection(tile_state);
  auto char_proj = this->char_state_projection(character_state);

  // GELU activation function on state projections
  grid_proj = torch::gelu(grid_proj);
  tile_proj = torch::gelu(tile_proj);
  char_proj = torch::gelu(char_proj);

  // get QKV matrices
  auto query_grid = this->query_grid_weights(grid_proj);
  auto query_tile = this->query_tile_weights(tile_proj);
  auto query_char = this->query_char_weights(char_proj);

  auto key_grid = this->key_grid_weights(grid_proj);
  auto key_tile = this->key_tile_weights(tile_proj);
  auto key_char = this->key_char_weights(char_proj);

  auto value_grid = this->value_grid_weights(grid_proj);
  auto value_tile = this->value_tile_weights(tile_proj);
  auto value_char = this->value_char_weights(char_proj);

  // attention
  auto attention_grid = torch::matmul(query_grid, key_grid.transpose(0, 1));
  auto attention_tile = torch::matmul(query_tile, key_tile.transpose(0, 1));
  auto attention_char = torch::matmul(query_char, key_char.transpose(0, 1));

  float d = sqrt(static_cast<float>(projection_size));
  attention_grid = torch::softmax(attention_grid/d, 1);
  attention_tile = torch::softmax(attention_tile/d, 1);
  attention_char = torch::softmax(attention_char/d, 1);

  attention_grid = torch::matmul(attention_grid, value_grid);
  attention_tile = torch::matmul(attention_tile, value_tile);
  attention_char = torch::matmul(attention_char, value_char);

  attention_grid = torch::layer_norm(attention_grid, {static_cast<int64_t>(projection_size)});
  attention_tile = torch::layer_norm(attention_tile, {static_cast<int64_t>(projection_size)});
  attention_char = torch::layer_norm(attention_char, {static_cast<int64_t>(projection_size)});

  // weight attention
  attention_grid = this->grid_weight(attention_grid);
  attention_tile = this->tile_weight(attention_tile);
  attention_char = this->char_weight(attention_char);

  // sum attention
  auto attention = attention_grid + attention_tile + attention_char;
  attention = torch::gelu(attention);

  // project into output space
  auto output = this->output_projection(attention);
  output = torch::gelu(output);
  output = this->output_layer(output);

  // exponential softmax
  output = torch::softmax(output, 0);

  return output;
}