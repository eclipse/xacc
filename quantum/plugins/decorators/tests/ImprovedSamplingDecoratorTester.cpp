/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "AcceleratorDecorator.hpp"
#include "xacc_service.hpp"

using namespace xacc;


TEST(ImprovedSamplingDecoratorTester, checkSimple) {
  int shots = 8192;
  int nExecs = 4;

  if (xacc::hasAccelerator("qpp")) {
    auto acc = xacc::getAccelerator("qpp", {std::make_pair("shots",shots)});
    auto buffer = xacc::qalloc(2);

    auto compiler = xacc::getService<xacc::Compiler>("xasm");
    const std::string src = R"src(__qpu__ void f(qbit q) {
       H(q[0]);
       Measure(q[0]);
       } )src";

    auto ir = compiler->compile(src, acc);
    auto f = ir->getComposite("f");

    auto decorator = xacc::getService<AcceleratorDecorator>("improved-sampling");
    decorator->setDecorated(acc);
    xacc::setOption("ibm-shots", std::to_string(shots));
    xacc::setOption("sampler-n-execs", std::to_string(nExecs));

    decorator->execute(buffer, f);

    int nshots = 0;
    auto counts = buffer->getMeasurementCounts();
    buffer->print();
    for (auto &kv : counts) {
      nshots += kv.second;
    }

    EXPECT_EQ(nshots, nExecs * shots);
  }
}

TEST(ImprovedSamplingDecoratorTester, checkMultiple) {
  int shots = 8192;
  int nExecs = 2;

  if (xacc::hasAccelerator("qpp")) {
    auto acc = xacc::getAccelerator("qpp",{std::make_pair("shots",shots)});
    auto buffer = xacc::qalloc(2);

    auto compiler = xacc::getService<xacc::Compiler>("xasm");
    const std::string src = R"src(__qpu__ void f2(qbit q) {
       H(q[0]);
       Measure(q[0]);
       } )src";

    const std::string src2 = R"src2(__qpu__ void g(qbit q) {
       H(q[0]);
       Measure(q[0]);
       } )src2";

    auto ir = compiler->compile(src, acc);
    auto ir2 = compiler->compile(src2, acc);

    auto f = ir->getComposite("f2");
    auto g = ir2->getComposite("g");

     auto decorator = xacc::getService<AcceleratorDecorator>("improved-sampling");
    decorator->setDecorated(acc);
    xacc::setOption("ibm-shots", std::to_string(shots));
    xacc::setOption("sampler-n-execs", std::to_string(nExecs));

    decorator->execute(buffer, {f, g});
    auto buffers = buffer->getChildren();

    for (auto &b : buffers) {
      int nshots = 0;
      auto counts = b->getMeasurementCounts();
      b->print();
      for (auto &kv : counts) {
        nshots += kv.second;
      }

      EXPECT_EQ(nshots, nExecs * shots);
    }
  }
}
int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
