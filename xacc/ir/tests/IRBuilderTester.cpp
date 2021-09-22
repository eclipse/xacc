/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>
#include "xacc.hpp"

TEST(IRBuilderTester, checkSimple) {

  {
    xacc::IRBuilder builder;
    builder.h(0);
    builder.h(1);
    builder.cnot(0, 1);
    builder.rz(1, 2.2);
    builder.cphase(0, 1, 3.1415);
    builder.rx(0, "theta");
    builder.u(2, 1.1, 2.2, 3.3);
    builder.mz(0);
    builder.mz(1);
    auto composite = builder.to_ir();
    std::cout << composite->toString() << "\n";
    EXPECT_EQ(9, composite->nInstructions());
  }

  {
    xacc::IRBuilder builder("test");
    builder.h(std::make_pair("qregQQ", 0));
    builder.cnot(std::make_pair("qregQQ", 0), std::make_pair("qregQQ", 1));
    auto composite = builder.to_ir();
    std::cout << composite->toString() << "\n";
    EXPECT_EQ(2, composite->nInstructions());
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
