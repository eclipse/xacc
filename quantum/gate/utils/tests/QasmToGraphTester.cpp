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
#include "QasmToGraph.hpp"
TEST(QasmToGraphTester, checkConversion) {
  const std::string qasm = "qubit qreg0\n"
                           "qubit qreg1\n"
                           "qubit qreg2\n"
                           "H qreg1\n"
                           "CNOT qreg1,qreg2\n"
                           "CNOT qreg0,qreg1\n"
                           "H qreg0\n"
                           "MeasZ qreg0\n"
                           "MeasZ qreg1\n"
                           "H qreg2\n"
                           "CNOT qreg2,qreg1\n"
                           "H qreg2\n"
                           "CNOT qreg2,qreg0";
  auto graph = xacc::quantum::QasmToGraph::getCircuitGraph(qasm);
  // FIXME Need to create a graph to check against
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
