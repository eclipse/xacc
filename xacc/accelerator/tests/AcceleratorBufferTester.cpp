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

#include "AcceleratorBuffer.hpp"

using namespace xacc;

TEST(AcceleratorBufferTester, checkGetExpectationValueZ) {

  AcceleratorBuffer buffer("qreg", 5);

  boost::dynamic_bitset<> measurement1(std::string("00000"));
  boost::dynamic_bitset<> measurement2(std::string("00001"));

  buffer.appendMeasurement(measurement1, 500);
  buffer.appendMeasurement(measurement2, 504);

  EXPECT_TRUE(std::fabs(buffer.getExpectationValueZ() + 0.00398406) < 1e-6);

  //	Kernel 38 Z2, 2 mapped to qubit 6
  //	[2017-11-29 15:01:42.752] [xacc-console] [info] Measured Qubits: 6,
  //	[2017-11-29 15:01:42.752] [xacc-console] [info] IBM Results:
  // 0000100000000000:1 	[2017-11-29 15:01:42.752] [xacc-console] [info]
  // Our Results: 000000000000:1 	[2017-11-29 15:01:42.752] [xacc-console]
  // [info] IBM Results: 0000110000000000:7827 	[2017-11-29 15:01:42.752]
  // [xacc-console] [info] Our Results: 000000000000:7827 	[2017-11-29
  //15:01:42.761] [xacc-console] [info] IBM Results: 0000110001000000:364
  //[2017-11-29 15:01:42.761] [xacc-console] [info] Our Results:
  // 000001000000:364

  boost::dynamic_bitset<> m1(std::string("000000000000"));
  boost::dynamic_bitset<> m2(std::string("000000000000"));
  boost::dynamic_bitset<> m3(std::string("000001000000"));

  AcceleratorBuffer bigBuffer("qreg", 12);

  bigBuffer.appendMeasurement(m1, 1);
  bigBuffer.appendMeasurement(m2, 7827);
  bigBuffer.appendMeasurement(m3, 364);

  EXPECT_TRUE(std::fabs(bigBuffer.getExpectationValueZ() - 0.911011) < 1e-6);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
