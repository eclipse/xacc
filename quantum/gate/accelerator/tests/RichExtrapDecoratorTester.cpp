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
#include "XACC.hpp"
#include "RichExtrapDecorator.hpp"

using namespace xacc;

using namespace xacc::quantum;

TEST(RichExtrapDecoratorTester, checkSimple) {
  int shots = 8192;
  int nExecs = 4;

  if (xacc::hasAccelerator("tnqvm")) {
    auto acc = xacc::getAccelerator("tnqvm");
    auto buffer = acc->createBuffer("buffer", 2);

    auto compiler = xacc::getService<xacc::Compiler>("xacc-py");
    const std::string src = R"src(def f(buffer):
       H(0)
       CNOT(0,1)
       Measure(0,0)
       )src";

    auto ir = compiler->compile(src, acc);
    auto f = ir->getKernel("f");

    xacc::setOption("rich-extrap-r", "1");
    
    RichExtrapDecorator decorator;
    decorator.setDecorated(acc);

    decorator.execute(buffer, f);

    
  }
}

TEST(ImprovedSamplingDecoratorTester, checkMultiple) {
  int shots = 8192;
  int nExecs = 2;

  if (xacc::hasAccelerator("tnqvm")) {
    auto acc = xacc::getAccelerator("tnqvm");
    auto buffer = acc->createBuffer("buffer", 2);

    auto compiler = xacc::getService<xacc::Compiler>("xacc-py");
    const std::string src = R"src(def f(buffer):
       H(0)
       CNOT(0,1)
       Measure(0,0)
       )src";

    const std::string src2 = R"src(def g(buffer):
       H(0)
       CNOT(0,1)
       Measure(0,0)
       )src";

    auto ir = compiler->compile(src, acc);
    auto ir2 = compiler->compile(src2, acc);

    auto f = ir->getKernel("f");
    auto g = ir2->getKernel("g");

    RichExtrapDecorator decorator;
    decorator.setDecorated(acc);
    
    xacc::setOption("rich-extrap-r", "3");

    auto buffers = decorator.execute(buffer, {f, g});

   
  }
}
int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
