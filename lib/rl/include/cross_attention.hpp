#ifndef CROSS_ATTENTION_HPP
#define CROSS_ATTENTION_HPP

#include <torch/torch.h>

class CrossAttentionImpl : public torch::nn::Module {
public:
  CrossAttentionImpl(int64_t embed_dim, int64_t num_heads);

  // for selecting the best target given a contextual descriptor
  // target: [number of options, embed_dim] [t, d]
  // descriptor: [number of contextual objects, embed_dim] [o, d]
  torch::Tensor forward(torch::Tensor target, torch::Tensor descriptor);

private:
  int64_t embed_dim_;
  int64_t num_heads_;
  torch::nn::Linear query_;
  torch::nn::Linear key_;
  torch::nn::Linear value_;
  torch::nn::Linear fc_out_;
};

TORCH_MODULE(CrossAttention);

#endif // CROSS_ATTENTION_HPP