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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/

#include "xacc.hpp"
#include <gtest/gtest.h>
#include "xacc_observable.hpp"

TEST(AerAcceleratorPulseTester, checkSimple) {
  auto accelerator =
      xacc::getAccelerator("aer:ibmq_armonk", {{"sim-type", "pulse"}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void test(qbit q) {
      H(q[0]);
      Measure(q[0]);
    })",
                                  accelerator);

  auto program = ir->getComposite("test");

  auto buffer = xacc::qalloc(1);
  accelerator->execute(buffer, program);

  buffer->print();
}

int main(int argc, char **argv) {
  xacc::Initialize();

  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();

  xacc::Finalize();

  return result;
}
