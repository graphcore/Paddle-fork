// Copyright (c) 2021 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "paddle/fluid/framework/ipu/common.h"
#include "paddle/fluid/framework/ipu/popart_canonicalization/canonicalization_utils.h"

namespace paddle {
namespace framework {
namespace ipu {

std::string GenerateVarName();

Node *MakeVarNode(Graph *graph, Node *node);
Node *MakeOpNode(Graph *graph, Node *node, const std::string &type,
                 const std::vector<Node *> &inputs,
                 const std::vector<Node *> &outputs);

Node *CreateBaseOp(Graph *graph, Node *node, const std::string &type,
                   const std::vector<Node *> &inputs,
                   const std::vector<Node *> &outputs,
                   const AttributeMap &attrs = {});

template <typename T>
AttributeMap MakeConstAttrMap(std::vector<T> value, std::vector<int64_t> dims,
                              int dtype) {
  return AttributeMap{{"value", value}, {"dims", dims}, {"dtype", dtype}};
}

template <typename T>
AttributeMap MakeConstAttrMapFromValue(T v, std::vector<int64_t> dims,
                                       int dtype) {
  size_t size = 1;
  for (auto &dim : dims) {
    size *= dim;
  }
  return MakeConstAttrMap<T>(std::vector<T>(size, v), dims, dtype);
}

Node *CreateConst(Graph *graph, Node *node, const std::vector<Node *> &inputs,
                  const std::vector<Node *> &outputs,
                  const AttributeMap &attrs);

// otype is proto::VarType::Type
Node *CreateCast(Graph *graph, Node *node, const std::vector<Node *> &inputs,
                 const std::vector<Node *> &outputs, const int otype);

Node *CreateGemm(Graph *graph, Node *node, const std::vector<Node *> &inputs,
                 const std::vector<Node *> &outputs, int64_t transA = 0,
                 int64_t transB = 0, float alpha = 1.0f, float beta = 1.0f);

Node *CreateReshape(Graph *graph, Node *node, const std::vector<Node *> &inputs,
                    const std::vector<Node *> &outputs,
                    const std::vector<int64_t> &oshape);

Node *CreateConv(Graph *graph, Node *node, const std::vector<Node *> &inputs,
                 const std::vector<Node *> &outputs,
                 const std::vector<int64_t> &dilations = {1, 1},
                 int64_t group = 1,
                 const std::vector<int64_t> &kernel_shape = {},
                 const std::vector<int64_t> &pads = {0, 0, 0, 0},
                 const std::vector<int64_t> &strides = {1, 1});

}  // namespace ipu
}  // namespace framework
}  // namespace paddle
