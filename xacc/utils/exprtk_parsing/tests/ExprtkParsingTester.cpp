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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "expression_parsing_util.hpp"

using namespace xacc;

TEST(ExprtkParsingTester, checkSimple) {

  auto parsingUtil = xacc::getService<ExpressionParsingUtil>("exprtk");

  double value;
  EXPECT_TRUE(parsingUtil->isConstant("-pi + 33*pi - 2 *pi/2", value));
  EXPECT_NEAR(97.3893722613, value, 1e-4);
  EXPECT_FALSE(parsingUtil->isConstant("-pi + 33*pi - 2 *pi/2 + 10*t0", value));

  EXPECT_FALSE(parsingUtil->validExpression(
      "x0 + x1 + x2 + 33*x3", std::vector<std::string>{"x0", "x1"}));
  EXPECT_TRUE(parsingUtil->validExpression(
      "x0 + x1 + x2 + 33*x3",
      std::vector<std::string>{"x0", "x1", "x2", "x3"}));

  EXPECT_TRUE(parsingUtil->evaluate(
      "x0 + x1 + x2 + 33*x3", std::vector<std::string>{"x0", "x1", "x2", "x3"},
      {1., 2., 3., 4.}, value));
  EXPECT_NEAR(138.0, value, 1e-4);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
