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
#include <memory>
#include <gtest/gtest.h>
#include "QuilVisitor.hpp"
#include "InstructionIterator.hpp"
#include "xacc.hpp"
#include "Circuit.hpp"

using namespace xacc;
using namespace xacc::quantum;

TEST(QuilVisitorTester, checkIRToQuil) {

  auto f = std::make_shared<Circuit>("foo");

  auto x = std::make_shared<X>(0);
  auto h = std::make_shared<Hadamard>(1);
  auto cn1 = std::make_shared<CNOT>(1, 2);
  auto cn2 = std::make_shared<CNOT>(0, 1);
  auto h2 = std::make_shared<Hadamard>(0);
  auto m0 = std::make_shared<Measure>(0, 0);
  auto m1 = std::make_shared<Measure>(1, 1);

//   auto cond1 = std::make_shared<ConditionalFunction>(0);
//   auto z = std::make_shared<Z>(2);
//   cond1->addInstruction(z);
//   auto cond2 = std::make_shared<ConditionalFunction>(1);
//   auto x2 = std::make_shared<X>(2);
//   cond2->addInstruction(x2);

  f->addInstruction(x);
  f->addInstruction(h);
  f->addInstruction(cn1);
  f->addInstruction(cn2);
  f->addInstruction(h2);
  f->addInstruction(m0);
  f->addInstruction(m1);
//   f->addInstruction(cond1);
//   f->addInstruction(cond2);

  // Create the Instruction Visitor that is going
  // to map our IR to Quil.
  auto visitor = std::make_shared<QuilVisitor>();

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

  std::cout << visitor->getQuilString() << "\n";

//   std::string expectedQuil = "X 0\n"
//                              "H 1\n"
//                              "CNOT 1 2\n"
//                              "CNOT 0 1\n"
//                              "H 0\n"
//                              "MEASURE 0 [0]\n"
//                              "MEASURE 1 [1]\n;

//   EXPECT_TRUE(expectedQuil == visitor->getQuilString());
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
