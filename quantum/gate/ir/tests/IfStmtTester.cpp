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
#include <gtest/gtest.h>
#include <stdexcept>
#include <fstream>

#include "Instruction.hpp"
#include "xacc.hpp"
#include "xacc_config.hpp"
#include "xacc_service.hpp"
#include "Utils.hpp"

TEST(IfStmtTester, checkBasic) {
  // TODO load from json file prototypical pulses and cmd_defs...

  auto q = xacc::qalloc(1);
  q->setName("q");
  xacc::storeBuffer(q);

  auto p = xacc::getIRProvider("quantum");
  auto ifstmt =
      p->createInstruction("ifstmt", std::vector<std::size_t>{0}, {"q"});
  EXPECT_EQ(0, ifstmt->bits()[0]);
  EXPECT_EQ("q", ifstmt->getParameter(0).as<std::string>());

  auto h = p->createInstruction("H", std::vector<std::size_t>{0});
  auto cx = p->createInstruction("CX", std::vector<std::size_t>{0, 1});

  xacc::ir::asComposite(ifstmt)->addInstructions({h, cx});

  std::cout << "Before measurement:\n" << ifstmt->toString() << "\n";

  q->measure(0, 1);

  xacc::ir::asComposite(ifstmt)->expand({});
  std::cout << "After measurement:\n" << ifstmt->toString() << "\n";

  q->reset_single_measurements();
  ifstmt->disable();

  std::cout << "After reset:\n" << ifstmt->toString() << "\n";

  q->measure(0, 0);

  xacc::ir::asComposite(ifstmt)->expand({});
  std::cout << "After 2nd measurement:\n" << ifstmt->toString() << "\n";

  q->reset_single_measurements();
  ifstmt->disable();

  q->measure(0, 1);

  xacc::ir::asComposite(ifstmt)->expand({});
  std::cout << "After 3rd measurement:\n" << ifstmt->toString() << "\n";
}
int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
