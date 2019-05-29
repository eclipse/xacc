/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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

TEST(UTester, checkCreation) {

//   U1 u1(0, 3.14);
//   EXPECT_TRUE(mpark::get<double>(u1.getParameter(0)) == 3.14);
//   EXPECT_TRUE(u1.toString("qreg") == "U1(3.14) qreg0");
//   EXPECT_TRUE(u1.bits().size() == 1);
//   EXPECT_TRUE(u1.bits()[0] == 0);
//   EXPECT_TRUE(u1.name() == "U1");

//   U1 u12(44, 1.71234);

//   EXPECT_TRUE(mpark::get<double>(u12.getParameter(0)) == 1.71234);
//   EXPECT_TRUE(u12.toString("qreg") == "U1(1.71234) qreg44");
//   EXPECT_TRUE(u12.bits().size() == 1);
//   EXPECT_TRUE(u12.bits()[0] == 44);
//   EXPECT_TRUE(u12.name() == "U1");

//   U2 u2(0, 3.14, 1.57);
//   EXPECT_TRUE(mpark::get<double>(u2.getParameter(0)) == 3.14);
//   EXPECT_TRUE(mpark::get<double>(u2.getParameter(1)) == 1.57);
//   EXPECT_TRUE(u2.toString("qreg") == "U2(3.14,1.57) qreg0");
//   EXPECT_TRUE(u2.bits().size() == 1);
//   EXPECT_TRUE(u2.bits()[0] == 0);
//   EXPECT_TRUE(u2.name() == "U2");

//   U2 u22(44, 1.71234, .22);

//   EXPECT_TRUE(mpark::get<double>(u22.getParameter(0)) == 1.71234);
//   EXPECT_TRUE(mpark::get<double>(u22.getParameter(1)) == .22);
//   EXPECT_TRUE(u22.toString("qreg") == "U2(1.71234,0.22) qreg44");
//   EXPECT_TRUE(u22.bits().size() == 1);
//   EXPECT_TRUE(u22.bits()[0] == 44);
//   EXPECT_TRUE(u22.name() == "U2");

  U u(0, 3.14, 1.57, .33);
  EXPECT_TRUE(mpark::get<double>(u.getParameter(0)) == 3.14);
  EXPECT_TRUE(mpark::get<double>(u.getParameter(1)) == 1.57);
  EXPECT_TRUE(mpark::get<double>(u.getParameter(2)) == .33);
  EXPECT_TRUE(u.toString("qreg") == "U(3.14,1.57,0.33) qreg0");
  EXPECT_TRUE(u.bits().size() == 1);
  EXPECT_TRUE(u.bits()[0] == 0);
  EXPECT_TRUE(u.name() == "U");

  U u222(44, 1.71234, .22, .44);

  EXPECT_TRUE(mpark::get<double>(u222.getParameter(0)) == 1.71234);
  EXPECT_TRUE(mpark::get<double>(u222.getParameter(1)) == .22);
  EXPECT_TRUE(mpark::get<double>(u222.getParameter(2)) == .44);
  EXPECT_TRUE(u222.toString("qreg") == "U(1.71234,0.22,0.44) qreg44");
  EXPECT_TRUE(u222.bits().size() == 1);
  EXPECT_TRUE(u222.bits()[0] == 44);
  EXPECT_TRUE(u222.name() == "U");

}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
