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
#include "X.hpp"

using namespace xacc::quantum;

TEST(XTester, checkCreation) {

  X x(0);
  EXPECT_TRUE(x.toString("qreg") == "X qreg0");
  EXPECT_TRUE(x.bits().size() == 1);
  EXPECT_TRUE(x.bits()[0] == 0);
  EXPECT_TRUE(x.name() == "X");

  X x2(44);
  EXPECT_TRUE(x2.toString("qreg") == "X qreg44");
  EXPECT_TRUE(x2.bits().size() == 1);
  EXPECT_TRUE(x2.bits()[0] == 44);
  EXPECT_TRUE(x2.name() == "X");
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
