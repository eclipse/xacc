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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "gtest/gtest.h"

#include "xacc.hpp"
#include "xacc_service.hpp"

TEST(Staq_RotationFoldingTester, checkSimple) {
  auto irt = xacc::getIRTransformation("rotation-folding");
  auto compiler = xacc::getCompiler("xasm");
  auto program = compiler->compile(R"(__qpu__ void test_t_t(qreg q) {
      T(q[0]);
      T(q[0]);
  })")->getComposite("test_t_t");

  EXPECT_EQ(2, program->nInstructions());
  EXPECT_EQ("T", program->getInstruction(0)->name());
  EXPECT_EQ("T", program->getInstruction(1)->name());

  irt->apply(program, nullptr);

  EXPECT_EQ(1, program->nInstructions());
  EXPECT_EQ("S", program->getInstruction(0)->name());


  program = compiler->compile(R"(__qpu__ void test_t_tdg(qreg q) {
      T(q[0]);
      Tdg(q[0]);
  })")->getComposite("test_t_tdg");

  EXPECT_EQ(2, program->nInstructions());
  EXPECT_EQ("T", program->getInstruction(0)->name());
  EXPECT_EQ("Tdg", program->getInstruction(1)->name());

  irt->apply(program, nullptr);

  EXPECT_EQ(0, program->nInstructions());

  program = compiler->compile(R"(__qpu__ void test_conj_merge(qreg q) {
      H(q[0]);
      T(q[0]);
      H(q[0]);
      X(q[0]);
      H(q[0]);
      T(q[0]);
      H(q[0]);
  })")->getComposite("test_conj_merge");

  irt->apply(program, nullptr);

  EXPECT_EQ(4, program->nInstructions());
  EXPECT_EQ("X", program->getInstruction(0)->name());
  EXPECT_EQ("H", program->getInstruction(1)->name());
  EXPECT_EQ("S", program->getInstruction(2)->name());
  EXPECT_EQ("H", program->getInstruction(3)->name());

  program = compiler->compile(R"(__qpu__ void test_rz_merge(qreg q) {
     Rz(q[0],pi/16);
     Rz(q[0], pi/16);
  })")->getComposite("test_rz_merge");
  irt->apply(program, nullptr);

  EXPECT_EQ(1, program->nInstructions());
  EXPECT_EQ("Rz", program->getInstruction(0)->name());
  EXPECT_NEAR(.3927, program->getInstruction(0)->getParameter(0).as<double>(), 1e-3);

  program = compiler->compile(R"(__qpu__ void test_cx_merge(qreg q) {
     CX(q[0],q[1]);
     T(q[1]);
     CX(q[0], q[1]);
     CX(q[1], q[0]);
     T(q[0]);
     CX(q[1],q[0]);
  })")->getComposite("test_cx_merge");
  irt->apply(program, nullptr);

  EXPECT_EQ(3, program->nInstructions());
  EXPECT_EQ("CNOT", program->getInstruction(0)->name());
  EXPECT_EQ(1, program->getInstruction(0)->bits()[0]);
  EXPECT_EQ(0, program->getInstruction(0)->bits()[1]);
  EXPECT_EQ("S", program->getInstruction(1)->name());
  EXPECT_EQ("CNOT", program->getInstruction(2)->name());
  EXPECT_EQ(1, program->getInstruction(2)->bits()[0]);
  EXPECT_EQ(0, program->getInstruction(2)->bits()[1]);

  program = compiler->compile(R"(__qpu__ void test_rz_merge_float(qreg q) {
     Rz(q[0], 0.000000025);
     Rz(q[0], 0.000000025);
  })")->getComposite("test_rz_merge_float");
  irt->apply(program, nullptr);

  EXPECT_EQ(1, program->nInstructions());
  EXPECT_EQ("Rz", program->getInstruction(0)->name());
  EXPECT_NEAR(0.00000005, program->getInstruction(0)->getParameter(0).as<double>(), 1e-12);
}


int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  xacc::set_verbose(true);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
