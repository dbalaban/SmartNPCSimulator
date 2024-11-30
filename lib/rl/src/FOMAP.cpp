#include "FOMAP.hpp"

#include "tile.hpp"
#include "character.hpp"
#include "abstract_action.hpp"
#include "gridworld.hpp"

#include "param_reader.hpp"

using namespace rl;

FOMAP::FOMAP() :
    projection_size(data_management::ParamReader::getInstance().getParam<size_t>("FOMAP", "projection_size", 1024)),
    output_size(data_management::ParamReader::getInstance().getParam<size_t>("FOMAP", "output_size", 1024)),
    grid_state_size(GridWorld::FeatureSize),
    tile_state_size(Tile::FeatureSize),
    character_state_size(Character::FeatureSize),
    action_size(ActionDesc::actionSize),
    grid_state_projection(torch::nn::Linear(torch::nn::LinearOptions(grid_state_size, projection_size))),
    tile_state_projection(torch::nn::Linear(torch::nn::LinearOptions(tile_state_size, projection_size))),
    character_state_projection(torch::nn::Linear(torch::nn::LinearOptions(character_state_size, projection_size))),
    query_actions(torch::nn::Linear(torch::nn::LinearOptions(action_size, projection_size))),
    key_grid_state_projection(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    key_tile_state_projection(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    key_character_state_projection(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    value_grid_state_projection(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    value_tile_state_projection(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    value_character_state_projection(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    grid_weight(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    tile_weight(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    char_weight(torch::nn::Linear(torch::nn::LinearOptions(projection_size, projection_size))),
    output_projection(torch::nn::Linear(torch::nn::LinearOptions(projection_size, output_size))),
    output_layer(torch::nn::Linear(torch::nn::LinearOptions(output_size, 1))) {
  register_module("grid_state_projection", grid_state_projection);
  register_module("tile_state_projection", tile_state_projection);
  register_module("character_state_projection", character_state_projection);
  register_module("query_actions", query_actions);
  register_module("key_grid_state_projection", key_grid_state_projection);
  register_module("key_tile_state_projection", key_tile_state_projection);
  register_module("key_character_state_projection", key_character_state_projection);
  register_module("value_grid_state_projection", value_grid_state_projection);
  register_module("value_tile_state_projection", value_tile_state_projection);
  register_module("value_character_state_projection", value_character_state_projection);
  register_module("grid_weight", grid_weight);
  register_module("tile_weight", tile_weight);
  register_module("char_weight", char_weight);
  register_module("output_projection", output_projection);
  register_module("output_layer", output_layer);
}

torch::Tensor FOMAP::forward(torch::Tensor grid_state,
                            torch::Tensor tile_state,
                            torch::Tensor character_state,
                            torch::Tensor actions) {
  // project into shared space
  auto grid_proj = this->grid_state_projection(grid_state);
  auto tile_proj = this->tile_state_projection(tile_state);
  auto char_proj = this->character_state_projection(character_state);
  auto query = this->query_actions(actions);

  // GELU activation function on state projections
  grid_proj = torch::gelu(grid_proj);
  tile_proj = torch::gelu(tile_proj);
  char_proj = torch::gelu(char_proj);

  auto key_grid_state = this->key_grid_state_projection(grid_proj);
  auto key_tile_state = this->key_tile_state_projection(tile_proj);
  auto key_character_state = this->key_character_state_projection(char_proj);

  auto value_grid_state = this->value_grid_state_projection(grid_proj);
  auto value_tile_state = this->value_tile_state_projection(tile_proj);
  auto value_character_state = this->value_character_state_projection(char_proj);

  auto attention_grid = torch::matmul(query, key_grid_state.transpose(0, 1));
  auto attention_tile = torch::matmul(query, key_tile_state.transpose(0, 1));
  auto attention_char = torch::matmul(query, key_character_state.transpose(0, 1));

  float d = sqrt(static_cast<float>(projection_size));

  attention_grid = torch::softmax(attention_grid/d, 1);
  attention_tile = torch::softmax(attention_tile/d, 1);
  attention_char = torch::softmax(attention_char/d, 1);

  attention_grid = torch::matmul(attention_grid, value_grid_state);
  attention_tile = torch::matmul(attention_tile, value_tile_state);
  attention_char = torch::matmul(attention_char, value_character_state);

  attention_grid = torch::layer_norm(attention_grid, {static_cast<int64_t>(projection_size)});
  attention_tile = torch::layer_norm(attention_tile, {static_cast<int64_t>(projection_size)});
  attention_char = torch::layer_norm(attention_char, {static_cast<int64_t>(projection_size)});

  attention_grid = this->grid_weight(attention_grid);
  attention_tile = this->tile_weight(attention_tile);
  attention_char = this->char_weight(attention_char);

  auto attention = attention_grid + attention_tile + attention_char;
  attention = torch::gelu(attention);

  auto output = this->output_projection(attention);
  output = torch::gelu(output);
  output = this->output_layer(output);

  // exponential softmax
  output = torch::softmax(output, 0);

  return output;
}