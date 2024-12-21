#ifndef SELF_ATTENTION_HPP
#define SELF_ATTENTION_HPP

#include <torch/torch.h>

class SelfAttentionImpl : public torch::nn::Module {
public:
  SelfAttentionImpl(int embed_size, int heads);
  torch::Tensor forward(torch::Tensor x);

private:
  int embed_size_;
  int heads_;
  int head_dim_;
  torch::nn::Linear query_;
  torch::nn::Linear key_;
  torch::nn::Linear value_;
  torch::nn::Linear fc_out_;
};

TORCH_MODULE(SelfAttention);

#endif // SELF_ATTENTION_HPP