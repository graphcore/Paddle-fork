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

#include "paddle/fluid/framework/ipu/popart_canonicalization/canonicalization_utils.h"

namespace paddle {
namespace framework {
namespace ipu {

std::string GenerateVarName();

ir::Node *MakeVarNode(ir::Graph *graph);
ir::Node *MakeOpNode(ir::Graph *graph, const std::string &type,
                     const std::vector<ir::Node *> &inputs,
                     const std::vector<ir::Node *> &outputs);

Node *CreateBaseOp(ir::Graph *graph, const std::string &type,
                   const std::vector<ir::Node *> &inputs,
                   const std::vector<ir::Node *> &outputs,
                   const AttributeMap &attrs = {});

// TODO(alleng) make template
AttributeMap MakeConstAttributeMap(float v, std::vector<int64_t> dims,
                                   int dtype);
ir::Node *CreateConst(ir::Graph *graph, const std::vector<ir::Node *> &inputs,
                      const std::vector<ir::Node *> &outputs,
                      const AttributeMap &attrs);

}  // namespace ipu
}  // namespace framework
}  // namespace paddle
