#ifndef FOMAP_HPP
#define FOMAP_HPP

// Fully Observable Markovian Action Policy (FOMAP)

#include <torch/torch.h>

class FOMAP : public torch::nn::Module {
public:
  FOMAP(const size_t projection_size,
        const size_t output_size);
  torch::Tensor forward(torch::Tensor grid_state,
                        torch::Tensor tile_state,
                        torch::Tensor character_state,
                        torch::Tensor actions);

private:
  size_t projection_size;
  size_t output_size;
  size_t grid_state_size;
  size_t tile_state_size;
  size_t character_state_size;
  size_t action_size;

  torch::nn::Linear grid_state_projection;
  torch::nn::Linear tile_state_projection;
  torch::nn::Linear character_state_projection;

  // multi head attention
  torch::nn::Linear query_actions;
  torch::nn::Linear key_grid_state_projection;
  torch::nn::Linear key_tile_state_projection;
  torch::nn::Linear key_character_state_projection;
  torch::nn::Linear value_grid_state_projection;
  torch::nn::Linear value_tile_state_projection;
  torch::nn::Linear value_character_state_projection;

  torch::nn::Linear grid_weight;
  torch::nn::Linear tile_weight;
  torch::nn::Linear char_weight;

  torch::nn::Linear output_projection;
  torch::nn::Linear output_layer;
  
};

#endif // FOMAP_HPP