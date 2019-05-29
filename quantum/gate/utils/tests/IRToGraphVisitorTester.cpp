/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#include "GateFunction.hpp"
#include "IRToGraphVisitor.hpp"
#include "DigitalGates.hpp"
#include "InstructionIterator.hpp"

using namespace xacc::quantum;

TEST(IRToGraphTester, checkSimple) {
  auto f = std::make_shared<GateFunction>("foo");

  auto x = std::make_shared<X>(0);
  auto h = std::make_shared<Hadamard>(1);
  auto cn1 = std::make_shared<CNOT>(1, 2);
  auto rz = std::make_shared<Rz>(1, 3.1415);
  auto z = std::make_shared<Z>(2);

  f->addInstruction(x);
  f->addInstruction(h);
  f->addInstruction(cn1);
  f->addInstruction(rz);
  f->addInstruction(z);

  auto vis = std::make_shared<IRToGraphVisitor>(3);
  xacc::InstructionIterator it(f);
  while (it.hasNext()) {
    auto inst = it.next();
    inst->accept(vis);
  }

  auto graph = vis->getGraph();

  graph->write(std::cout);
}

TEST(IRToGraphTester, checkCNOTLadder) {
  auto f = std::make_shared<GateFunction>("foo");

  auto cn1 = std::make_shared<CNOT>(0, 1);
  auto cn2 = std::make_shared<CNOT>(1, 2);
  auto cn3 = std::make_shared<CNOT>(1, 2);
  auto cn4 = std::make_shared<CNOT>(0, 1);

  f->addInstruction(cn1);
  f->addInstruction(cn2);
  f->addInstruction(cn3);
  f->addInstruction(cn4);

  auto vis = std::make_shared<IRToGraphVisitor>(3);
  xacc::InstructionIterator it(f);
  while (it.hasNext()) {
    auto inst = it.next();
    inst->accept(vis);
  }

  auto graph = vis->getGraph();

  graph->write(std::cout);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
