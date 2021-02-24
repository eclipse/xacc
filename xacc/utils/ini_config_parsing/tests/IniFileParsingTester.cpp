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
#include "config_file_parser.hpp"

using namespace xacc;

TEST(IniFileParsingTester, checkSimple) {
  const std::string testFile = std::string(INI_SOURCE_DIR) + "/test1.ini";
  auto parser = xacc::getService<ConfigFileParsingUtil>("ini");
  auto config = parser->parse(testFile);
  EXPECT_EQ(config.get<int>("one"), 1);
  EXPECT_NEAR(config.get<double>("pi"), M_PI, 0.01);
  EXPECT_EQ(config.getString("name"), "XACC");
  EXPECT_TRUE(config.get<bool>("true_val"));
  EXPECT_FALSE(config.get<bool>("false_val"));
}

TEST(IniFileParsingTester, checkArrays) {
  const std::string testFile = std::string(INI_SOURCE_DIR) + "/test1.ini";
  auto parser = xacc::getService<ConfigFileParsingUtil>("ini");
  auto config = parser->parse(testFile);
  const auto intArray = config.get<std::vector<int>>("array");
  EXPECT_EQ(intArray.size(), 3);
  for (int i = 0; i < intArray.size(); ++i) {
    EXPECT_EQ(intArray[i], i + 1);
  }

  const auto doubleArray = config.get<std::vector<double>>("array_double");
  EXPECT_EQ(doubleArray.size(), 3);
  for (int i = 0; i < doubleArray.size(); ++i) {
    EXPECT_NEAR(doubleArray[i], 1.0 * (i + 1), 1e-12);
  }

  const auto connectivity =
      config.get<std::vector<std::pair<int, int>>>("connectivity");
  EXPECT_EQ(connectivity.size(), 3);
  for (int i = 0; i < connectivity.size(); ++i) {
    std::cout << connectivity[i].first << "-->" << connectivity[i].second
              << "\n";
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
