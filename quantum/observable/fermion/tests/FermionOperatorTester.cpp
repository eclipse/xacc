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
#include "FermionOperator.hpp"
#include "xacc.hpp"
#include "IRProvider.hpp"
#include "xacc_service.hpp"

using namespace xacc::quantum;

TEST(FermionOperatorTester, checkDanielBug) {

    const std::string str = R"#((-0.165607,-0)  2^ 1^ 2 1 + (0.1202,0)  1^ 0^ 1 0 + (-0.0454063,-0)  3^ 0^ 2 1 + (0.168336,0)  2^ 0^ 2 0 + (0.0454063,0)  2^ 1^ 3 0 + (0.168336,0)  2^ 0^ 2 0 + (0.165607,0)  3^ 0^ 3 0 + (-0.0454063,-0)  3^ 0^ 2 1 + (-0.0454063,-0)  3^ 1^ 2 0 + (-0.0454063,-0)  3^ 1^ 2 0 + (0.165607,0)  2^ 1^ 2 1 + (-0.165607,-0)  3^ 0^ 3 0 + (-0.479678,-0)  3^ 3 + (-0.0454063,-0)  2^ 1^ 3 0 + (-0.174073,-0)  3^ 1^ 3 1 + (-0.0454063,-0)  2^ 0^ 3 1 + (0.1202,0)  1^ 0^ 1 0 + (0.0454063,0)  2^ 0^ 3 1 + (0.174073,0)  3^ 1^ 3 1 + (0.165607,0)  2^ 1^ 2 1 + (-0.0454063,-0)  2^ 1^ 3 0 + (-0.1202,-0)  3^ 2^ 3 2 + (0.1202,0)  3^ 2^ 3 2 + (-0.168336,-0)  2^ 0^ 2 0 + (0.1202,0)  3^ 2^ 3 2 + (-0.1202,-0)  3^ 2^ 3 2 + (0.0454063,0)  3^ 1^ 2 0 + (-1.24885,-0)  0^ 0 + (0.0454063,0)  3^ 1^ 2 0 + (-0.168336,-0)  2^ 0^ 2 0 + (0.165607,0)  3^ 0^ 3 0 + (-0.0454063,-0)  2^ 0^ 3 1 + (0.0454063,0)  2^ 0^ 3 1 + (-1.24885,-0)  2^ 2 + (0.0454063,0)  2^ 1^ 3 0 + (0.174073,0)  3^ 1^ 3 1 + (-0.479678,-0)  1^ 1 + (-0.174073,-0)  3^ 1^ 3 1 + (0.0454063,0)  3^ 0^ 2 1 + (-0.165607,-0)  3^ 0^ 3 0 + (0.0454063,0)  3^ 0^ 2 1 + (-0.165607,-0)  2^ 1^ 2 1 + (-0.1202,-0)  1^ 0^ 1 0 + (-0.1202,-0)  1^ 0^ 1 0 + (0.708024,0))#";
    FermionOperator op(str);

    std::cout << op.toString() << "\n";
}

TEST(FermionOperatorTester,checkSimple) {

  FermionOperator op(Operators{{3,1},{2,0}}, 2.2);

  EXPECT_TRUE(op == op);

  std::cout << "HI: " << (op*op).toString() << "\n";
  EXPECT_TRUE(op*op == FermionOperator());

  auto op2 = op + op;

  std::cout << op2.toString() << "\n";

  EXPECT_TRUE(-1.*op2 == -1.*(op+op));

}

TEST(FermionOperatorTester,checkFromStr) {

  std::string s("3.3 4^ 3^ 2^ 1^ 4 3 2 1");

  FermionOperator op(s);

  std::cout << op.toString() << "\n";

  std::string s2 = R"s2(0.7080240949826064
- 1.248846801817026 0^ 0
- 1.248846801817026 1^ 1
- 0.4796778151607899 2^ 2
- 0.4796778151607899 3^ 3 +
0.33667197218932576 0^ 1^ 1 0)s2";

FermionOperator op2(s2);

std::cout << op2.toString() << "\n";

}

TEST(FermionOperatorTester, checkFromStr2) {
    std::string src = ".12 0^ 1^ 0 1 - .12 0^ 1^ 1 0";
    FermionOperator op(src);
    std::cout << op.toString() << "\n";
}
int main(int argc, char** argv) {
    xacc::Initialize(argc,argv);
   ::testing::InitGoogleTest(&argc, argv);
   auto ret = RUN_ALL_TESTS();
   xacc::Finalize();
   return ret;
}
