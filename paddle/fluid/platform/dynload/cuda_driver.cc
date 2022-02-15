/* Copyright (c) 2019 PaddlePaddle Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include "paddle/fluid/platform/dynload/cuda_driver.h"
#include "paddle/pten/backends/dynload/cuda_driver.h"

namespace paddle {
namespace platform {
namespace dynload {

#define DEFINE_WRAP(__name) DynLoad__##__name __name

#if CUDA_VERSION >= 10020
CUDA_ROUTINE_EACH_VVM(DEFINE_WRAP);
#endif
CUDA_ROUTINE_EACH(DEFINE_WRAP);

bool HasCUDADriver() { return pten::dynload::HasCUDADriver(); }

}  // namespace dynload
}  // namespace platform
}  // namespace paddle
