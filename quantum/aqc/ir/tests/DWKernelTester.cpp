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
#include "DWKernel.hpp"

using namespace xacc::quantum;

TEST(DWKernelTester, checkDWKernelConstruction) {

  auto qmi = std::make_shared<DWQMI>(0, 1, 2.2);
  auto qmi2 = std::make_shared<DWQMI>(0);
  auto qmi3 = std::make_shared<DWQMI>(22, 3.3);

  DWKernel kernel("foo");
  kernel.addInstruction(qmi);
  kernel.addInstruction(qmi2);
  kernel.addInstruction(qmi3);

  EXPECT_TRUE(kernel.nInstructions() == 3);
  EXPECT_TRUE(kernel.name() == "foo");
  EXPECT_TRUE(kernel.getInstruction(0) == qmi);
  EXPECT_TRUE(kernel.getInstruction(1) == qmi2);
  EXPECT_TRUE(kernel.getInstruction(2) == qmi3);

  const std::string expected = "0 1 2.2\n0 0 0\n22 22 3.3\n";

  EXPECT_TRUE(kernel.toString("") == expected);
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
