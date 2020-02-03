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
#include <gtest/gtest.h>

#include "xacc.hpp"
#include "xacc_service.hpp"

#include "PauliOperator.hpp"

using namespace xacc;

TEST(QPTTester, checkHadamard) {
    if (xacc::hasAccelerator("aer")) {
    auto acc = xacc::getAccelerator("aer");
    auto buffer = xacc::qalloc(1);

    auto compiler = xacc::getCompiler("xasm");

    auto ir = compiler->compile(R"(__qpu__ void f(qbit q) {
        H(q[0]);
    })", nullptr);
    auto h = ir->getComposite("f");


    auto qpt = xacc::getService<Algorithm>("qpt");
    EXPECT_TRUE(qpt->initialize({std::make_pair("circuit",h),
                                std::make_pair("accelerator",acc)}));
    qpt->execute(buffer);
    }
}


TEST(QPTTester, checkBell) {
    if (xacc::hasAccelerator("aer")) {
    auto acc = xacc::getAccelerator("aer");
    auto buffer = xacc::qalloc(2);

    auto compiler = xacc::getCompiler("xasm");

    auto ir = compiler->compile(R"(__qpu__ void bell(qbit q) {
        H(q[0]);
        CX(q[0],q[1]);
    })", nullptr);
    auto h = ir->getComposite("bell");


    auto qpt = xacc::getService<Algorithm>("qpt");
    EXPECT_TRUE(qpt->initialize({std::make_pair("circuit",h),
                                std::make_pair("accelerator",acc)}));
    qpt->execute(buffer);
    }
}
int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
      xacc::external::load_external_language_plugins();

  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
