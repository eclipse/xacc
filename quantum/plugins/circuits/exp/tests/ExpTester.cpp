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

TEST(ExpTester, checkSimple) {

  // NOW Test it somehow...
  auto exp = std::dynamic_pointer_cast<quantum::Circuit>(
      xacc::getService<Instruction>("exp_i_theta"));
  EXPECT_TRUE(exp->expand({std::make_pair("pauli", "Y0 X1 X2")}));

  std::cout << "F:\n" << exp->toString() << "\n";
  auto exp2 = std::dynamic_pointer_cast<quantum::Circuit>(
      xacc::getService<Instruction>("exp_i_theta"));

  EXPECT_TRUE(exp2->expand({std::make_pair("pauli", "X0 Y1 - X1 Y0")}));

  std::cout << "F2:\n" << exp2->toString() << "\n";
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  //   xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
