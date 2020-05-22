/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Algorithm.hpp"

using namespace xacc;

TEST(QpeTester, checkSimple) 
{
  auto acc = xacc::getAccelerator("qpp", {std::make_pair("shots", 1024)});
  // Test case: T gate, eigenstate = |1>
  // 3-bit precision
  auto buffer = xacc::qalloc(4);
  auto qpe = xacc::getService<Algorithm>("QPE");
  auto compiler = xacc::getCompiler("xasm");
  
  // Oracle = T gate 
  // |1> => exp(i*pi/4) |1>
  // Expected result = |100> = |4>
  auto oracle = compiler->compile(R"(__qpu__ void oracle(qbit q) {
    T(q[0]); 
  })", nullptr)->getComposite("oracle");
  
  // Eigenstate preparation = X gate (|1> state)
  auto statePrep = compiler->compile(R"(__qpu__ void prep(qbit q) {
    X(q[0]); 
  })", nullptr)->getComposite("prep");  
  
  EXPECT_TRUE(qpe->initialize({
                    std::make_pair("accelerator", acc),
                    std::make_pair("oracle", oracle),
                    std::make_pair("state-preparation", statePrep)
                  }));
  qpe->execute(buffer);

  const auto result = buffer->computeMeasurementProbability("100");
  EXPECT_NEAR(result, 1.0, 0.01);
}

TEST(QpeTester, checkMultiQubitOracle) 
{
  auto acc = xacc::getAccelerator("qpp", {std::make_pair("shots", 1024)});
  // Test case: T gate on both qubits, eigenstate = |11>
  // i.e. TT |11> = exp(i*pi/4)* exp(i*pi/4)*|11> = exp(i*pi/2)*|11>
  // => the expected answer is 2 = 010

  // 3-bit precision => 5 qubits in total
  auto buffer = xacc::qalloc(5);
  auto qpe = xacc::getService<Algorithm>("QPE");
  auto compiler = xacc::getCompiler("xasm");
  
  // Oracle: T gate on both qubits
  auto oracle = compiler->compile(R"(__qpu__ void oracle(qbit q) {
    T(q[0]); 
    T(q[1]); 
  })", nullptr)->getComposite("oracle");

  // Eigenstate preparation = |11> state
  auto statePrep = compiler->compile(R"(__qpu__ void prep1(qbit q) {
    X(q[0]); 
    X(q[1]); 
  })", nullptr)->getComposite("prep1");  
  
  EXPECT_TRUE(qpe->initialize({
                    std::make_pair("accelerator", acc),
                    std::make_pair("oracle", oracle),
                    std::make_pair("state-preparation", statePrep)
                  }));
  qpe->execute(buffer);
  const auto result = buffer->computeMeasurementProbability("010");
  EXPECT_NEAR(result, 1.0, 0.01);
}

int main(int argc, char **argv) 
{
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
