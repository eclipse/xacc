/***********************************************************************************
 * Copyright (c) 2018, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#include <memory>
#include <gtest/gtest.h>
#include "QCSAccelerator.hpp"

#include "xacc_service.hpp"

using namespace xacc::quantum;

TEST(QCSTester, checkSimple) {
  int shots = 8192;
  int nExecs = 4;

    auto acc = xacc::getAccelerator("qcs");
    auto buffer = acc->createBuffer("buffer", 17);

    auto compiler = xacc::getService<xacc::Compiler>("xacc-py");
    const std::string src = R"src(def f(buffer):
       Rx(1.57,1)
       Rx(1.57,14)
       Rx(1.57,15)
       Measure(0, 1)
       Measure(1, 14)
       Measure(2, 15)
       )src";

    auto ir = compiler->compile(src, acc);
    auto f = ir->getKernel("f");

    xacc::setOption("qcs-shots", "10000");
    xacc::setOption("qcs-backend","Aspen-1-4Q-G");
    // acc->execute(buffer, f);

    // // int nshots = 0;
    // auto counts = buffer->getMeasurementCounts();
    // buffer->print();
    // std::cout << "\n\n";
    // for (auto &kv : counts) {
    //     std::cout << kv.first << ":" << kv.second << "\n";
    // }

    // EXPECT_EQ(nshots, nExecs * shots);
}


int main(int argc, char **argv) {
    xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
