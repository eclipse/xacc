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

TEST(MappingTester, checkSwapShort) {

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
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  xacc::set_verbose(true);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
