#  Copyright (c) 2021 PaddlePaddle Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from __future__ import print_function

import numpy as np
import unittest
import sys
import paddle
import paddle.fluid as fluid
import paddle.fluid.compiler as compiler
from paddle.fluid.tests.unittests.ipu.op_test_ipu import (IPUOpTest,
                                                          np_dtype_to_fluid_str)

paddle.enable_static()
SEED = 2021


@unittest.skipIf(not paddle.is_compiled_with_ipu(),
                 "core is not compiled with IPU")
class TestBase(IPUOpTest):
    def setUp(self):
        self.set_atol()
        self.set_feed()
        self.set_feed_attr()
        self.set_attrs()

    def set_atol(self):
        self.atol = 1e-6

    def set_feed(self):
        self.feed = {
            "image": np.random.uniform(size=[1, 3, 10, 10]).astype('float32'),
        }

    def set_feed_attr(self):
        self.feed_shape = [x.shape for x in self.feed.values()]
        self.feed_list = list(self.feed.keys())
        self.feed_dtype = [
            np_dtype_to_fluid_str(x.dtype) for x in self.feed.values()
        ]

    def set_attrs(self):
        self.attrs = {
            "optimizer": 'sgd',
            "weight_decay": 0.0,
        }

    def _test_optimizer(self, run_ipu=True):
        scope = fluid.core.Scope()
        main_prog = paddle.static.Program()
        startup_prog = paddle.static.Program()
        main_prog.random_seed = SEED
        startup_prog.random_seed = SEED
        np.random.seed(SEED)

        with fluid.scope_guard(scope):
            with paddle.static.program_guard(main_prog, startup_prog):
                image = paddle.static.data(
                    name='image', shape=[1, 3, 10, 10], dtype='float32')
                conv1 = paddle.static.nn.conv2d(
                    image, num_filters=3, filter_size=3, bias_attr=False)
                loss = paddle.mean(conv1)

                weight_decay = self.attrs['weight_decay']
                opt = paddle.optimizer.SGD(learning_rate=1e-1,
                                           weight_decay=weight_decay)
                if self.attrs['optimizer'] == 'adam':
                    opt = paddle.optimizer.Adam(
                        learning_rate=1e-1, weight_decay=weight_decay)
                elif self.attrs['optimizer'] == 'lamb':

                    opt = paddle.optimizer.Lamb(
                        learning_rate=1e-1, lamb_weight_decay=weight_decay)
                opt.minimize(loss)

            if run_ipu:
                place = paddle.IPUPlace()
            else:
                place = paddle.CPUPlace()
            exe = paddle.static.Executor(place)
            exe.run(startup_prog)

            if run_ipu:
                feed_list = [image.name]
                fetch_list = [loss.name]
                ipu_strategy = compiler.get_ipu_strategy()
                ipu_strategy.is_training = True
                program = compiler.IpuCompiler(
                    main_prog, ipu_strategy=ipu_strategy).compile(feed_list,
                                                                  fetch_list)
            else:
                program = main_prog

            result = []
            for epoch in range(100):
                loss_res = exe.run(program, feed=self.feed, fetch_list=[loss])
                result.append(loss_res)

            return np.array(result)

    def test(self):
        # cpu and ipu dimenstion mismatch, cpu:(100, 1, 1), ipu:(100, 1)
        ipu_loss = self._test_optimizer(True).flatten()
        cpu_loss = self._test_optimizer(False).flatten()

        self.assertTrue(np.allclose(ipu_loss, cpu_loss, atol=self.atol))


@unittest.skip('unsupported currently')
class TestSGD(TestBase):
    def set_attrs(self):
        self.attrs = {
            "optimizer": 'sgd',
            "weight_decay": 0.1,
        }


class TestAdamCase1(TestBase):
    def set_attrs(self):
        self.attrs = {
            "optimizer": 'adam',
            "weight_decay": 0.1,
        }


class TestAdamCase2(TestBase):
    def set_attrs(self):
        self.attrs = {
            "optimizer": 'adam',
            "weight_decay": 0.0,
        }


@unittest.skip('seems cpu output wrong')
class TestLambCase1(TestBase):
    def set_attrs(self):
        self.attrs = {
            "optimizer": 'lamb',
            "weight_decay": 0.0,
        }


@unittest.skip('seems cpu output wrong')
class TestLamb(TestBase):
    def set_attrs(self):
        self.attrs = {
            "optimizer": 'lamb',
            "weight_decay": 0.1,
        }


if __name__ == "__main__":
    unittest.main()
