#include "cross_attention.hpp"

CrossAttentionImpl::CrossAttentionImpl(int64_t embed_dim, int64_t num_heads) :
    embed_dim_(embed_dim),
    num_heads_(num_heads),
    query_(torch::nn::Linear(embed_dim, embed_dim)),
    key_(torch::nn::Linear(embed_dim, embed_dim)),
    value_(torch::nn::Linear(embed_dim, embed_dim)),
    fc_out_(torch::nn::Linear(embed_dim, embed_dim)) {
  register_module("query", query_);
  register_module("key", key_);
  register_module("value", value_);
  register_module("fc_out", fc_out_);
}

torch::Tensor CrossAttentionImpl::forward(torch::Tensor target, torch::Tensor descriptor) {
  int64_t t = target.size(0);
  int64_t o = descriptor.size(0);
  int64_t d = embed_dim_ / num_heads_;

  // [t,f] -> [t, num_heads, d]
  auto q = query_(target).view({t, num_heads_, d});
  // [o,f] -> [o, num_heads, d]
  auto k = key_(descriptor).view({o, num_heads_, d});
  auto v = value_(descriptor).view({o, num_heads_, d});

  // [t, num_heads, d] x [o, num_heads, d] -> [t, o]
  auto qk = torch::matmul(q, k) / sqrt(d);
  auto weights = torch::softmax(qk, 1);

  // [t, o] x [o, num_heads, d] -> [t, f]
  auto out = torch::matmul(weights, v).view({t, embed_dim_});
  out = torch::layer_norm(out, {embed_dim_});
  out = fc_out_(out);
  return torch::gelu(out);
}