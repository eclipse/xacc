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
#include "DigitalGates.hpp"

using namespace xacc::quantum;

TEST(CNOTTester, checkCreation) {

  CNOT cnot(0, 1);
  EXPECT_TRUE(cnot.toString("qreg") == "CNOT qreg0,qreg1");
  EXPECT_TRUE(cnot.bits().size() == 2);
  EXPECT_TRUE(cnot.bits()[0] == 0);
  EXPECT_TRUE(cnot.bits()[1] == 1);
  EXPECT_TRUE(cnot.name() == "CNOT");

  CNOT cnot2(44, 46);
  EXPECT_TRUE(cnot2.toString("qreg") == "CNOT qreg44,qreg46");
  EXPECT_TRUE(cnot2.bits().size() == 2);
  EXPECT_TRUE(cnot2.bits()[0] == 44);
  EXPECT_TRUE(cnot2.bits()[1] == 46);
  EXPECT_TRUE(cnot2.name() == "CNOT");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
