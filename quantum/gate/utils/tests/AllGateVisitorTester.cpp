/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>
#include "AllGateVisitorTester.hpp"
#include "xacc.hpp"
#include "Circuit.hpp"

using namespace xacc::quantum;

TEST(AllGateVisitorTester, checkVisit) {
  auto f = std::make_shared<Circuit>("foo");

  auto h = std::make_shared<Hadamard>(0);
  auto i = std::make_shared<Identity>(0);
  auto cp = std::make_shared<CPhase>(std::vector<std::size_t>{0,1});
  auto x = std::make_shared<X>(0);
  auto y = std::make_shared<Y>(0);
  auto z = std::make_shared<Z>(0);
  auto rx = std::make_shared<Rx>(std::vector<std::size_t>{0});
  auto ry = std::make_shared<Ry>(std::vector<std::size_t>{0});
  auto rz = std::make_shared<Rz>(std::vector<std::size_t>{0});
  auto cz = std::make_shared<CZ>(0,1);
  auto u = std::make_shared<U>(std::vector<std::size_t>{0});
  auto cn = std::make_shared<CNOT>(0, 1);
  auto sw = std::make_shared<Swap>(0,1);

  auto unknown = std::make_shared<Unknown>(0);

  f->addInstruction(h);
  f->addInstruction(cn);
  f->addInstruction(i);
  f->addInstruction(cp);
  f->addInstruction(x);
  f->addInstruction(y);
  f->addInstruction(z);
  f->addInstruction(rx);
  f->addInstruction(ry);
  f->addInstruction(rz);
  f->addInstruction(cz);
  f->addInstruction(u);
  f->addInstruction(sw);
  f->addInstruction(unknown);

  auto vis = std::make_shared<TestAllGateVisitor>();
  xacc::InstructionIterator it(f);
  while (it.hasNext()) {
    auto inst = it.next();
    inst->accept(vis);
  }

  EXPECT_TRUE(vis->foundCp);
  EXPECT_TRUE(vis->foundCX);
  EXPECT_TRUE(vis->foundI);
  EXPECT_TRUE(vis->foundH);
  EXPECT_TRUE(vis->foundX);
  EXPECT_TRUE(vis->foundY);
  EXPECT_TRUE(vis->foundZ);
  EXPECT_TRUE(vis->foundRx);
  EXPECT_TRUE(vis->foundRy);
  EXPECT_TRUE(vis->foundRz);
  EXPECT_TRUE(vis->foundSw);
  EXPECT_TRUE(vis->foundU);
  EXPECT_TRUE(vis->foundCz);
  EXPECT_TRUE(unknown->wasVisited);
  EXPECT_EQ("hello", vis->getNativeAssembly());

  std::cout << vis->getNativeAssembly() << "\n";

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
