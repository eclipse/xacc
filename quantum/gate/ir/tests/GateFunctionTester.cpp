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
#include "CNOT.hpp"
#include "GateFunction.hpp"
#include "Hadamard.hpp"
#include "InstructionIterator.hpp"
#include "Rx.hpp"
#include "Ry.hpp"
#include "Rz.hpp"
#include "X.hpp"
#include "Z.hpp"
#include <gtest/gtest.h>
#include "Graph.hpp"

using namespace xacc::quantum;

const std::string expectedQasm = "H qreg1\n"
                                 "CNOT qreg1,qreg2\n"
                                 "CNOT qreg0,qreg1\n"
                                 "H qreg0\n";

TEST(GateFunctionTester, checkFunctionMethods) {

  GateFunction f("foo");
  EXPECT_TRUE(f.name() == "foo");
  EXPECT_TRUE(f.nInstructions() == 0);

  auto h = std::make_shared<Hadamard>(1);
  auto cn1 = std::make_shared<CNOT>(1, 2);
  auto cn2 = std::make_shared<CNOT>(0, 1);
  auto h2 = std::make_shared<Hadamard>(0);

  f.addInstruction(h);
  f.addInstruction(cn1);
  f.addInstruction(cn2);
  f.addInstruction(h2);

  EXPECT_TRUE(f.nInstructions() == 4);
  EXPECT_TRUE(f.toString("qreg") == expectedQasm);

  f.removeInstruction(0);
  EXPECT_TRUE(f.nInstructions() == 3);

  const std::string removeExpectedQasm = "CNOT qreg1,qreg2\n"
                                         "CNOT qreg0,qreg1\n"
                                         "H qreg0\n";
  EXPECT_TRUE(f.toString("qreg") == removeExpectedQasm);

  f.replaceInstruction(0, h);
  const std::string replaceExpectedQasm = "H qreg1\n"
                                          "CNOT qreg0,qreg1\n"
                                          "H qreg0\n";
  EXPECT_TRUE(f.toString("qreg") == replaceExpectedQasm);

  f.replaceInstruction(2, cn1);
  const std::string replaceExpectedQasm2 = "H qreg1\n"
                                           "CNOT qreg0,qreg1\n"
                                           "CNOT qreg1,qreg2\n";
  EXPECT_TRUE(f.toString("qreg") == replaceExpectedQasm2);
}

TEST(GateFunctionTester, checkWalkFunctionTree) {
  auto f = std::make_shared<GateFunction>("foo");
  auto g = std::make_shared<GateFunction>("goo");

  auto h = std::make_shared<Hadamard>(1);
  auto cn1 = std::make_shared<CNOT>(1, 2);
  auto cn2 = std::make_shared<CNOT>(0, 1);

  g->addInstruction(cn1);
  g->addInstruction(cn2);

  f->addInstruction(h);
  f->addInstruction(g);

  xacc::InstructionIterator it(f);
  while (it.hasNext()) {
    auto inst = it.next();
    if (!inst->isComposite())
      std::cout << inst->name() << "\n";
  }
}

TEST(GateFunctionTester, checkEvaluateVariables) {

  xacc::InstructionParameter p("phi");
  xacc::InstructionParameter fParam("phi");

  EXPECT_TRUE(p == fParam);

  auto rz = std::make_shared<Rz>(std::vector<int>{0});
  rz->setParameter(0, p);

  GateFunction f("foo", std::vector<xacc::InstructionParameter>{fParam});

  f.addInstruction(rz);

  std::cout << f.toString("qreg") << "\n";

  Eigen::VectorXd v(1);
  v(0) = 3.1415;

  auto evaled = f(v);

  EXPECT_TRUE(boost::get<double>(evaled->getInstruction(0)->getParameter(0)) ==
              3.1415);

  std::cout << "ParamSet:\n" << f.toString("qreg") << "\n";

  v(0) = 6.28;

  evaled = f(v);

  std::cout << "ParamSet:\n" << evaled->toString("qreg") << "\n";
}

TEST(GateFunctionTester, checkParameterInsertion) {

  xacc::InstructionParameter p1("theta");
  xacc::InstructionParameter p2("1 * theta");
  xacc::InstructionParameter p3("0.5 * psi");

  auto rz = std::make_shared<Rz>(std::vector<int>{1});
  auto ry = std::make_shared<Ry>(std::vector<int>{2});
  auto rx = std::make_shared<Rx>(std::vector<int>{3});

  rz->setParameter(0, p1);
  ry->setParameter(0, p2);
  rx->setParameter(0, p3);

  GateFunction f("foo");

  f.addInstruction(rz);
  f.addInstruction(ry);

  EXPECT_TRUE(f.nInstructions() == 2);
  EXPECT_TRUE(f.nParameters() == 1);
  EXPECT_TRUE(f.toString("qreg") == "Rz(theta) qreg1\nRy(1 * theta) qreg2\n");

  // replace duplicated parameter with a unique one
  f.replaceInstruction(1, rx);

  EXPECT_TRUE(f.nInstructions() == 2);
  EXPECT_TRUE(f.nParameters() == 2);
  EXPECT_TRUE(f.toString("qreg") == "Rz(theta) qreg1\nRx(0.5 * psi) qreg3\n");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "theta");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(1)) == "psi");

  // replace unique Instruction parameter with a duplicated parameter
  f.replaceInstruction(1, ry);

  EXPECT_TRUE(f.nInstructions() == 2);
  EXPECT_TRUE(f.nParameters() == 1);
  EXPECT_TRUE(f.toString("qreg") == "Rz(theta) qreg1\nRy(1 * theta) qreg2\n");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "theta");
  // make sure parameters get removed completely
  f.removeInstruction(0);
  EXPECT_TRUE(f.nParameters() == 1);
  f.removeInstruction(0);

  EXPECT_TRUE(f.nInstructions() == 0);
  EXPECT_TRUE(f.nParameters() == 0);

  auto x = std::make_shared<X>(std::vector<int>{2});

  f.addInstruction(x);

  EXPECT_TRUE(f.nParameters() == 0);
  EXPECT_TRUE(f.nInstructions() == 1);

  f.addInstruction(rz);
  // replace an Instruction without a parameter with a parameterized Instruction
  f.replaceInstruction(0, rx);

  EXPECT_TRUE(f.nParameters() == 2);
  EXPECT_TRUE(f.nInstructions() == 2);
  EXPECT_TRUE(f.toString("qreg") == "Rx(0.5 * psi) qreg3\nRz(theta) qreg1\n");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "theta");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(1)) == "psi");

  // replace a parameterized Instruction with one that does not have a parameter
  f.replaceInstruction(0, x);

  EXPECT_TRUE(f.nParameters() == 1);
  EXPECT_TRUE(f.nInstructions() == 2);
  EXPECT_TRUE(f.toString("qreg") == "X qreg2\nRz(theta) qreg1\n");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "theta");

  f.replaceInstruction(0, rx);

  f.addInstruction(ry);

  EXPECT_TRUE(f.nParameters() == 2);
  EXPECT_TRUE(f.nInstructions() == 3);
  EXPECT_TRUE(f.toString("qreg") ==
              "Rx(0.5 * psi) qreg3\nRz(theta) qreg1\nRy(1 * theta) qreg2\n");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "theta");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(1)) == "psi");

  auto rx2 = std::make_shared<Rx>(std::vector<int>{4});
  xacc::InstructionParameter p5("1 * phi");
  rx2->setParameter(0, p5);
  // replacing duplicated parameter with a brand new parameter
  f.replaceInstruction(1, rx2);

  EXPECT_TRUE(f.nParameters() == 3);
  EXPECT_TRUE(f.nInstructions() == 3);
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(0)) == "theta");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(1)) == "psi");
  EXPECT_TRUE(boost::get<std::string>(f.getParameter(2)) == "phi");

  EXPECT_TRUE(f.toString("qreg") ==
              "Rx(0.5 * psi) qreg3\nRx(1 * phi) qreg4\nRy(1 * theta) qreg2\n");
}

TEST(GateFunctionTester, checkGenerateGraph) {

  auto f = std::make_shared<GateFunction>("foo");
  auto h = std::make_shared<Hadamard>(1);
  auto cn1 = std::make_shared<CNOT>(1, 2);
  auto cn2 = std::make_shared<CNOT>(0, 1);
  auto h2 = std::make_shared<Hadamard>(0);
  auto rz = std::make_shared<Rz>(2, 3.1415);
  f->addInstruction(h);
  f->addInstruction(cn1);
  f->addInstruction(cn2);
  f->addInstruction(h2);
  f->addInstruction(rz);

  auto g = f->toGraph();

  std::stringstream ss;
  g.write(ss);

  std::string expected = R"expected(digraph G {
node [shape=box style=filled]
0 [label="gate=InitialState;id=0;bits=[0, 1, 2]"];
1 [label="gate=H;id=1;bits=[1]"];
2 [label="gate=CNOT;id=2;bits=[1, 2]"];
3 [label="gate=CNOT;id=3;bits=[0, 1]"];
4 [label="gate=H;id=4;bits=[0]"];
5 [label="gate=Rz;id=5;bits=[2]"];
6 [label="gate=FinalState;id=6;bits=[0, 1, 2]"];
0->1 ;
0->2 ;
0->3 ;
1->2 ;
2->3 ;
2->5 ;
3->4 ;
3->6 ;
4->6 ;
5->6 ;
})expected";

  std::cout << ss.str() << "\n\n" << expected << "\n";
  EXPECT_TRUE(expected == ss.str());
}

TEST(GateFunctionTester, checkDepth) {
  auto f = std::make_shared<GateFunction>("foo");
  auto x = std::make_shared<X>(0);
  auto h = std::make_shared<Hadamard>(1);
  auto cn1 = std::make_shared<CNOT>(1, 2);
  auto rz = std::make_shared<Rz>(1, 3.1415);
  auto z = std::make_shared<Z>(2);

  f->addInstruction(x);
  f->addInstruction(h);
  f->addInstruction(cn1);
  f->addInstruction(rz);
  f->addInstruction(z);

  auto g = f->toGraph();

  std::cout << "DEPTH: " << g.depth() << "\n";
  EXPECT_EQ(3, g.depth());
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
