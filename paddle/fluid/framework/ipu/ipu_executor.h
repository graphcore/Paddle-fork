/* Copyright (c) 2021 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once

#include <popart/dataflow.hpp>
#include <popart/half.hpp>
#include <popart/names.hpp>
#include <popart/patterns/patterns.hpp>
#include <popart/session.hpp>
#include <popart/tensorinfo.hpp>

#include "paddle/fluid/framework/ipu/ipu_compiler.h"
#include "paddle/fluid/framework/ipu/ipu_names.h"
#include "paddle/fluid/framework/ipu/ipu_strategy.h"
#include "paddle/fluid/framework/ipu/ipu_utils.h"
#include "paddle/fluid/framework/operator.h"
#include "paddle/fluid/framework/scope.h"

namespace paddle {
namespace framework {
namespace ipu {

struct OneSession {
  // map<tensor_id, paddle_var_ptr>
  popart::WeightsIO weights_io;
  // <popart_var, paddle_var> pairs, include weights and optimizer states
  std::vector<std::pair<popart::TensorId, popart::TensorId>>
      weights_and_opt_state;
};

class Executor {
 public:
  Executor() = default;
  ~Executor() = default;

  // build popart session
  void Prepare(const std::string &proto);

  // run popart session
  void Run(const std::vector<const Tensor *> &inputs,
           const std::vector<Tensor *> &outputs,
           const framework::ExecutionContext &ctx);

  // detach IPU
  void Detach();

  void SetWeightsIO();
  void ConvertWeights(bool align_to_popart);
  void WeightsFromPaddle();
  void WeightsToPaddle();

  // Scope
  void SetScope(const Scope *scope) { scope_ = scope; }

  // Strategy
  void SetIpuStrategy(const IpuStrategy &strategy) {
    ipu_strategy_ = &strategy;
  }

  // OneBuilder
  void SetBuilder(OneBuilder *one_builder) { one_builder_ = one_builder; }

 private:
  void AcquireDevice();

 private:
  // not own
  const Scope *scope_ = nullptr;
  const IpuStrategy *ipu_strategy_ = nullptr;
  OneBuilder *one_builder_ = nullptr;

  // deviceinfo for popart session
  std::shared_ptr<popart::DeviceInfo> device_;
  // popart session, where graph running
  std::unique_ptr<popart::Session> session_;
  // one OneSession means a graph
  std::shared_ptr<OneSession> one_session_;

  int step_ = 0;
};

}  // namespace ipu
}  // namespace framework
}  // namespace paddle
