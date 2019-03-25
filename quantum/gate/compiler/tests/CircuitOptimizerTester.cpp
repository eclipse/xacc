/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#include "GateFunction.hpp"
#include "InstructionIterator.hpp"
#include <gtest/gtest.h>
#include "Compiler.hpp"
#include "CircuitOptimizer.hpp"
#include "GateIR.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "Rz.hpp"
#include "CNOT.hpp"

using namespace xacc::quantum;
const std::string uccsdSrc = R"uccsdSrc(def foo(theta0,theta1):
   X(0)
   X(1)
   H(3)
   Rx(1.5708, 1)
   CNOT(1, 2)
   CNOT(2, 3)
   Rz(1 * theta0, 3)
   CNOT(2, 3)
   CNOT(1, 2)
   H(3)
   Rx(-1.5708, 1)
   Rx(1.5708, 3)
   H(1)
   CNOT(1, 2)
   CNOT(2, 3)
   Rz(-1 * theta0, 3)
   CNOT(2, 3)
   CNOT(1, 2)
   Rx(-1.5708, 3)
   H(1)
   H(3)
   Rx(1.5708, 2)
   H(1)
   H(0)
   CNOT(0, 1)
   CNOT(1, 2)
   CNOT(2, 3)
   Rz(0.5 * theta1, 3)
   CNOT(2, 3)
   CNOT(1, 2)
   CNOT(0, 1)
   H(3)
   Rx(-1.5708, 2)
   H(1)
   H(0)
   Rx(1.5708, 3)
   Rx(1.5708, 2)
   Rx(1.5708, 1)
   H(0)
   CNOT(0, 1)
   CNOT(1, 2)
   CNOT(2, 3)
   Rz(0.5 * theta1, 3)
   CNOT(2, 3)
   CNOT(1, 2)
   CNOT(0, 1)
   Rx(-1.5708, 3)
   Rx(-1.5708, 2)
   Rx(-1.5708, 1)
   H(0)
   H(3)
   H(2)
   H(1)
   Rx(1.5708, 0)
   CNOT(0, 1)
   CNOT(1, 2)
   CNOT(2, 3)
   Rz(-0.5 * theta1, 3)
   CNOT(2, 3)
   CNOT(1, 2)
   CNOT(0, 1)
   H(3)
   H(2)
   H(1)
   Rx(-1.5708, 0)
   Rx(1.5708, 3)
   H(2)
   Rx(1.5708, 1)
   Rx(1.5708, 0)
   CNOT(0, 1)
   CNOT(1, 2)
   CNOT(2, 3)
   Rz(-0.5 * theta1, 3)
   CNOT(2, 3)
   CNOT(1, 2)
   CNOT(0, 1)
   Rx(-1.5708, 3)
   H(2)
   Rx(-1.5708, 1)
   Rx(-1.5708, 0)
   Rx(1.5708, 3)
   H(2)
   H(1)
   H(0)
   CNOT(0, 1)
   CNOT(1, 2)
   CNOT(2, 3)
   Rz(0.5 * theta1, 3)
   CNOT(2, 3)
   CNOT(1, 2)
   CNOT(0, 1)
   Rx(-1.5708, 3)
   H(2)
   H(1)
   H(0)
   Rx(1.5708, 2)
   H(0)
   CNOT(0, 1)
   CNOT(1, 2)
   Rz(-1 * theta0, 2)
   CNOT(1, 2)
   CNOT(0, 1)
   Rx(-1.5708, 2)
   H(0)
   H(2)
   Rx(1.5708, 0)
   CNOT(0, 1)
   CNOT(1, 2)
   Rz(1 * theta0, 2)
   CNOT(1, 2)
   CNOT(0, 1)
   H(2)
   Rx(-1.5708, 0)
   Rx(1.5708, 3)
   Rx(1.5708, 2)
   H(1)
   Rx(1.5708, 0)
   CNOT(0, 1)
   CNOT(1, 2)
   CNOT(2, 3)
   Rz(0.5 * theta1, 3)
   CNOT(2, 3)
   CNOT(1, 2)
   CNOT(0, 1)
   Rx(-1.5708, 3)
   Rx(-1.5708, 2)
   H(1)
   Rx(-1.5708, 0)
   H(3)
   Rx(1.5708, 2)
   Rx(1.5708, 1)
   Rx(1.5708, 0)
   CNOT(0, 1)
   CNOT(1, 2)
   CNOT(2, 3)
   Rz(-0.5 * theta1, 3)
   CNOT(2, 3)
   CNOT(1, 2)
   CNOT(0, 1)
   H(3)
   Rx(-1.5708, 2)
   Rx(-1.5708, 1)
   Rx(-1.5708, 0)
   H(3)
   H(2)
   Rx(1.5708, 1)
   H(0)
   CNOT(0, 1)
   CNOT(1, 2)
   CNOT(2, 3)
   Rz(-0.5 * theta1, 3)
   CNOT(2, 3)
   CNOT(1, 2)
   CNOT(0, 1)
   H(3)
   H(2)
   Rx(-1.5708, 1)
   H(0)
   )uccsdSrc";

TEST(CircuitOptimizerTester, checkOptimize) {

  if (xacc::hasCompiler("xacc-py")) {
    auto c = xacc::getService<xacc::Compiler>("xacc-py");
    auto f = c->compile(uccsdSrc)->getKernels()[0];
    std::vector<double> p{0.0,0.0};
    auto fevaled = (*f)(p);

    auto ir = std::make_shared<GateIR>();
    ir->addKernel(fevaled);

    CountGatesOfTypeVisitor<CNOT> countCx(fevaled);

    CircuitOptimizer opt;
    auto newir = opt.transform(ir);

    auto optF = newir->getKernels()[0];

    optF = std::dynamic_pointer_cast<GateFunction>(optF->enabledView());
    CountGatesOfTypeVisitor<CNOT> countCx2(optF);
    CountGatesOfTypeVisitor<Rz> countRz(optF);
    EXPECT_EQ(0, countRz.countGates());
    EXPECT_EQ(0, countCx2.countGates());
    EXPECT_EQ(2, optF->nInstructions());
    EXPECT_EQ("X", optF->getInstruction(0)->name());
    EXPECT_EQ("X", optF->getInstruction(1)->name());
    EXPECT_EQ(std::vector<int>{0}, optF->getInstruction(0)->bits());
    EXPECT_EQ(std::vector<int>{1}, optF->getInstruction(1)->bits());

    std::cout << "FINAL CIRCUIT:\n" << optF->toString("q") << "\n";
  }
}

TEST(CircuitOptimizerTester, checkSimple) {

  if (xacc::hasCompiler("xacc-py")) {
    auto c = xacc::getService<xacc::Compiler>("xacc-py");
    auto f = c->compile("def foo(buffer):\n   CNOT(0,1)\n   CNOT(0,1)\n")
                 ->getKernels()[0];

    auto ir = std::make_shared<GateIR>();
    ir->addKernel(f);

    CircuitOptimizer opt;
    auto newir = opt.transform(ir);
    auto optF = newir->getKernels()[0];

    optF = std::dynamic_pointer_cast<GateFunction>(optF->enabledView());
    EXPECT_EQ(0, optF->nInstructions());

    f = c->compile(
             "def foo(buffer):\n   H(0)\n   CNOT(0,1)\n   CNOT(0,1)\n   H(0)\n")
            ->getKernels()[0];

    ir = std::make_shared<GateIR>();
    ir->addKernel(f);

    newir = opt.transform(ir);
    optF = newir->getKernels()[0];
    optF = std::dynamic_pointer_cast<GateFunction>(optF->enabledView());

    EXPECT_EQ(0, optF->nInstructions());

    f = c->compile(
             "def foo(buffer):\n   CNOT(0,1)\n   H(1)\n   H(1)\n   CNOT(0,1)\n")
            ->getKernels()[0];

    ir = std::make_shared<GateIR>();
    ir->addKernel(f);

    newir = opt.transform(ir);
    optF = newir->getKernels()[0];

    optF = std::dynamic_pointer_cast<GateFunction>(optF->enabledView());
    EXPECT_EQ(0, optF->nInstructions());
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
