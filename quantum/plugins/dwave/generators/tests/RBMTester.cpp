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
#include "AnnealingProgram.hpp"

using namespace xacc;

TEST(RBMTester, checkSimple) {
 auto rbm =std::dynamic_pointer_cast<quantum::AnnealingProgram>(
      xacc::getService<Instruction>("rbm-ap"));

  EXPECT_TRUE(rbm->expand(
      {std::make_pair("nv", 4), std::make_pair("nh", 4)}));

  std::cout << "F:\n" << rbm->toString() << "\n";
  EXPECT_EQ(4+4+16, rbm->nInstructions());

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
