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
#include "Circuit.hpp"
#include "Optimizer.hpp"
#include "xacc_observable.hpp"
#include "xacc_service.hpp"
#include <Eigen/Dense>
using namespace xacc;

TEST(QFastTester, checkSimple) 
{
  auto tmp = xacc::getService<Instruction>("QFAST");
  auto qfast = std::dynamic_pointer_cast<quantum::Circuit>(tmp);
  Eigen::MatrixXcd ccnotMat = Eigen::MatrixXcd::Identity(8, 8);
  ccnotMat(6, 6) = 0.0;
  ccnotMat(7, 7) = 0.0;
  ccnotMat(6, 7) = 1.0;
  ccnotMat(7, 6) = 1.0;
  
  const bool expandOk = qfast->expand({ 
    std::make_pair("unitary", ccnotMat)
  });
  EXPECT_TRUE(expandOk);
  
  auto acc = xacc::getAccelerator("qpp", { std::make_pair("shots", 1024)});
  auto gateRegistry = xacc::getService<IRProvider>("quantum");
  auto xGate0 = gateRegistry->createInstruction("X", { 0 });
  auto xGate1 = gateRegistry->createInstruction("X", { 1 });
  auto xGate2 = gateRegistry->createInstruction("X", { 2 });
  auto measureGate0 = gateRegistry->createInstruction("Measure", { 0 });
  auto measureGate1 = gateRegistry->createInstruction("Measure", { 1 });
  auto measureGate2 = gateRegistry->createInstruction("Measure", { 2 });

  const auto runTestCase = [&](bool in_bit0, bool in_bit1, bool in_bit2) {
    static int counter = 0;
    auto composite = gateRegistry->createComposite("__TEMP_COMPOSITE__" + std::to_string(counter));
    counter++;
    // State prep:
    if (in_bit0)
    {
      composite->addInstruction(xGate0);
    }
    if (in_bit1)
    {
      composite->addInstruction(xGate1);
    }
    if (in_bit2)
    {
      composite->addInstruction(xGate2);
    }

    composite->addInstructions(qfast->getInstructions());
    // Mesurement:
    composite->addInstructions({ measureGate0, measureGate1, measureGate2 });

    auto buffer = xacc::qalloc(3);
    acc->execute(buffer, composite);
    buffer->print();
  };

  // 3 bits
  for (int i = 0; i < (1 << 3); ++i)
  {
    runTestCase(i & 0x001, i & 0x002, i & 0x004);
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  xacc::set_verbose(true);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}