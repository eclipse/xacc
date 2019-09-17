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
#include "InstructionIterator.hpp"
#include "InstructionParameter.hpp"
#include "OpenPulseVisitor.hpp"
#include "TestPulse.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace xacc;
using namespace xacc::quantum;

TEST(OpenQasmVisitorTester, checkOpenPulseJson) {
  std::string file = "qcor_pulse_library.json";
  InstructionParameter q(std::vector<std::pair<double, double>>{
      {0.0, 0.0}, {0.5, 0.0}, {1.0, 0.0}});
  InstructionParameter q2(std::vector<std::pair<double, double>>{
      {0.0, 0.0}, {0.5, 0.0}, {1.0, 0.0}});
  xacc::appendCache(file, "sample1", q);
  xacc::appendCache(file, "sample2", q2);

  auto f = std::make_shared<GateFunction>("foo");

  auto x = std::make_shared<TestPulse>(std::vector<int>{0});
  x->setOption("pulse_id", InstructionParameter("sample1"));
  auto y = std::make_shared<TestPulse>(std::vector<int>{1});
  y->setOption("pulse_id", InstructionParameter("sample2"));

  f->addInstruction(x);
  f->addInstruction(y);

  // Create the Instruction Visitor that is going
  // to map our IR to Quil.
  auto visitor = std::make_shared<OpenPulseVisitor>(f->name());

  // Our QIR is really a tree structure
  // so create a pre-order tree traversal
  // InstructionIterator to walk it
  InstructionIterator it(f);
  while (it.hasNext()) {
    // Get the next node in the tree
    auto nextInst = it.next();
    if (nextInst->isEnabled())
      nextInst->accept(visitor);
  }

  std::cout << visitor->getOpenPulseInstructionsJson() << "\n";

  auto expected = R"expected({ "name": "foo", "instructions": [{ "ch": "d0", "name": "sample1", "t0":0 },{ "ch": "d1", "name": "sample2", "t0":0 }]})expected";
  EXPECT_TRUE(x->wasVisited());
//   EXPECT_EQ(expected, visitor->getOpenPulseInstructionsJson());

  std::string toRemove = xacc::getRootDirectory() + "/" + file;
  std::remove(toRemove.c_str());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
