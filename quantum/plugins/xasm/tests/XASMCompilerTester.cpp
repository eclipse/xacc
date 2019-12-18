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
#include "gtest/gtest.h"

#include "xacc.hpp"
#include "xacc_service.hpp"

#include "Circuit.hpp"
TEST(XASMCompilerTester, checkTranslate) {
     auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler -> compile(R"(__qpu__ void bell_test(qbit q, double t0) {
  H(q[0]);
  CX(q[0], q[1]);
  Ry(q[0], t0);
  Measure(q[0]);
  Measure(q[1]);
})");

auto evaled = (*IR->getComposite("bell_test"))({1.1});
auto translated = compiler->translate(evaled);
std::cout << "HELLO:\n" << translated << "\n";
}
TEST(XASMCompilerTester, checkSimple) {

  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler -> compile(R"(__qpu__ void bell(qbit q, double t0) {
  H(q[0]);
  CX(q[0], q[1]);
  Ry(q[0], t0);
  Measure(q[0]);
  Measure(q[1]);
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";


  IR = compiler -> compile(R"([&](qbit q, double t0) {
  H(q[0]);
  CX(q[0], q[1]);
  Ry(q[0], t0);
  bell(q);
  Measure(q[0]);
  Measure(q[1]);
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";

}

TEST(XASMCompilerTester, checkVectorArg) {

  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler -> compile(R"(__qpu__ void bell22(qbit q, std::vector<double> x) {
  H(q[0]);
  CX(q[0], q[1]);
  Ry(q[0], x[0]);
  Measure(q[0]);
  Measure(q[1]);
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
  std::cout << "KERNEL\n" << IR->getComposites()[0]->operator()({2.})->toString() << "\n";
}

TEST(XASMCompilerTester, checkSimpleFor) {

  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler -> compile(R"(__qpu__ void testFor(qbit q, std::vector<double> x) {
  for (int i = 0; i < 5; i++) {
     H(q[i]);
     Rx(q[i], x[i]);
     CX(q[0], q[i]);
  }

  for (int i = 0; i < 3; i++) {
      CX(q[i], q[i+1]);
  }
  Rz(q[3], 0.22);

  for (int i = 3; i > 0; i--) {
      CX(q[i-1],q[i]);
  }
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
  for (auto ii : IR->getComposites()[0]->getVariables()) std::cout << ii << "\n";
  EXPECT_EQ(22, IR->getComposites()[0]->nInstructions());
}
TEST(XASMCompilerTester, checkHWEFor) {

  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler -> compile(R"([&](qbit q, std::vector<double> x) {
    for (auto i = 0; i < 2; i++) {
        Rx(q[i],x[i]);
        Rz(q[i],x[2+i]);
    }
    CX(q[1],q[0]);
    for (auto i = 0; i < 2; i++) {
        Rx(q[i], x[i+4]);
        Rz(q[i], x[i+4+2]);
        Rx(q[i], x[i+4+4]);
    }
  })");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
  for (auto ii : IR->getComposites()[0]->getVariables()) std::cout << ii << "\n";
  EXPECT_EQ(11, IR->getComposites()[0]->nInstructions());
}

TEST(XASMCompilerTester, checkApplyAll) {
class custom_range : public xacc::quantum::Circuit {
  public:
    custom_range() : Circuit("range") {}
    bool expand(const xacc::HeterogeneousMap &runtimeOptions) override {
      if (!runtimeOptions.keyExists<int>("nq") &&
          !runtimeOptions.keyExists<std::string>("gate")) {
        return false;
      }

      auto val1 = runtimeOptions.get<int>("nq");
      auto val2 = runtimeOptions.get<std::string>("gate");

      auto provider = xacc::getIRProvider("quantum");
      auto f = provider->createComposite("range" + val2);
      for (std::size_t i = 0; i < val1; i++) {
         auto g = provider->createInstruction(val2, std::vector<std::size_t>{i});
         addInstruction(g);
      }
      return true;
    }
    const std::vector<std::string> requiredKeys() override {
      return {"nq", "gate"};
    }
  };
  std::shared_ptr<xacc::Instruction> service = std::make_shared<custom_range>();
  xacc::contributeService("custom_range", service);

  auto compiler = xacc::getCompiler("xasm");
  auto IR = compiler -> compile(R"([&](qbit q) {
  custom_range(q, {{"gate","H"},{"nq",4}});
})");
  EXPECT_EQ(1, IR->getComposites().size());
  std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";
}

// TEST(XASMCompilerTester, checkUnknownParameter) {
// auto compiler = xacc::getCompiler("xasm");
//   auto IR = compiler -> compile(R"([&](qbit q) {
//   uccsd(q, {{"nqubits",4},{"nelectrons",2}});
// })");

//  EXPECT_EQ(1, IR->getComposites().size());
//   std::cout << "KERNEL\n" << IR->getComposites()[0]->toString() << "\n";

//   IR = compiler -> compile(R"([&](qbit q) {
//   uccsd(q, {{"nqubits",4},{"nelectrons",ne}});
// })");

//  EXPECT_EQ(1, IR->getComposites().size());
//   auto f = IR->getComposites()[0];
//   std::cout << "KERNEL2\n" << IR->getComposites()[0]->toString() << "\n";

// //   f->expandIRGenerators({{"nqubits", 4}, {"nelectrons",2}});
// //     std::cout << "KERNEL3\n" << f->toString() << "\n";

// }

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
