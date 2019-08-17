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
#include "InstructionIterator.hpp"
#include <gtest/gtest.h>
#include "Compiler.hpp"
// #include "CircuitOptimizer.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "CommonGates.hpp"

#include "XACC.hpp"
#include "xacc_service.hpp"
#include "IRTransformation.hpp"

using namespace xacc;
using namespace xacc::quantum;
const std::string uccsdSrc = R"uccsdSrc(__qpu__ void foo77(qbit q, double theta0, double theta1) {
   X(q[0]);
   X(q[1]);
   H(q[3]);
   Rx(q[1], pi/2);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], theta0);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   H(q[3]);
   Rx(q[1], -pi/2);
   Rx(q[3], pi/2);
   H(q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], -theta0);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   Rx(q[3], -pi/2);
   H(q[1]);
   H(q[3]);
   Rx(q[2], pi/2);
   H(q[1]);
   H(q[0]);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   H(q[3]);
   Rx(q[2], -pi/2);
   H(q[1]);
   H(q[0]);
   Rx(q[3], pi/2);
   Rx(q[2], pi/2);
   Rx(q[1], pi/2);
   H(q[0]);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   Rx(q[3], -pi/2);
   Rx(q[2], -pi/2);
   Rx(q[1], -pi/2);
   H(q[0]);
   H(q[3]);
   H(q[2]);
   H(q[1]);
   Rx(q[0], pi/2);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], -theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   H(q[3]);
   H(q[2]);
   H(q[1]);
   Rx(q[0], -pi/2);
   Rx(q[3], pi/2);
   H(q[2]);
   Rx(q[1], pi/2);
   Rx(q[0], pi/2);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], -theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   Rx(q[3], -pi/2);
   H(q[2]);
   Rx(q[1], -pi/2);
   Rx(q[0], -pi/2);
   Rx(q[3], pi/2);
   H(q[2]);
   H(q[1]);
   H(q[0]);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   Rx(q[3], -pi/2);
   H(q[2]);
   H(q[1]);
   H(q[0]);
   Rx(q[2], pi/2);
   H(q[0]);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   Rz(q[2], -theta0);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   Rx(q[2], -pi/2);
   H(q[0]);
   H(q[2]);
   Rx(q[0], pi/2);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   Rz(q[2], theta0);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   H(q[2]);
   Rx(q[0], -pi/2);
   Rx(q[3], pi/2);
   Rx(q[2], pi/2);
   H(q[1]);
   Rx(q[0], pi/2);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   Rx(q[3], -pi/2);
   Rx(q[2], -pi/2);
   H(q[1]);
   Rx(q[0], -pi/2);
   H(q[3]);
   Rx(q[2], pi/2);
   Rx(q[1], pi/2);
   Rx(q[0], pi/2);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], -theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   H(q[3]);
   Rx(q[2], -pi/2);
   Rx(q[1], -pi/2);
   Rx(q[0], -pi/2);
   H(q[3]);
   H(q[2]);
   Rx(q[1], pi/2);
   H(q[0]);
   CNOT(q[0], q[1]);
   CNOT(q[1], q[2]);
   CNOT(q[2], q[3]);
   Rz(q[3], -theta1/2);
   CNOT(q[2], q[3]);
   CNOT(q[1], q[2]);
   CNOT(q[0], q[1]);
   H(q[3]);
   H(q[2]);
   Rx(q[1], -pi/2);
   H(q[0]);
   }
   )uccsdSrc";

TEST(CircuitOptimizerTester, checkOptimize) {

    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto f = c->compile(uccsdSrc)->getComposites()[0];
    std::vector<double> p{0.0,0.0};
    auto fevaled = (*f)(p);

    auto provider = xacc::getIRProvider("quantum");

    auto ir = provider->createIR();
    ir->addComposite(fevaled);

    CountGatesOfTypeVisitor<CNOT> countCx(fevaled);

    auto opt = xacc::getService<IRTransformation>("circuit-optimizer");
    auto newir = opt->transform(ir);

    auto optF = newir->getComposites()[0];

    optF = optF->enabledView();//std::dynamic_pointer_cast<Circuit>(optF->enabledView());
    CountGatesOfTypeVisitor<CNOT> countCx2(optF);
    CountGatesOfTypeVisitor<Rz> countRz(optF);
    EXPECT_EQ(0, countRz.countGates());
    EXPECT_EQ(0, countCx2.countGates());
    EXPECT_EQ(2, optF->nInstructions());
    EXPECT_EQ("X", optF->getInstruction(0)->name());
    EXPECT_EQ("X", optF->getInstruction(1)->name());
    EXPECT_EQ(std::vector<std::size_t>{0}, optF->getInstruction(0)->bits());
    EXPECT_EQ(std::vector<std::size_t>{1}, optF->getInstruction(1)->bits());

    std::cout << "FINAL CIRCUIT:\n" << optF->toString() << "\n";

}

TEST(CircuitOptimizerTester, checkAdjRotations) {
    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto f = c->compile(R"(__qpu__ void foo(qbit q) {
        H(q[0]);
        Rx(q[1], pi/2);
        Rx(q[1], -pi/2);
        H(q[2]);
        Rz(q[3], 0.5);
        Rz(q[3], 0.5);
    })")
                 ->getComposites()[0];
     auto provider = xacc::getIRProvider("quantum");

    auto ir = provider->createIR();

    ir->addComposite(f);

    auto opt = xacc::getService<IRTransformation>("circuit-optimizer");

    auto newir = opt->transform(ir);
    auto optF = newir->getComposites()[0];

    optF = optF->enabledView();
    CountGatesOfTypeVisitor<Rz> countRz(optF);
    CountGatesOfTypeVisitor<Rx> countRx(optF);

    EXPECT_EQ(3, optF->nInstructions());
    EXPECT_EQ(1, countRz.countGates());
    EXPECT_EQ(0, countRx.countGates());
    EXPECT_TRUE(optF->getInstruction(2)->getParameter(0).as<double>()==1.0 );
    std::cout << "FINAL CIRCUIT:\n" << optF->toString() << "\n";

}

TEST(CircuitOptimizerTester, checkSimple) {

    auto c = xacc::getService<xacc::Compiler>("xasm");
    auto f = c->compile(R"(__qpu__ void foo1(qbit q) {
        CNOT(q[0],q[1]);
        CNOT(q[0],q[1]);
    })")
                 ->getComposites()[0];

  auto provider = xacc::getIRProvider("quantum");

    auto ir = provider->createIR();
    ir->addComposite(f);

    auto opt = xacc::getService<IRTransformation>("circuit-optimizer");
    auto newir = opt->transform(ir);
    auto optF = newir->getComposites()[0];

    optF = optF->enabledView();
    EXPECT_EQ(0, optF->nInstructions());

    f = c->compile(R"(__qpu__ void foo2(qbit q) {
        H(q[0]);
        CNOT(q[0],q[1]);
        CNOT(q[0],q[1]);
        H(q[0]);
    })")
            ->getComposites()[0];

    ir = provider->createIR();
    ir->addComposite(f);

    newir = opt->transform(ir);
    optF = newir->getComposites()[0];
    optF = optF->enabledView();

    EXPECT_EQ(0, optF->nInstructions());

    f = c->compile(R"(__qpu__ void foo3(qbit q) {
        CNOT(q[0],q[1]);
        H(q[1]);
        H(q[1]);
        CNOT(q[0],q[1]);
    })")
            ->getComposites()[0];

    ir = provider->createIR();
    ir->addComposite(f);

    newir = opt->transform(ir);
    optF = newir->getComposites()[0];

    optF = optF->enabledView();
    EXPECT_EQ(0, optF->nInstructions());

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
