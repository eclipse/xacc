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
#include "QFT.hpp"
// #include "JsonVisitor.hpp"
#include "GateIR.hpp"
#include "XACC.hpp"
#include "DigitalGates.hpp"

using namespace xacc;
using namespace xacc::quantum;

TEST(QFTTester, checkCreation) {

  xacc::Initialize();
  auto qft = std::make_shared<QFT>();

  auto buffer = std::make_shared<AcceleratorBuffer>("", 3);
  auto qftKernel = qft->generate(buffer);
  auto ir = std::make_shared<GateIR>();
  ir->addKernel(qftKernel);

//   JsonVisitor v(qftKernel);
//   std::cout << v.write() << "\n";

  auto h1 = std::make_shared<Hadamard>(2);
  auto cphase1 = std::make_shared<CPhase>(1, 2, 1.5707963);
  auto h2 = std::make_shared<Hadamard>(2);
  auto cphase2 = std::make_shared<CPhase>(0, 2, 0.78539815);
  auto cphase3 = std::make_shared<CPhase>(0, 1, 1.5707963);
  auto h3 = std::make_shared<Hadamard>(0);
  auto swap = std::make_shared<Swap>(0, 2);

  auto expectedF = std::make_shared<GateFunction>("qft");
  expectedF->addInstruction(h1);
  expectedF->addInstruction(cphase1);
  expectedF->addInstruction(h2);
  expectedF->addInstruction(cphase2);
  expectedF->addInstruction(cphase3);
  expectedF->addInstruction(h3);
  expectedF->addInstruction(swap);

  auto expectedIR = std::make_shared<GateIR>();
  expectedIR->addKernel(expectedF);
  xacc::Finalize();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
