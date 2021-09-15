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
#include "Circuit.hpp"
#include "IRToGraphVisitor.hpp"
#include "CommonGates.hpp"
#include "InstructionIterator.hpp"
#include "xacc.hpp"

using namespace xacc::quantum;

TEST(IRToGraphTester, checkSimple) {
  auto f = std::make_shared<Circuit>("foo");

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
  EXPECT_EQ(graph->order(), f->nInstructions() + 2);
  // X and H are on the first layer
  EXPECT_EQ(graph->getVertexProperties(1).getString("name"), x->name());
  EXPECT_EQ(graph->getVertexProperties(1).get<int>("layer"), 0);
  EXPECT_EQ(graph->getVertexProperties(2).getString("name"), h->name());
  EXPECT_EQ(graph->getVertexProperties(2).get<int>("layer"), 0);

  // CNOT on the second layer
  EXPECT_EQ(graph->getVertexProperties(3).getString("name"), "CNOT");
  EXPECT_EQ(graph->getVertexProperties(3).get<int>("layer"), 1);

  // Rz and Z on the third layer
  EXPECT_EQ(graph->getVertexProperties(4).getString("name"), rz->name());
  EXPECT_EQ(graph->getVertexProperties(4).get<int>("layer"), 2);

  EXPECT_EQ(graph->getVertexProperties(5).getString("name"), z->name());
  EXPECT_EQ(graph->getVertexProperties(5).get<int>("layer"), 2);
  EXPECT_EQ(f->depth(), 3);
}

TEST(IRToGraphTester, checkCNOTLadder) {
  auto f = std::make_shared<Circuit>("foo");

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
  // These CNOT gates cannot be on the same layer.
  for (int i = 1; i < graph->order() - 2; i++) {
    auto node = graph->getVertexProperties(i);
    EXPECT_EQ(node.getString("name"), "CNOT");
    EXPECT_EQ(node.get<int>("layer"), i - 1);
  }
  EXPECT_EQ(f->depth(), 4);
}

TEST(IRToGraphTester, checkSingleGate) {
  auto f = std::make_shared<Circuit>("foo");
  auto x = std::make_shared<X>(0);
  f->addInstruction(x);
  f->toGraph()->write(std::cout);
  EXPECT_EQ(f->depth(), 1);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
