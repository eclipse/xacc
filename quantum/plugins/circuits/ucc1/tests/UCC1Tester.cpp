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

TEST(UCC3Tester, checkSimple) {

  // NOW Test it somehow...
 auto ucc1 = std::dynamic_pointer_cast<quantum::Circuit>(
      xacc::getService<Instruction>("ucc1"));
  ucc1->addVariable("x");
  EXPECT_TRUE(ucc1->expand({
          }));

  std::cout << ucc1->toString() << "\n";
  EXPECT_EQ(15, ucc1->nInstructions());
  EXPECT_EQ(1, ucc1->nVariables());

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
