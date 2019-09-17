/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
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
#include <memory>
#include <gtest/gtest.h>
#include "QCSAccelerator.hpp"

#include "xacc_service.hpp"

using namespace xacc::quantum;

TEST(QCSTester, checkSimpleTransformation) {
  int shots = 8192;
  int nExecs = 4;
  xacc::setOption("qcs-shots", "10000");
    xacc::setOption("qcs-backend","Aspen-4-4Q-A");
    auto acc = xacc::getAccelerator("qcs");
    auto buffer = acc->createBuffer("buffer", 4);

    auto compiler = xacc::getService<xacc::Compiler>("xacc-py");
    const std::string src = R"src(def f(buffer):
       CX(0,1)
       CX(1,2)
       CX(2,3)
       Measure(0, 0)
       Measure(1, 1)
       Measure(2, 2)
       )src";

    auto ir = compiler->compile(src, acc);
    auto f = ir->getKernel("f");



    auto t = acc->getIRTransformations()[0];
    ir = t->transform(ir);

    std::cout << "TEST:\n" << ir->getKernels()[0]->toString() << "\n";

}


int main(int argc, char **argv) {
    xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
