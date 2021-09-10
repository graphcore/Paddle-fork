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

#include <popart/devicemanager.hpp>
#include <popart/names.hpp>

#include "paddle/fluid/framework/feed_fetch_type.h"
#include "paddle/fluid/framework/ipu/ipu_compiler.h"
#include "paddle/fluid/framework/ipu/device.h"
#include "paddle/fluid/framework/ipu/ipu_executor.h"
#include "paddle/fluid/framework/ipu/ipu_strategy.h"
#include "paddle/fluid/framework/scope.h"
#include "paddle/fluid/framework/tensor.h"
#include "paddle/fluid/platform/enforce.h"

namespace paddle {
namespace framework {
namespace ipu {

class IpuBackend {
  // IpuBackend is the center of paddle-ipu, its function include:
  //   1. Compile paddle model to popart model
  //   2. Run popart model, inference or training
  //   3. Request and release device
  //   4. Other helper function

 public:
  IpuBackend();
  ~IpuBackend();

  // return if exsits, else create and return
  static std::shared_ptr<IpuBackend> GetInstance();

  // always return a new instance_
  static std::shared_ptr<IpuBackend> GetNewInstance();

  // what compile does include(call compiler_):
  //   1. map paddle-op -> poart op
  //   2. construct popart onnx compute graph
  void Compile(ir::Graph *graph, const std::vector<std::string> &feed_list,
               const std::vector<std::string> &fetch_list);

  // what run does include:
  //   1. construct forward onnx graph
  //   2. graph-level optimization
  //   3. autodiff
  void Run(const std::vector<const Tensor *> &inputs,
           const std::vector<Tensor *> &outputs);

  Executor &GetExecutor() { return *executor_; };

  void SetScope(Scope &scope);
  void SetIpuStrategy(const IpuStrategy &strategy);

  // Device
  size_t GetNumDevices();
  std::vector<int> GetDeviceIds();
  Device GetDevice(int id);
  void AttachDevice(int id);
  bool DeviceIsAttached();

 private:
  void Prepare();
  int UpperIpuNum();

 private:
  std::shared_ptr<Compiler> compiler_;
  std::unique_ptr<Executor> executor_;
  std::shared_ptr<popart::DeviceInfo> device_;
  bool is_prepared_ = false;

  // not own
  Scope *scope_ = nullptr;
  const IpuStrategy *ipu_strategy_ = nullptr;
};

}  // namespace ipu
}  // namespace framework
}  // namespace paddle
