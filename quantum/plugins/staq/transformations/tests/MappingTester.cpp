/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
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
using namespace xacc;

class AcceleratorWithConnectivity : public xacc::Accelerator {
protected:
  std::vector<std::pair<int, int>> edges;

public:
  AcceleratorWithConnectivity(std::vector<std::pair<int, int>> &&connections)
      : edges(connections) {}
  const std::string name() const override { return "acc_with_connectivity"; }
  const std::string description() const override { return ""; }
  void initialize(const HeterogeneousMap &params = {}) override { return; }
  void execute(std::shared_ptr<xacc::AcceleratorBuffer> buf,
               std::shared_ptr<xacc::CompositeInstruction> f) override {}
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   functions) override {}
  void updateConfiguration(const HeterogeneousMap &config) override {}
  const std::vector<std::string> configurationKeys() override { return {}; }

  std::vector<std::pair<int, int>> getConnectivity() override { return edges; }
};

TEST(Staq_MappingTester, checkSwapShort) {

  auto qpu = std::make_shared<AcceleratorWithConnectivity>(
      std::vector<std::pair<int, int>>{{0, 1},
                                       {0, 5},
                                       {1, 2},
                                       {1, 4},
                                       {2, 3},
                                       {3, 4},
                                       {3, 8},
                                       {4, 5},
                                       {4, 7},
                                       {5, 6},
                                       {6, 7},
                                       {7, 8}});

  auto x = xacc::qalloc(9);
  x->setName("x");
  xacc::storeBuffer(x);

  auto irt = xacc::getIRTransformation("swap-shortest-path");
  auto compiler = xacc::getCompiler("xasm");
  auto program = compiler->compile(R"(__qpu__ void test_simple(qreg x) {
      CX(x[0], x[2]);
      Measure(x[0]);
  })")->getComposite("test_simple");

  EXPECT_EQ(2, program->nInstructions());
  EXPECT_EQ("CNOT", program->getInstruction(0)->name());

  irt->apply(program, nullptr);
  EXPECT_EQ(2, program->nInstructions());

  irt->apply(program, qpu);

  EXPECT_EQ(5, program->nInstructions());
  std::cout << program->toString() << "\n";



auto circuit = compiler->compile( R"(__qpu__ void bell_x_2(qreg q, qreg r) {
  // First bell state on qbits 0,1
  H(q[0]);
  CX(q[0],q[2]);

  // Second bell state on qbis 2,3
  H(r[0]);
  CX(r[0],r[1]);

  // Measure them all
  Measure(q[0]);
  Measure(q[1]);

  Measure(r[0]);
  Measure(r[1]);
  // should see 0000, 0011, 1100, 1111
})")->getComposite("bell_x_2");

irt->apply(circuit, qpu);

}

TEST(Staq_MappingTester, checkSwapShortSwap) {
  auto qpu = std::make_shared<AcceleratorWithConnectivity>(
      std::vector<std::pair<int, int>>{{0, 1},
                                       {1, 2}});

  auto x = xacc::qalloc(3);
  x->setName("x");
  xacc::storeBuffer(x);

  auto irt = xacc::getIRTransformation("swap-shortest-path");
  auto compiler = xacc::getCompiler("xasm");
  auto program = compiler->compile(R"(__qpu__ void test_simple_swap(qreg x) {
      X(x[0]);
      Swap(x[0], x[2]);
      Measure(x[0]);
      Measure(x[2]);
  })")->getComposite("test_simple_swap");

  EXPECT_EQ(4, program->nInstructions());
  EXPECT_EQ("Swap", program->getInstruction(1)->name());

  irt->apply(program, nullptr);
  EXPECT_EQ(4, program->nInstructions());

  irt->apply(program, qpu);

  // Staq will expand the CNOT
  EXPECT_EQ(9, program->nInstructions());
  std::cout << program->toString() << "\n";
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  xacc::set_verbose(true);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
