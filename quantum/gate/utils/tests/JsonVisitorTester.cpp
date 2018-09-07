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
#include "GateFunction.hpp"
#include "JsonVisitor.hpp"
#include "Rz.hpp"

using namespace rapidjson;

using namespace xacc::quantum;

TEST(JsonVisitorTester, checkSimpleSerialization) {
  auto f = std::make_shared<GateFunction>("foo");

  auto x = std::make_shared<X>(0);
  auto h = std::make_shared<Hadamard>(1);
  auto cn1 = std::make_shared<CNOT>(1, 2);
  auto rz = std::make_shared<Rz>(1, 3.1415);
  auto cond1 = std::make_shared<ConditionalFunction>(0);
  auto z = std::make_shared<Z>(2);
  cond1->addInstruction(z);

  f->addInstruction(x);
  f->addInstruction(h);
  f->addInstruction(cn1);
  f->addInstruction(rz);
  f->addInstruction(cond1);

  JsonVisitor visitor(f);

  auto json = visitor.write();

  std::cout << json << "\n";
}

TEST(JsonVisitorTester, checkTeleportSerialization) {
  auto f = std::make_shared<GateFunction>("foo");

  auto x = std::make_shared<X>(0);
  auto h = std::make_shared<Hadamard>(1);
  auto cn1 = std::make_shared<CNOT>(1, 2);
  auto cn2 = std::make_shared<CNOT>(0, 1);
  auto h2 = std::make_shared<Hadamard>(0);
  auto m0 = std::make_shared<Measure>(0, 0);
  auto m1 = std::make_shared<Measure>(1, 1);

  auto cond1 = std::make_shared<ConditionalFunction>(0);
  auto z = std::make_shared<Z>(2);
  cond1->addInstruction(z);
  auto cond2 = std::make_shared<ConditionalFunction>(1);
  auto x2 = std::make_shared<X>(2);
  cond2->addInstruction(x2);

  f->addInstruction(x);
  f->addInstruction(h);
  f->addInstruction(cn1);
  f->addInstruction(cn2);
  f->addInstruction(h2);
  f->addInstruction(m0);
  f->addInstruction(m1);
  f->addInstruction(cond1);
  f->addInstruction(cond2);

  JsonVisitor visitor(f);

  auto json = visitor.write();

  std::cout << "HELLO: \n" << json << "\n";
}

TEST(JsonVisitorTester, checkFunctionWithFunction) {

  auto f = std::make_shared<GateFunction>("foo");
  auto init = std::make_shared<GateFunction>("init");

  xacc::InstructionParameter p = "phi";
  auto rz = std::make_shared<Rz>(std::vector<int>{0});
  rz->setParameter(0, p);

  init->addInstruction(rz);

  f->addInstruction(init);

  auto x = std::make_shared<X>(0);
  auto h = std::make_shared<Hadamard>(1);

  f->addInstruction(x);
  f->addInstruction(h);

  JsonVisitor visitor(f);

  auto json = visitor.write();

  std::cout << "HELLO: \n" << json << "\n";
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
