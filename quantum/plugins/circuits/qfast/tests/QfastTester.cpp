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
  srand(time(0));
  // Pick 0 or 1
  const int randPick = rand() % 2;
  const std::string optimizerName = (randPick == 0) ? "nlopt" : "mlpack";
  
  // Randomly pick an optimizer:
  // To save test time, we don't want to run many long tests.
  auto optimizer = xacc::getOptimizer(optimizerName);
  std::cout << "Use '" << optimizer->name() << "' optimizer\n";

  auto acc = xacc::getAccelerator("qpp", { std::make_pair("shots", 8192)});
  auto gateRegistry = xacc::getService<IRProvider>("quantum");
  auto xGate0 = gateRegistry->createInstruction("X", { 0 });
  auto xGate1 = gateRegistry->createInstruction("X", { 1 });
  auto xGate2 = gateRegistry->createInstruction("X", { 2 });
  auto measureGate0 = gateRegistry->createInstruction("Measure", { 0 });
  auto measureGate1 = gateRegistry->createInstruction("Measure", { 1 });
  auto measureGate2 = gateRegistry->createInstruction("Measure", { 2 });

  const auto runTestCase = [&](bool in_bit0, bool in_bit1, bool in_bit2) {
    // We repeatedly expand this using QFAST to validate
    // the caching mechanism.
    // i.e. only the first run needs decomposition,
    // later runs will be a direct cache-hit.
    auto tmp = xacc::getService<Instruction>("QFAST");
    auto qfast = std::dynamic_pointer_cast<quantum::Circuit>(tmp);
    Eigen::MatrixXcd ccnotMat = Eigen::MatrixXcd::Identity(8, 8);
    ccnotMat(6, 6) = 0.0;
    ccnotMat(7, 7) = 0.0;
    ccnotMat(6, 7) = 1.0;
    ccnotMat(7, 6) = 1.0;
  
    const bool expandOk = qfast->expand({ 
      std::make_pair("unitary", ccnotMat),
      std::make_pair("optimizer", optimizer)
    });
    EXPECT_TRUE(expandOk);
    
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

    std::string inputBitString;
    inputBitString.append(in_bit0 ? "1" : "0");
    inputBitString.append(in_bit1 ? "1" : "0");
    inputBitString.append(in_bit2 ? "1" : "0");

    composite->addInstructions(qfast->getInstructions());
    // Mesurement:
    composite->addInstructions({ measureGate0, measureGate1, measureGate2 });

    auto buffer = xacc::qalloc(3);
    acc->execute(buffer, composite);
    std::cout << "Input bitstring: " << inputBitString << "\n";
    buffer->print();
    // CCNOT gate:
    const auto expectedBitString = [&inputBitString]() -> std::string {
      // If both control bits are 1's
      if (inputBitString == "110")
      {
        return "111";
      }
      if (inputBitString == "111")
      {
        return "110";
      }
      // Otherwise, no changes
      return inputBitString;
    }();

    // Check bit string
    EXPECT_NEAR(buffer->computeMeasurementProbability(expectedBitString),  1.0,  0.1);
  };  

  // 3 bits: run all test cases (8)
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