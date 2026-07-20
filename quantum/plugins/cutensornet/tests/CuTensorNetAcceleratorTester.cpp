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
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "PauliOperator.hpp"

TEST(CuTensorNetAcceleratorTester, testDeuteron)
{
xacc::set_verbose(true);
    std::shared_ptr<xacc::quantum::PauliOperator> observable = std::make_shared<
      xacc::quantum::
          PauliOperator>("X0 X1");
    auto accelerator = xacc::getAccelerator("cutensornet", {{"bond-dimension", 2}, {"observable", observable}});
    //auto xasmCompiler = xacc::getCompiler("xasm");
    /*
    auto ir = xasmCompiler->compile(R"(__qpu__ void ansatz(qbit q, double t) {
      H(q[0]);
      CX(q[0], q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })", accelerator);
    
    auto ir = xasmCompiler->compile(R"(__qpu__ void ansatz(qbit q, double t) {
      H(q[0]);
      CX(q[0], q[1]);
      CX(q[1], q[2]);
      CX(q[2], q[3]);
      CX(q[3], q[4]);
      CX(q[4], q[5]);
      CX(q[5], q[6]);
      CX(q[6], q[7]);
      CX(q[7], q[8]);
      CX(q[8], q[9]);
      CX(q[9], q[10]);
      CX(q[10], q[11]);
      CX(q[11], q[12]);
      CX(q[12], q[13]);
      CX(q[13], q[14]);
      CX(q[14], q[15]);
      S(q[0]);
    })", accelerator);

    auto program = ir->getComposite("ansatz");
        auto buffer = xacc::qalloc(16);
        accelerator->execute(buffer, program);// std::vector<std::shared_ptr<xacc::CompositeInstruction>>{program});
*/

  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void ansatz(qbit q, double t) {
      X(q[0]);
      Ry(q[1], t);
      CX(q[1], q[0]);
})", accelerator);

  auto program = ir->getComposite("ansatz");

  int c = 0;
  auto angles = xacc::linspace(-xacc::constants::pi, xacc::constants::pi, 20);
  for (auto &a : angles) {
    auto buffer = xacc::qalloc(2);
    auto evaled = program->operator()({a});
    accelerator->execute(buffer, evaled);
    std::cout << "<X0X1>(theta) = " << buffer->getExpectationValueZ() << "\n";
  }

}

int main(int argc, char **argv) {
  xacc::Initialize();

  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();

  xacc::Finalize();

  return result;
}
