/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#include "ReadoutErrorAcceleratorBufferPostprocessor.hpp"
#include "GateIR.hpp"
#include <boost/math/constants/constants.hpp>
#include "XACC.hpp"
#include "GateFunction.hpp"
#include "GateInstruction.hpp"
#include "GateIRProvider.hpp"

using namespace xacc;

using namespace xacc::quantum;


TEST(ROErrorDecoratorTester, checkSimple) {

  xacc::Initialize();
  xacc::Finalize();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
