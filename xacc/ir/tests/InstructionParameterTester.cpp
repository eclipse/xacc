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

#include "InstructionParameter.hpp"

using namespace xacc;

TEST(InstructionParameterTester, checkSimple) {

 InstructionParameter p("theta");
 InstructionParameter q(
      std::vector<std::pair<int, int>>{{0, 1}, {2, 3}});

  EXPECT_EQ(p.toString(), "theta");
  EXPECT_EQ(q.toString(), "[[0,1], [2,3]]");

  EXPECT_TRUE(p.isVariable());
  EXPECT_FALSE(q.isVariable());

  EXPECT_TRUE(InstructionParameter(1).isNumeric());
  EXPECT_TRUE(InstructionParameter(1.12).isNumeric());
  
  auto str = p.as<std::string>();
  EXPECT_EQ("theta",str);

//   EXPECT_EQ(1, InstructionParameter(1.0).as<int>());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
