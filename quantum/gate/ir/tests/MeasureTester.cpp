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

TEST(MeasureTester, checkCreation) {

  Measure meas(0, 1);
  EXPECT_TRUE(mpark::get<int>(meas.getParameter(0)) == 1);
  EXPECT_TRUE(meas.toString("qreg") == "Measure qreg0");
  EXPECT_TRUE(meas.bits().size() == 1);
  EXPECT_TRUE(meas.bits()[0] == 0);
  EXPECT_TRUE(meas.name() == "Measure");
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
