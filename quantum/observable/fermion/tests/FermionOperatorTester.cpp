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

    auto str = std::string(
    "(-0.1656068,0)1^ 2^ 1 2 + (0.1202005,0)1^ 0^ 0 1 + "
    "(-0.0454063,0)0^ 3^ 1 2 + (0.168336,0)2^ 0^ 0 2 + "
    "(0.0454063,0)1^ 2^ 3 0 + (0.168336,0)0^ 2^ 2 0 + "
    "(0.1656068,0)0^ 3^ 3 0 + (-0.0454063,0)3^ 0^ 2 1 + "
    "(-0.0454063,0)1^ 3^ 0 2 + (-0.0454063,0)3^ 1^ 2 0 + "
    "(0.1656068,0)1^ 2^ 2 1 + (-0.1656068,0)0^ 3^ 0 3 + "
    "(-0.4796778,0)3^ 3 + (-0.0454063,0)1^ 2^ 0 3 + "
    "(-0.174073,0)1^ 3^ 1 3 + (-0.0454063,0)0^ 2^ 1 3 + "
    "(0.1202005,0)0^ 1^ 1 0 + (0.0454063,0)0^ 2^ 3 1 + "
    "(0.174073,0)1^ 3^ 3 1 + (0.1656068,0)2^ 1^ 1 2 + "
    "(-0.0454063,0)2^ 1^ 3 0 + (-0.1202005,0)2^ 3^ 2 3 + "
    "(0.1202005,0)2^ 3^ 3 2 + (-0.168336,0)0^ 2^ 0 2 + "
    "(0.1202005,0)3^ 2^ 2 3 + (-0.1202005,0)3^ 2^ 3 2 + "
    "(0.0454063,0)1^ 3^ 2 0 + (-1.2488468,0)0^ 0 + "
    "(0.0454063,0)3^ 1^ 0 2 + (-0.168336,0)2^ 0^ 2 0 + "
    "(0.1656068,0)3^ 0^ 0 3 + (-0.0454063,0)2^ 0^ 3 1 + "
    "(0.0454063,0)2^ 0^ 1 3 + (-1.2488468,0)2^ 2 + "
    "(0.0454063,0)2^ 1^ 0 3 + (0.174073,0)3^ 1^ 1 3 + "
    "(-0.4796778,0)1^ 1 + (-0.174073,0)3^ 1^ 3 1 + "
    "(0.0454063,0)3^ 0^ 1 2 + (-0.1656068,0)3^ 0^ 3 0 + "
    "(0.0454063,0)0^ 3^ 2 1 + (-0.1656068,0)2^ 1^ 2 1 + "
    "(-0.1202005,0)0^ 1^ 0 1 + (-0.1202005,0)1^ 0^ 1 0 + "
    "(0.7080241,0)");

    FermionOperator op(str);

    std::cout << op.toString() << "\n"  << op.nBits() << "\n";
}

TEST(FermionOperatorTester, checkAnotherBug) {
  FermionOperator ad1(xacc::quantum::Operators{{1, true}});
  FermionOperator a0(xacc::quantum::Operators{{0, false}});
  auto t = ad1 * a0;
  std::cout << "HELLO: " <<  t.toString() << "\n";
}

TEST(FermionOperatorTester, checkBug) {
  // Two equivalent permutations of the result:
  auto str1 = R"((-0.479678,-0) 1^ 1 + (0.708024,0) + (0.0454063,0) 0^ 3^ 2 1)";
  auto str2 = R"((0.0454063,0) 0^ 3^ 2 1 + (0.708024,0) + (-0.479678,-0) 1^ 1)";
  FermionOperator op(str1);
  std::cout << op.toString() << "\n";
  EXPECT_TRUE(op.toString() == str1 || op.toString() == str2);
}

TEST(FermionOperatorTester,checkSimple) {

  FermionOperator op(Operators{{3,1},{2,0}}, 2.2);

  EXPECT_TRUE(op == op);

  std::cout << "HI: " << (op*op).toString() << "\n";
  EXPECT_TRUE(op*op == FermionOperator(Operators{{3,1},{2,0},{3,1},{2,0}}, 4.84));

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

TEST(FermionOperatorTester, checkSciNot) {
    std::string src = "1.234e-10 0^ 1^ 3 2";
    FermionOperator op1(src);
    std::cout << op1.toString() << "\n";

    src = "(1.234e-10,0) 0^ 1^ 3 2";
    FermionOperator op2(src);
    std::cout << op2.toString() << "\n";

    src = "(1e-10,0) 0^ 1^ 3 2";
    FermionOperator op3(src);
    std::cout << op3.toString() << "\n";
}


TEST(FermionOperatorTester, checkMult) {
    FermionOperator op1("3^ 2^ 3 2"), op2("0^ 3 1^ 2");

    for (auto b : op1.getTerms()){std::cout << "First " << b.second.id() << "\n";}
    for (auto b : op2.getTerms()){std::cout << "Second " << b.second.id() << "\n";}

    FermionTerm mult;
    for (auto a : op1.getTerms()){
      for (auto b : op2.getTerms()){
        mult = a.second * b.second;
      }
    }

    for (auto k : mult.ops()){
      std::cout << k.first << " " << k.second << "\n";
    }

    std::cout << "Print op =" << (op1*op2).toString() << "\n\n";
    EXPECT_TRUE((op1*op2) == FermionOperator("3^ 2^ 3 2 0^ 3 1^ 2"));

    FermionOperator op3("(0, 1)3^ 2^ 3 2"), op4("(0, 1) 0^ 1 1^ 0");
    std::cout << "Print op =" << (op3*op4).toString() << "\n\n";
    EXPECT_TRUE((op3 * op4) == FermionOperator("3^ 2^ 3 2 0^ 1 1^ 0"));

}

TEST(FermionOperatorTester, checkHermitianConjugate) {
    std::string src = "3^ 2^ 1 0";
    FermionOperator op(src);
    auto opHC = op.hermitianConjugate();

    std::cout << "Operator" << "\t" << op.toString() << "\n";
    std::cout << "Hermitian conjugate" << "\t" << opHC.toString() << "\n";

}

TEST(FermionOperatorTester, checkNormalize) {
    std::string src = "1^ 0 + (0,1)2^ 3";
    FermionOperator op(src);
    std::cout << op.toString() << "\n";
    op.normalize();
    std::cout << op.toString() << "\n";

}

TEST(FermionOperatorTester, normalOrder) {

  FermionOperator op1("(1, 0) 1 0^");
  std::cout << op1.normalOrder()->toString() << "\n";
  EXPECT_TRUE(*std::dynamic_pointer_cast<FermionOperator>(op1.normalOrder()) == FermionOperator("(-1, 0) 0^ 1"));

  FermionOperator op2("(1, 0) 1^ 0");
  std::cout << op2.normalOrder()->toString() << "\n";
  EXPECT_TRUE(*std::dynamic_pointer_cast<FermionOperator>(op2.normalOrder()) == op2);

  FermionOperator op3("(1, 0) 0^ 1");
  std::cout << op3.normalOrder()->toString() << "\n";
  EXPECT_TRUE(*std::dynamic_pointer_cast<FermionOperator>(op3.normalOrder()) == op3);

  FermionOperator op4("(1, 0) 0 1^");
  std::cout << op4.normalOrder()->toString() << "\n";
  EXPECT_TRUE(*std::dynamic_pointer_cast<FermionOperator>(op4.normalOrder()) == (-1.0 * op2));

  FermionOperator op5("(1, 0) 1^ 0 0^ 1");
  std::cout << op5.normalOrder()->toString() << "\n";
  EXPECT_TRUE(*std::dynamic_pointer_cast<FermionOperator>(op5.normalOrder()) == FermionOperator("1^ 1 + 1^ 0^ 1 0"));

  FermionOperator op6("(1, 0) 0 1 2^ 3^");
  std::cout << op6.normalOrder()->toString() << "\n";
  EXPECT_TRUE(*std::dynamic_pointer_cast<FermionOperator>(op6.normalOrder()) == FermionOperator("3^ 2^ 1 0"));

  FermionOperator op7("(1, 0) 0 1 2^ 3^ 0^");
  std::cout << op7.normalOrder()->toString() << "\n";
  EXPECT_TRUE(*std::dynamic_pointer_cast<FermionOperator>(op7.normalOrder()) == FermionOperator("3^ 2^ 1 + 3^ 2^ 0^ 1 0"));

  FermionOperator op8("(1, 0) 0 0^ 4^ 0 1^ 2 0^");
  std::cout << op8.normalOrder()->toString() << "\n";
  EXPECT_TRUE(*std::dynamic_pointer_cast<FermionOperator>(op8.normalOrder()) == FermionOperator("4^ 1^ 2 + 4^ 1^ 0^ 2 0"));
}

int main(int argc, char** argv) {
    xacc::Initialize(argc,argv);
   ::testing::InitGoogleTest(&argc, argv);
   auto ret = RUN_ALL_TESTS();
   xacc::Finalize();
   return ret;
}
