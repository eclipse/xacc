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
#include "DWAccelerator.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace xacc::quantum;

TEST(DWAcceleratorTester, checkKernelExecution) {

  // DWAccelerator acc;
  // acc.initialize();
  // auto buffer = acc.createBuffer("qubits");
  // auto f = std::make_shared<DWKernel>("simple");
  // f->addInstruction(std::make_shared<DWQMI>(0, 0, 20));
  // f->addInstruction(std::make_shared<DWQMI>(1, 1, 50));
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
