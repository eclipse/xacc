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

TEST(UCC1Tester, checkSimple) {

  // NOW Test it somehow...
 auto ucc3 = std::dynamic_pointer_cast<quantum::Circuit>(
      xacc::getService<Instruction>("ucc3"));
  ucc3->addVariable("x");
  ucc3->addVariable("y");
  ucc3->addVariable("z");

  EXPECT_TRUE(ucc3->expand({
          }));

  std::cout << ucc3->toString() << "\n";
  EXPECT_EQ(21, ucc3->nInstructions());
  EXPECT_EQ(3, ucc3->nVariables());

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
