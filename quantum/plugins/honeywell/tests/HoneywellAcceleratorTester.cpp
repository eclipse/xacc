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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include "xacc.hpp"
#include <gtest/gtest.h>

TEST(HoneywellAcceleratorTester, checkSimple) {
  xacc::set_verbose(true);
  auto accelerator = xacc::getAccelerator("honeywell:H1-1SC");
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void bell(qbit q) {
      H(q[0]);
      CX(q[0], q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })",
                                  accelerator);

  auto program = ir->getComposite("bell");

  auto buffer = xacc::qalloc(2);
  accelerator->execute(buffer, program);

  buffer->print(std::cout);
}

TEST(HoneywellAcceleratorTester, checkConditionalTeleport) {
  xacc::set_verbose(true);
  auto accelerator = xacc::getAccelerator("honeywell:H1-1SC");
  auto buffer = xacc::qalloc(3);  
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void teleport(qbit q) {
        // State preparation (Bob)
        Rx(q[0], 0.6);
        // Bell channel setup
        H(q[1]);
        CX(q[1], q[2]);
        // Alice Bell measurement
        CX(q[0], q[1]);
        H(q[0]);
        Measure(q[0]);
        Measure(q[1]);
        // Correction
        if (q[0])
        {
            Z(q[2]);
        }
        if (q[1])
        {
            X(q[2]);
        }
        // Measure teleported qubit
        Measure(q[2]);
    })",
                                  accelerator);

  accelerator->execute(buffer, ir->getComposites()[0]);

  buffer->print();
}

TEST(HoneywellAcceleratorTester, checkConditionalIQPE) {
  xacc::set_verbose(true);
  auto accelerator = xacc::getAccelerator("honeywell:H1-1SC");
  auto buffer = xacc::qalloc(3);  
  auto xasmCompiler = xacc::getCompiler("xasm");
  // Make sure we can validate it.
  auto ir = xasmCompiler->compile(R"(__qpu__ void IQPE(qbit q) {
    H(q[0]);
    X(q[1]);
    // Prepare the state:
    CPhase(q[0], q[1], -5*pi/8);
    CPhase(q[0], q[1], -5*pi/8);
    CPhase(q[0], q[1], -5*pi/8);
    CPhase(q[0], q[1], -5*pi/8);
    CPhase(q[0], q[1], -5*pi/8);
    CPhase(q[0], q[1], -5*pi/8);
    CPhase(q[0], q[1], -5*pi/8);
    CPhase(q[0], q[1], -5*pi/8);
    H(q[0]);
    // Measure and reset
    Measure(q[0], c[0]);
    Reset(q[0]);
    H(q[0]);
    CPhase(q[0], q[1], -5*pi/8);
    CPhase(q[0], q[1], -5*pi/8);
    CPhase(q[0], q[1], -5*pi/8);
    CPhase(q[0], q[1], -5*pi/8);
    // Conditional rotation
    if(c[0]) {
        Rz(q[0], -pi/2);
    }
    H(q[0]);
    Measure(q[0], c[1]);
    Reset(q[0]);
    H(q[0]);
    CPhase(q[0], q[1], -5*pi/8);
    CPhase(q[0], q[1], -5*pi/8);
    if(c[0]) {
        Rz(q[0], -pi/4);
    }
    if(c[1]) {
        Rz(q[0], -pi/2);
    }
    H(q[0]);
    Measure(q[0], c[2]);
    Reset(q[0]);
    H(q[0]);
    CPhase(q[0], q[1], -5*pi/8);
    if(c[0]) {
        Rz(q[0], -pi/8);
    }
    if(c[1]) {
        Rz(q[0], -pi/4);
    }
    if(c[2]) {
        Rz(q[0], -pi/2);
    }
    H(q[0]);
    Measure(q[0], c[3]);
    })",
                                  accelerator);

  accelerator->execute(buffer, ir->getComposites()[0]);
  buffer->print();
}

int main(int argc, char **argv) {
  xacc::Initialize();
  //   xacc::set_verbose(true);
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();

  xacc::Finalize();

  return result;
}
