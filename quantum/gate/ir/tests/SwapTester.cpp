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
#include "Swap.hpp"

using namespace xacc::quantum;

TEST(SwapTester, checkCreation) {

  Swap sw(0, 1);
  EXPECT_TRUE(sw.toString("qreg") == "Swap qreg0,qreg1");
  EXPECT_TRUE(sw.bits().size() == 2);
  EXPECT_TRUE(sw.bits()[0] == 0);
  EXPECT_TRUE(sw.bits()[1] == 1);
  EXPECT_TRUE(sw.name() == "Swap");

  Swap sw2(44, 45);

  EXPECT_TRUE(sw2.toString("qreg") == "Swap qreg44,qreg45");
  EXPECT_TRUE(sw2.bits().size() == 2);
  EXPECT_TRUE(sw2.bits()[0] == 44);
  EXPECT_TRUE(sw2.bits()[1] == 45);
  EXPECT_TRUE(sw2.name() == "Swap");
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
