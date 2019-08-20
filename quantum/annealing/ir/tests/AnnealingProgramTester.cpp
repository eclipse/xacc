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
#include <gtest/gtest.h>
#include "AnnealingProgram.hpp"

using namespace xacc::quantum;

TEST(DWFunctionTester, checkDWKernelConstruction) {

  auto qmi = std::make_shared<DWQMI>(0, 1, 2.2);
  auto qmi2 = std::make_shared<DWQMI>(0);
  auto qmi3 = std::make_shared<DWQMI>(22, 3.3);

  AnnealingProgram kernel("foo");
  kernel.addInstruction(qmi);
  kernel.addInstruction(qmi2);
  kernel.addInstruction(qmi3);

  EXPECT_TRUE(kernel.nInstructions() == 3);
  EXPECT_TRUE(kernel.name() == "foo");
  EXPECT_TRUE(kernel.getInstruction(0) == qmi);
  EXPECT_TRUE(kernel.getInstruction(1) == qmi2);
  EXPECT_TRUE(kernel.getInstruction(2) == qmi3);

  const std::string expected = "0 1 2.2;\n0 0 0;\n22 22 3.3;\n";

  EXPECT_TRUE(kernel.toString() == expected);
}

TEST(DWFunctionTester, checkGraph) {

  auto qmi = std::make_shared<DWQMI>(0, 1, 2.2);
  auto qmi2 = std::make_shared<DWQMI>(0, 1.2);
  auto qmi3 = std::make_shared<DWQMI>(1, 3.3);

  AnnealingProgram kernel("foo");
  kernel.addInstruction(qmi);
  kernel.addInstruction(qmi2);
  kernel.addInstruction(qmi3);

  auto graph = kernel.toGraph();

  graph->write(std::cout);
}

TEST(DWFunctionTester, checkVariableParameterEval) {
  auto param1 = xacc::InstructionParameter("param1");
  auto param2 = xacc::InstructionParameter("param1");
  auto param3 = xacc::InstructionParameter("param2");
  auto qmi = std::make_shared<DWQMI>(0, 1, param1);
  auto qmi2 = std::make_shared<DWQMI>(2, 3, param1);
  auto qmi3 = std::make_shared<DWQMI>(3,4 , param3);


  AnnealingProgram kernel("foo", {"param1","param2"});
  kernel.addInstruction(qmi);
  kernel.addInstruction(qmi2);
  kernel.addInstruction(qmi3);

  std::cout << kernel.nVariables() << std::endl;
  for (auto param : kernel.getVariables()) {
      std::cout <<param << std::endl;
  }
  EXPECT_TRUE(kernel.nVariables() == 2);
  EXPECT_TRUE(kernel.nInstructions() == 3);
  EXPECT_TRUE(kernel.name() == "foo");
  EXPECT_TRUE(kernel.getInstruction(0) == qmi);
  EXPECT_TRUE(kernel.getInstruction(1) == qmi2);
  EXPECT_TRUE(kernel.getInstruction(2) == qmi3);

  const std::string expected = "0 1 param1;\n2 3 param1;\n3 4 param2;\n";
  EXPECT_TRUE(kernel.toString() == expected);
  std::vector<double> pars {3.4, 4.5};

  auto evaled = kernel(pars);

  std::cout << evaled->toString() << std::endl;
}
int main(int argc, char **argv) {
    xacc::Initialize(argc,argv);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
