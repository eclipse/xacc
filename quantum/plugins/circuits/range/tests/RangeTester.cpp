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
#include "xacc_service.hpp"
#include "Circuit.hpp"

using namespace xacc;

TEST(RangeTester, checkSimple) {

  // NOW Test it somehow...
  auto r = std::dynamic_pointer_cast<quantum::Circuit>(
      xacc::getService<Instruction>("range"));
  EXPECT_TRUE(r->expand(
      {std::make_pair("gate", std::string("H")), std::make_pair("nq", 10)}));

  std::cout << "F:\n" << r->toString() << "\n";
  EXPECT_EQ(10, r->nInstructions());

  auto r2 = std::dynamic_pointer_cast<quantum::Circuit>(
      xacc::getService<Instruction>("range"));
  EXPECT_TRUE(r2->expand({std::make_pair("gate", std::string("H")),
              std::make_pair("start", 3), std::make_pair("end", 10)}));
  std::cout << "G:\n" << r2->toString() << "\n";
  EXPECT_EQ(7, r2->nInstructions());
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
