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
 *  Joseph N. Huber - Initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>
#include "IRVerifier.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

TEST(qcecTester, checkSimpleHadamard) {
  auto verifier = xacc::getService<xacc::IRVerifier>("qcec");
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program1 = xasmCompiler
                      ->compile(R"(__qpu__ void test1(qbit q) {
      H(q[0]);
      H(q[0]);
      H(q[0]);
      CX(q[0], q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })")
                      ->getComposites()[0];
  auto program2 = xasmCompiler
                      ->compile(R"(__qpu__ void test2(qbit q) {
      H(q[0]);
      CX(q[0], q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })")
                      ->getComposites()[0];

  EXPECT_TRUE(
      verifier->verify(program1, program2, {{"method", "lookahead"}}).first);
}

TEST(qcecTester, checkUnequivalent) {
  auto verifier = xacc::getService<xacc::IRVerifier>("qcec");
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program1 = xasmCompiler
                      ->compile(R"(__qpu__ void test1(qbit q) {
      H(q[0]);
      CX(q[0], q[1]);
    })")
                      ->getComposites()[0];
  auto program2 = xasmCompiler
                      ->compile(R"(__qpu__ void test2(qbit q) {
      H(q[0]);
      CY(q[0], q[1]);
    })")
                      ->getComposites()[0];

  EXPECT_FALSE(verifier
                   ->verify(program1, program2,
                            {{"method", "simulation"},
                             {"stimuli", "globalquantum"},
                             {"numsims", 20}})
                   .first);
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();
  xacc::Finalize();
  return result;
}
