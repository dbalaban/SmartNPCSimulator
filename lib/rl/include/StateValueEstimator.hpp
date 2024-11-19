#ifndef STATE_VALUE_ESTIMATOR_HPP
#define STATE_VALUE_ESTIMATOR_HPP

#include <vector>
#include <torch/torch.h>

class StateValueEstimator : public torch::nn::Module {
public:
  // Constructor
  StateValueEstimator(size_t projection_size);

  // Destructor
  ~StateValueEstimator();

  // Forward pass
  torch::Tensor forward(torch::Tensor grid_state,
                        torch::Tensor tile_state,
                        torch::Tensor character_state);

private:
  size_t projection_size;
  size_t grid_state_size;
  size_t tile_state_size;
  size_t char_state_size;

  torch::nn::Linear grid_state_projection;
  torch::nn::Linear tile_state_projection;
  torch::nn::Linear char_state_projection;

  torch::nn::Linear query;

  torch::nn::Linear key_grid_weights;
  torch::nn::Linear key_tile_weights;
  torch::nn::Linear key_char_weights;

  torch::nn::Linear value_grid_weights;
  torch::nn::Linear value_tile_weights;
  torch::nn::Linear value_char_weights;

  torch::nn::Linear grid_weight;
  torch::nn::Linear tile_weight;
  torch::nn::Linear char_weight;

  torch::nn::Linear output_projection;
  torch::nn::Linear output_layer1;
  torch::nn::Linear output_layer2;

};

#endif // STATE_VALUE_ESTIMATOR_HPP