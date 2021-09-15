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
#include <stdexcept>
#include "CommonGates.hpp"
#include "Circuit.hpp"
#include "xacc.hpp"
using namespace xacc::quantum;

TEST(GateTester, checkIR3) {

  /* mimic the following 
  *
  * __qpu__ foo (qbit q, double x, double y, double z) {
  *   Rx(q[0], x);
  *   U(q[0], x, y, z);
  * }
  *
  */
  
  auto circuit = std::make_shared<Circuit>("foo");
  circuit->addVariable("x");
  circuit->addVariable("y");
  circuit->addVariable("z");

  circuit->addArgument("x", "double");
  circuit->addArgument("y", "double");
  circuit->addArgument("z", "double");

  auto argx = circuit->getArgument("x");
  auto argy = circuit->getArgument("y");
  auto argz = circuit->getArgument("z");

  auto rx = std::make_shared<Rx>(0, "x");
  rx->addArgument(argx, 0);

  auto u = std::make_shared<U>(0, std::vector<xacc::InstructionParameter>{"x", "y", "z"});
  u->addArgument(argx, 0);
  u->addArgument(argy, 1);
  u->addArgument(argz, 2);

  circuit->addInstructions({rx,u});

  EXPECT_THROW(circuit->updateRuntimeArguments(2.2), std::runtime_error);

  circuit->updateRuntimeArguments(2.2, 3.3, 4.4);

  std::cout << circuit->toString() <<"\n";
  
  circuit->updateRuntimeArguments(3.2, 4.3, 5.3);
  
  std::cout << circuit->toString() <<"\n";

}

TEST(GateTester, checkBugBug) {
     auto cnot = std::make_shared<CNOT>(0,1);
     cnot->setBufferNames({"a","b"});
     std::cout << "CNOT:\n" << cnot->toString() << "\n";
}

TEST(GateTester, checkBasicGatesAndCircuits) {

  auto h = std::make_shared<Hadamard>(0);
  auto cx = std::make_shared<CNOT>(0, 1);
  auto m0 = std::make_shared<Measure>(0);
  auto ry = std::make_shared<Ry>(1, "theta");

  auto circuit = std::make_shared<Circuit>("foo");
  EXPECT_EQ("foo", circuit->name());
  circuit->addInstruction(h);
  circuit->addInstruction(cx);
  circuit->addInstruction(m0);

  EXPECT_ANY_THROW(circuit->addInstruction(ry));
  EXPECT_EQ(3, circuit->nInstructions());

  circuit->addVariable("theta");
  circuit->addInstruction(ry);

  EXPECT_EQ(4, circuit->nInstructions());
  EXPECT_FALSE(circuit->isAnalog());
  EXPECT_TRUE(circuit->isComposite());
  EXPECT_EQ("H", circuit->getInstruction(0)->name());
  EXPECT_EQ("Ry", circuit->getInstruction(3)->name());

  circuit->removeInstruction(2);
  EXPECT_EQ(3, circuit->nInstructions());
  EXPECT_EQ("H", circuit->getInstruction(0)->name());
  EXPECT_EQ("CNOT", circuit->getInstruction(1)->name());
  EXPECT_EQ("Ry", circuit->getInstruction(2)->name());

  circuit->replaceInstruction(2, m0);
  EXPECT_EQ(3, circuit->nInstructions());
  EXPECT_EQ("H", circuit->getInstruction(0)->name());
  EXPECT_EQ("CNOT", circuit->getInstruction(1)->name());
  EXPECT_EQ("Measure", circuit->getInstruction(2)->name());

  circuit->replaceInstruction(2, ry);
  circuit->addInstruction(m0);

  EXPECT_EQ(4, circuit->nInstructions());
  EXPECT_EQ("H", circuit->getInstruction(0)->name());
  EXPECT_EQ("CNOT", circuit->getInstruction(1)->name());
  EXPECT_EQ("Ry", circuit->getInstruction(2)->name());
  EXPECT_EQ("Measure", circuit->getInstruction(3)->name());

  EXPECT_EQ("theta",
            circuit->getInstruction(2)->getParameter(0).as<std::string>());
  auto evaled = circuit->operator()({2.2});
  EXPECT_EQ(2.2, evaled->getInstruction(2)->getParameter(0).as<double>());

  auto ry0 = std::make_shared<Ry>(0, "t0");
  auto ry1 = std::make_shared<Ry>(1, "t1");
  auto ry2 = std::make_shared<Ry>(2, "t2");
  auto ry3 = std::make_shared<Ry>(3, "t3");
  auto ry4 = std::make_shared<Ry>(4, "t4");
  auto ry5 = std::make_shared<Ry>(5, "-pi + 3*pi");
  auto ry6 = std::make_shared<Ry>(5, "-pi + 3*t0 + 2*t1");
  auto ry7 = std::make_shared<Ry>(5, "-pi + 3*t0 + 2*t1 + 33*s0");

  auto bar = std::make_shared<Circuit>("bar", std::vector<std::string>{"t0", "t1", "t2", "t3", "t4"});
  bar->addInstruction(ry0);
  bar->addInstruction(ry1);
  bar->addInstruction(ry2);
  bar->addInstruction(ry3);
  bar->addInstruction(ry4);
  bar->addInstruction(ry5);
  bar->addInstruction(ry6);

  std::cout << bar->toString() << "\n";

  EXPECT_ANY_THROW(bar->addInstruction(ry7));

  bar->addVariable("s0");
  bar->addInstruction(ry7);
  evaled = bar->operator()({.22, .33, .44, .55, .66, .77});
  std::cout << "\n" << evaled->toString() << "\n";

  EXPECT_NEAR(.22, evaled->getInstruction(0)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(.33, evaled->getInstruction(1)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(.44, evaled->getInstruction(2)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(.55, evaled->getInstruction(3)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(.66, evaled->getInstruction(4)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(6.28319, evaled->getInstruction(5)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(-1.82159, evaled->getInstruction(6)->getParameter(0).as<double>(),
              1e-4);
  EXPECT_NEAR(23.5884, evaled->getInstruction(7)->getParameter(0).as<double>(),
              1e-4);

  bar->getInstruction(0)->disable();
  EXPECT_EQ(8, bar->nInstructions());
  auto enabled = bar->enabledView();
  EXPECT_EQ(7, enabled->nInstructions());

  bar->getInstruction(0)->enable();

  std::cout << "Bar before:\n" << bar->toString() << "\n";

  bar->getInstruction(4)->disable();

  bar->removeDisabled();

  std::cout << "Bar after:\n" << bar->toString() << "\n";
//   EXPECT_TRUE(false);

}

TEST(GateTester, checkTerminatingNode) {
  class sub_circuit : public Circuit {
  public:
    sub_circuit(const std::string name) : Circuit(name) {}
    bool expand(const xacc::HeterogeneousMap &runtimeOptions) override {
      if (!runtimeOptions.keyExists<int>("key1") &&
          !runtimeOptions.keyExists<int>("key2")) {
        return false;
      }

      auto val1 = runtimeOptions.get<int>("key1");
      auto val2 = runtimeOptions.get<int>("key2");

      for (int i = 0; i < val1; i++) {
        auto h = std::make_shared<Hadamard>(i);
        addInstruction(h);
      }
      for (int i = 0; i < val2; i++) {
        addVariable(std::string("t") + std::to_string(i));
        auto rz = std::make_shared<Rz>(i, std::string("t") + std::to_string(i));
        addInstruction(rz);
      }

      return true;
    }
    const std::vector<std::string> requiredKeys() override {
      return {"key1", "key2"};
    }
  };

  auto sub = std::make_shared<sub_circuit>("sub");
  auto h = std::make_shared<Hadamard>(0);
  auto cx = std::make_shared<CNOT>(0, 1);

  auto circuit = std::make_shared<Circuit>("foo");
  EXPECT_EQ("foo", circuit->name());
  circuit->addInstruction(h);
  circuit->addInstruction(cx);
  circuit->addInstruction(sub);

  std::cout << "Circuit:\n" << circuit->toString() << "\n";

  xacc::HeterogeneousMap m(std::make_pair("key1", 3),
                           std::make_pair("key2", 4));
  std::cout << "HELLO\n";
  circuit->expand(m);
  std::cout << "Circuit2:\n" << circuit->toString() << "\n";

}


TEST(GateFunctionTester, checkPersistLoad) {

  auto f = std::make_shared<Circuit>("foo", std::vector<std::string>{"phi"});
  auto h = std::make_shared<Hadamard>(1);
  auto cn1 = std::make_shared<CNOT>(0, 1);
  auto rz = std::make_shared<Rz>(0, 3.1415);
  auto rz2 = std::make_shared<Rz>(std::vector<std::size_t>{1});
  xacc::InstructionParameter p("phi");
  rz2->setParameter(0, p);

  f->addInstruction(h);
  f->addInstruction(cn1);
  f->addInstruction(rz);
  f->addInstruction(rz2);

  std::stringstream ss;
  f->persist(ss);

  std::cout << ss.str() << "\n";

  std::istringstream iss(ss.str());

  auto newF = std::make_shared<Circuit>("new");
  newF->load(iss);

  std::cout << "HELLO: " << newF->toString() << "\n";
}

TEST(GateTester, checkGenerateGraph) {

  auto f = std::make_shared<Circuit>("foo");
  auto h = std::make_shared<Hadamard>(1);
  auto cn1 = std::make_shared<CNOT>(1, 2);
  auto cn2 = std::make_shared<CNOT>(0, 1);
  auto h2 = std::make_shared<Hadamard>(0);
  auto rz = std::make_shared<Rz>(2, 3.1415);
  f->addInstructions({h,cn1,cn2,h2,rz});

  auto s = f->persistGraph();

//   std::stringstream ss;
//   g->write(ss);

  std::string expected = R"expected(digraph G {
node [shape=box style=filled]
0 [label="bits=[0,1,2];id=0;name=InitialState"];
1 [label="bits=[1];id=1;layer=0;name=H"];
2 [label="bits=[1,2];id=2;layer=1;name=CNOT"];
3 [label="bits=[0,1];id=3;layer=2;name=CNOT"];
4 [label="bits=[0];id=4;layer=3;name=H"];
5 [label="bits=[2];id=5;layer=3;name=Rz"];
6 [label="bits=[0,1,2];id=6;name=FinalState"];
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
}
)expected";

  std::cout << s << "\nExpected:\n" << expected << "\n";
  EXPECT_TRUE(expected == s);
}

TEST(GateFunctionTester, checkDepth) {
  auto f = std::make_shared<Circuit>("foo");
  auto x = std::make_shared<X>(0);
  auto h = std::make_shared<Hadamard>(1);
  auto cn1 = std::make_shared<CNOT>(1, 2);
  auto rz = std::make_shared<Rz>(1, 3.1415);
  auto z = std::make_shared<Z>(2);

  f->addInstructions({x,h,cn1,rz,z});

  auto g = f->toGraph();

  EXPECT_EQ(3, g->depth());
}

TEST(GateFunctionTester, checkU3Eval) {
    xacc::qasm(R"(
.compiler xasm
.circuit qubit2_depth1
.parameters x0,x1
.qbit q
U(q[0], x0, -3.14159/2, 3.14159/2 );
U(q[0], 0, 0, x1);
)");
auto f = xacc::getCompiled("qubit2_depth1");
auto ff = f->operator()({1.0, 1.0});
std::cout << "F: " << ff->toString() << "\n";
}
int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
