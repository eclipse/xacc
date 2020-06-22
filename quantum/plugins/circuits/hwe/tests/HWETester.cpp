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
#include "xacc.hpp"
#include <gtest/gtest.h>
#include "Circuit.hpp"
#include "xacc_service.hpp"

using namespace xacc;

TEST(HWETester, checkSimple) {

  // NOW Test it somehow...
 auto hwe = std::dynamic_pointer_cast<quantum::Circuit>(
      xacc::getService<Instruction>("hwe"));
  EXPECT_TRUE(hwe->expand({
          std::make_pair("nq", 2),
          std::make_pair("layers", 1),
          std::make_pair("coupling", std::vector<std::pair<int,int>>{{0, 1}})
          }));

  std::cout << hwe->toString() << "\n";
  EXPECT_EQ(11, hwe->nInstructions());
  EXPECT_EQ(10, hwe->nVariables());

auto hwe2 = std::dynamic_pointer_cast<quantum::Circuit>(
      xacc::getService<Instruction>("hwe"));
  HeterogeneousMap m{
          std::make_pair("nq", 4),
          std::make_pair("layers", 1),
          std::make_pair("coupling", std::vector<std::pair<int,int>>{{0, 1},{1,2},{2,3}})
          };
  EXPECT_TRUE(hwe2->expand(m));
    // std::cout << f->toString() << "\n";
  EXPECT_EQ(23, hwe2->nInstructions());
  EXPECT_EQ(20, hwe2->nVariables());
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
//   xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
