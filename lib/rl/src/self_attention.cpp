#include "self_attention.hpp"

SelfAttentionImpl::SelfAttentionImpl(int embed_size, int heads) :
    embed_size_(embed_size),
    heads_(heads),
    head_dim_(embed_size / heads),
    query_(torch::nn::Linear(torch::nn::LinearOptions(embed_size, embed_size))),
    key_(torch::nn::Linear(torch::nn::LinearOptions(embed_size, embed_size))),
    value_(torch::nn::Linear(torch::nn::LinearOptions(embed_size, embed_size))),
    fc_out_(torch::nn::Linear(torch::nn::LinearOptions(embed_size, embed_size))) {
  register_module("query", query_);
  register_module("key", key_);
  register_module("value", value_);
  register_module("fc_out", fc_out_);
}

torch::Tensor SelfAttentionImpl::forward(torch::Tensor x) {
  int N = x.size(0);
  auto query = query_(x);
  auto key = key_(x);
  auto value = value_(x);

  // size [NxE] -> [NxhxF]
  query = query.view({N, heads_, head_dim_}).transpose(1, 0);
  key = key.view({N, heads_, head_dim_}).transpose(1, 0);
  value = value.view({N, heads_, head_dim_}).transpose(1, 0);

  // size [hxNxF] x [hxNxF] -> [hxNxN]
  auto attention = torch::matmul(query, key.transpose(1, 2));
  attention = torch::softmax(attention / sqrt(head_dim_), 2);
  // size [hxNxN] x [hxNxF] -> [hxNxF]
  auto out = torch::matmul(attention, value);
  // size [hxNxF] -> [NxE]
  out = out.transpose(1, 0).contiguous().view({N, embed_size_});

  out = torch::layer_norm(out, embed_size_);
  out = fc_out_(out);

  return torch::gelu(out);
}