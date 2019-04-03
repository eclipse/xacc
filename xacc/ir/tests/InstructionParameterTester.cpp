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
  InstructionParameter c(std::complex<double>(1,0));

  EXPECT_EQ(p.toString(), "theta");
  EXPECT_EQ(q.toString(), "[[0,1],[2,3]]");

  EXPECT_TRUE(p.isVariable());
  EXPECT_FALSE(q.isVariable());

  EXPECT_TRUE(InstructionParameter(1).isNumeric());
  EXPECT_TRUE(InstructionParameter(1.12).isNumeric());

  auto str = p.as<std::string>();
  EXPECT_EQ("theta",str);

  std::cout << "WHICH: " << p.which() << "\n";
  EXPECT_EQ(2, p.which());
  EXPECT_EQ(4, q.which());
  EXPECT_EQ(3,c.which());

  EXPECT_EQ(std::complex<double>(1,0), c.as<std::complex<double>>());
}

TEST(InstructionParameterTester, checkMap) {

    std::map<std::string, InstructionParameter> options;

    options.insert({"pulse_id", InstructionParameter("null")});

    EXPECT_EQ("null", options["pulse_id"].toString());

    std::map<std::string, InstructionParameter> options2 {{"hello",1}, {"key2","value"}};

    EXPECT_EQ(1, options2["hello"].as<int>());
    EXPECT_EQ("value", options2["key2"].as<std::string>());

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
