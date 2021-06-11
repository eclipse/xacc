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
#include "PauliOperator.hpp"
#include "xacc.hpp"
#include "IRProvider.hpp"
#include <Eigen/Dense>
#include "xacc_service.hpp"

using namespace xacc::quantum;
TEST(PauliOperatorTester, checkSpMat) {

  PauliOperator op("X0 + Z0");

  auto sp_mat = op.to_sparse_matrix();

  for (auto &s : sp_mat) {
    std::cout << s.coeff() << ", " << s.row() << ", " << s.col() << "\n";
  }

  PauliOperator op2(
      "(0.1202,0) Z2 Z3 + (0.174073,0) Z1 Z3 + (0.0454063,0) Y0 Y1 Y2 Y3 + "
      "(0.17028,0) Z0 + (0.0454063,0) X0 X1 Y2 Y3 + (-0.220041,0) Z1 + "
      "(0.165607,0) Z1 Z2 + (0.165607,0) Z0 Z3 + (0.17028,0) Z2 + "
      "(0.0454063,0) X0 X1 X2 X3 + (-0.106477,0) + (0.1202,0) Z0 Z1 + "
      "(0.0454063,0) Y0 Y1 X2 X3 + (0.168336,0) Z0 Z2 + (-0.220041,0) Z3");

  auto s = op2.to_sparse_matrix();
  for (auto &ss : s) {
    std::cout << ss.coeff() << ", " << ss.row() << ", " << ss.col() << "\n";
  }
}
TEST(PauliOperatorTester, checkEasy) {

  //	CURRENT: (0.25,0) X1 Z2 X3 + (0,0.25) X1 Z2 Y3 + (0.25,0) Y1 Z2 Y3 +
  //(0,-0.25) Y1 Z2 X3 	SUM: (0,-0.5) Z0 Y1 + (0.5,0) Z0 X1

  PauliOperator op({{1, "X"}, {2, "Z"}, {3, "X"}}, .25);
  op += PauliOperator({{1, "X"}, {2, "Z"}, {3, "Y"}},
                      std::complex<double>(0, .25));
  op += PauliOperator({{1, "Y"}, {2, "Z"}, {3, "Y"}}, .25);
  op += PauliOperator({{1, "Y"}, {2, "Z"}, {3, "X"}},
                      std::complex<double>(0, -.25));
  EXPECT_EQ(4, op.nQubits());

  PauliOperator sum({{0, "Z"}, {1, "Y"}}, std::complex<double>(0, -.5));
  sum += PauliOperator({{0, "Z"}, {1, "X"}}, .5);

  std::cout << "PRINT: " << op.toString() << "\n";
  std::cout << "SUM: " << sum.toString() << "\n";

  op *= sum;

  std::cout << "CURRENTMULT: " << op.toString() << "\n";

  EXPECT_EQ(0, op.nQubits());
}

TEST(PauliOperatorTester, toDenseMatrix) {
  PauliOperator op({{0, "X"}});
  auto data = op.toDenseMatrix(1);
  Eigen::MatrixXcd m = Eigen::Map<Eigen::MatrixXcd>(data.data(), 2, 2);
  std::cout << "HELLO: " << m << "\n";
}
TEST(PauliOperatorTester, checkConstruction) {

  PauliOperator inst({{4, "X"}, {3, "Z"}, {9, "Y"}, {1, "Z"}});

  std::cout << "PRINT: " << inst.toString() << "\n";

  PauliOperator i2({{4, "X"}, {3, "Z"}});
  auto sumInst = inst + i2;
  std::cout << "PRINT: " << sumInst.toString() << "\n";

  PauliOperator expected({{3, "Z"}, {4, "X"}});
  expected += PauliOperator({{1, "Z"}, {3, "Z"}, {4, "X"}, {9, "Y"}});
  std::cout << "EXPECTED: " << expected.toString() << "\n";
  EXPECT_TRUE(expected == sumInst);
  EXPECT_TRUE(sumInst == sumInst);

  PauliOperator i3({{3, "X"}});
  auto testPauliProducts = i2 * i3;
  std::cout << testPauliProducts.toString() << "\n";

  expected.clear();
  expected += PauliOperator({{3, "Y"}, {4, "X"}}, std::complex<double>(0, 1));
  //	expected.addTerm({{3,"Y"},{4,"X"}}, std::complex<double>(0,1));

  std::cout << "NEWEXPECTED: " << expected.toString() << "\n";
  EXPECT_TRUE(expected == testPauliProducts);

  PauliOperator testEqual({{4, "X"}, {3, "Z"}},
                          std::complex<double>(33.3, 0.0));
  EXPECT_TRUE(i2 == testEqual);
  EXPECT_TRUE(i2 != inst);

  // Term * scalar multiple
  auto multScalar = testPauliProducts * 3.3;
  EXPECT_TRUE("(0,3.3) Y3 X4" == multScalar.toString());
  EXPECT_TRUE(PauliOperator({{3, "Y"}, {4, "X"}},
                            std::complex<double>(0, 3.3)) == multScalar);

  testPauliProducts *= 3.3;
  EXPECT_TRUE(testPauliProducts == multScalar);

  // Plus with same terms
  sumInst = inst + 2.2 * inst;
  std::cout << inst.toString() << ", " << (2.2 * inst).toString() << ", "
            << sumInst.toString() << "\n";
  EXPECT_TRUE(PauliOperator({{1, "Z"}, {3, "Z"}, {4, "X"}, {9, "Y"}},
                            std::complex<double>(3.2, 0)) == sumInst);

  PauliOperator i4({{0, "Z"}});
  PauliOperator i6({{1, "Y"}}, std::complex<double>(0, 1));
  i6 += PauliOperator({{1, "X"}});
  PauliOperator i7({{1, "Z"}});

  expected.clear();
  expected += PauliOperator({{0, "Z"}, {1, "Y"}}, std::complex<double>(0, 1));
  expected += PauliOperator({{0, "Z"}, {1, "X"}});
  //	expected.addTerm({{0, "Z"}, {1, "Y"}}, std::complex<double>(0,1));
  //	expected.addTerm({{0, "Z"}, {1, "X"}});

  std::cout << i4.toString() << ", " << i6.toString() << "\nSTARTING:\n";
  auto newMultByComp = i4 * i6;
  std::cout << "ENDED:\nHI: " << newMultByComp.toString() << "\n";
  EXPECT_TRUE(expected == newMultByComp);

  std::cout << "MADE IT HERE\n";
  // Z1 * X1 + i * Z1 * Y1 = i * Y1 + i * (-i * X1)
  newMultByComp = i7 * i6;
  expected.clear();
  expected += PauliOperator({{1, "X"}}) +
              PauliOperator({{1, "Y"}}, std::complex<double>(0, 1));
  //	expected.addTerm({{1,"X"}}).addTerm({{1,"Y"}},
  //std::complex<double>(0,1));

  EXPECT_TRUE(expected == newMultByComp);
}

TEST(PauliOperatorTester, checkVariableCoefficient) {
  // make a4dag a3dag a9 a1
  PauliOperator inst(
      {
          {4, "X"},
          {3, "Z"},
      },
      "theta");
  auto three = 3 * inst;
  auto sum = ((inst + inst) + three);
  std::cout << "SUMSTR: " << sum.toString() << "\n";
  EXPECT_TRUE(sum.toString() == "(5,0) theta Z3 X4");

  PauliOperator i2({{4, "X"}, {3, "Z"}}, "theta2");

  EXPECT_TRUE(i2 != inst);
  auto sumInst = inst + i2;
  std::cout << "SUMSTR: " << sumInst.toString() << "\n";
  EXPECT_TRUE("(1,0) theta2 Z3 X4 + (1,0) theta Z3 X4" == sumInst.toString());
}

TEST(PauliOperatorTester, checkBinaryVector) {
  //	SpinInstruction inst( { { 4, "X" },
  //				{ 3, "Z" }, { 9, "Y" }, { 1, "Z" } });
  //
  //	// This is Z1 Z3 X4 Y9
  //
  //	auto bv = inst.toBinaryVector(10);
  //
  //	EXPECT_TRUE(bv.size() == 20);
  //
  //	std::vector<int> expected(20);
  //	expected[4] = 1;
  //	expected[9] = 1;
  //	expected[19] = 1;
  //	expected[13] = 1;
  //	expected[11] = 1;
  //
  //	EXPECT_TRUE(expected == bv);
  //
  //	SpinInstruction i;
  //	std::vector<int> zeros(20);
  //	EXPECT_TRUE(zeros == i.toBinaryVector(10));
  //
  //	i.fromBinaryVector(expected, std::complex<double>(1.0,0.0));
  //
  //	EXPECT_TRUE(inst.toString() == i.toString());
}

TEST(PauliOperatorTester, checkAction) {

  PauliOperator inst({{0, "X"}, {1, "Z"}, {2, "Y"}, {3, "Z"}},
                     std::complex<double>(2.2, 0));

  auto result = inst.computeActionOnKet("0110");

  EXPECT_TRUE(result[0].first == "1100");
  EXPECT_TRUE(result[0].second == std::complex<double>(0, 2.2));

  std::cout << "HELLO:\n"
            << result[0].first << ", " << result[0].second << "\n";

  PauliOperator inst2({{0, "X"}, {1, "Z"}, {2, "Y"}, {3, "Y"}},
                      std::complex<double>(2.2, 0));

  auto result2 = inst2.computeActionOnKet("0110");

  std::cout << "HELLO:\n"
            << result2[0].first << ", " << result2[0].second << "\n";

  EXPECT_TRUE(result2[0].first == "1101");
  EXPECT_TRUE(result2[0].second == std::complex<double>(-2.2, 0));
}

TEST(PauliOperatorTester, checkComplexTerms) {
  //	SpinInstruction inst({{1,"X"}, {3,"Y"}, {8,"Z"}},
  //std::complex<double>(0.5,0.0)), 			inst2({{1,"Z"}, {3,"X"}, {8,"Z"}},
  //std::complex<double>(1.2,0.0)), 			inst3({{1,"Z"}, {3,"Y"}, {9,"Z"}},
  //std::complex<double>(0.0,1.4)), 			c({}, std::complex<double>(.25+1.2*1.2, 0.0)
  //					+
  //std::complex<double>(0,1.4)*std::complex<double>(0,1.4)),
  //			c2({{1,"Y"},{3,"Z"}},
  //std::complex<double>(0,2)*std::complex<double>(0,1)*std::complex<double>(.5,0)*std::complex<double>(1.2,0));
  //
  //	auto op = inst+inst2+inst3;
  //
  //	auto res = op*op;
  //	std::cout << "RES: " << res.toString("") << "\n";
  //	res.simplify();
  //	std::cout << "Simplified: " << res.toString("") << "\n";
  //
  //	auto t = c + c2;
  //	t.simplify();
  //	std::cout << "TSimp: " << t.toString("") << "\n";
  //
  //	EXPECT_TRUE(t == res);
}

TEST(PauliOperatorTester, checkHelpMe) {

  PauliOperator sum1({{0, "X"}}, std::complex<double>(.5, 0));
  PauliOperator sum2({{0, "Y"}}, std::complex<double>(0, 0.5));
  PauliOperator current1({{0, "X"}}, std::complex<double>(-.62215, 0));
  PauliOperator current2({{0, "Y"}}, std::complex<double>(0, 0.62215));

  PauliOperator sum, current;
  sum += sum1 + sum2;
  current += current1 + current2;

  //	UM: (0.5,0) * X0 + (0,0.5) * Y0
  //	Current: (-0.62215,0) * X0 + (0,0.62215) * Y0

  std::cout << "SUM: " << sum.toString() << "\n";
  std::cout << "Current: " << current.toString() << "\n";
  auto x = current * sum;

  std::cout << "RESULT: " << x.toString() << "\n";

  std::cout << (std::complex<double>(1, 0) * std::complex<double>(0, 1))
            << "\n";
}

TEST(PauliOperatorTester, checkIdentity) {
  PauliOperator i1(std::complex<double>(3.3, 0));
  PauliOperator i2(std::complex<double>(5.3, 0));
  PauliOperator compInst;
  compInst += i1;

  compInst = compInst + i2;

  std::cout << "HI: " << compInst.toString() << "\n";
  EXPECT_TRUE("(8.6,0)" == compInst.toString());
  EXPECT_TRUE(PauliOperator(std::complex<double>(8.6, 0)) == compInst);
}

TEST(PauliOperatorTester, checkComposition) {
  PauliOperator i1({{0, "Z"}});
  PauliOperator i2({{1, "X"}});
  PauliOperator i3({{1, "Y"}}, std::complex<double>(0, 1));
  PauliOperator i4({{1, "Z"}});
  PauliOperator i5({{1, "Y"}}, std::complex<double>(0, -1));

  PauliOperator compInst, compInst2, compInst3;
  compInst += i2 + i3;

  compInst3 += i2 + i1;

  compInst2 += i2;

  std::complex<double> i(0, 1);
  EXPECT_TRUE(compInst == compInst);
  EXPECT_TRUE(compInst != compInst2);
  EXPECT_TRUE(compInst2 == i2);
  EXPECT_TRUE(compInst2 != i1);

  auto multBySpinInst = compInst * i4;
  PauliOperator expected({{1, "X"}}, -1);
  expected += PauliOperator({{1, "Y"}}, -i);
  EXPECT_TRUE(expected == multBySpinInst);

  multBySpinInst = compInst * i1;
  std::cout << "HOWDY: " << multBySpinInst.toString() << "\n";
  expected.clear();
  expected += PauliOperator({{0, "Z"}, {1, "Y"}}, i);
  expected += PauliOperator({{0, "Z"}, {1, "X"}});
  EXPECT_TRUE(expected == multBySpinInst);

  auto multByComposite = compInst * compInst;
  std::cout << "ZERO: " << multByComposite.toString() << "\n";
  EXPECT_TRUE(PauliOperator() == multByComposite);

  auto t = compInst3 * compInst3;
  expected.clear();
  expected += PauliOperator({{0, "Z"}, {1, "X"}}, 2);
  expected += PauliOperator(2);
  EXPECT_TRUE(expected == t);

  auto test = compInst * 4.4;
  expected.clear();
  expected += PauliOperator({{1, "X"}}, 4.4);
  expected += PauliOperator({{1, "Y"}}, 4.4 * i);
  EXPECT_TRUE(expected == test);

  EXPECT_TRUE(expected == (4.4 * compInst));

  test = compInst * std::complex<double>(4.4, 0);
  EXPECT_TRUE(expected == test);
  EXPECT_TRUE(expected == (std::complex<double>(4.4, 0) * compInst));

  // Test Addition now

  auto added = compInst + compInst3;
  expected.clear();
  expected += PauliOperator({{0, "Z"}});
  expected += PauliOperator({{1, "X"}}, 2);
  expected += PauliOperator({{1, "Y"}}, i);

  EXPECT_TRUE(expected == added);
}

TEST(PauliOperatorTester, checkMatrixElements) {
  PauliOperator op({{0, "X"}, {1, "Y"}, {2, "Z"}});
  auto elements = op.getSparseMatrixElements();
}

TEST(PauliOperatorTester, checkFromXACCIR) {

  using namespace xacc;

  auto gateRegistry = xacc::getService<IRProvider>("quantum");
  auto f = gateRegistry->createComposite("f");
  auto f2 = gateRegistry->createComposite("f2");

  auto h1 = gateRegistry->createInstruction("H", 0);
  auto h2 = gateRegistry->createInstruction("H", 1);
  auto m1 = gateRegistry->createInstruction("Measure", 0);
  // InstructionParameter p(0), p1(1), p2(2);
  // m1->setParameter(0, p);
  auto m2 = gateRegistry->createInstruction("Measure", 1);
  // m2->setParameter(0,p1);

  auto m3 = gateRegistry->createInstruction("Measure", 2);
  // m3->setParameter(0,p2);

  f->addInstruction(h1);
  f->addInstruction(h2);
  f->addInstruction(m1);
  f->addInstruction(m2);
  f->addInstruction(m3);

  auto h3 = gateRegistry->createInstruction("H", 0);
  auto rx = gateRegistry->createInstruction("Rx", 1);
  InstructionParameter q(xacc::constants::pi / 2.0);
  rx->setParameter(0, q);

  auto m4 = gateRegistry->createInstruction("Measure", 0);
  auto m5 = gateRegistry->createInstruction("Measure", 1);
  auto m6 = gateRegistry->createInstruction("Measure", 2);

  f2->addInstruction(h3);
  f2->addInstruction(rx);
  f2->addInstruction(m4);
  f2->addInstruction(m5);
  f2->addInstruction(m6);

  auto ir = gateRegistry->createIR();
  ir->addComposite(f);
  ir->addComposite(f2);

  PauliOperator op;

  op.fromXACCIR(ir);

  std::cout << "HEY: " << op.toString() << "\n";
}

TEST(PauliOperatorTester, checkFromXACCIR2) {

  PauliOperator op;
  op.fromString("I + Z0 Z1 + X0 X1 + Y0 Y1 + Z0 + Z1");

  std::cout << "OP ON OUR SIDE IS " << op.toString() << "\n";
  auto measureFunctions = op.toXACCIR()->getComposites();

  auto ir = xacc::getService<xacc::IRProvider>("quantum")->createIR();
  for (auto &f : measureFunctions) {
    ir->addComposite(f);
  }
  PauliOperator H;
  H.fromXACCIR(ir);
  std::cout << "H is\n" << H.toString() << "\n";

  EXPECT_TRUE(H == op);
}
TEST(PauliOperatorTester, checkFromStr) {
  const std::string s = "(-.00944179,0) Z1 X2 X3 + 0.0816923 Z2 Z3";

  PauliOperator op;
  op.fromString(s);

  std::cout << op.toString() << "\n";
  PauliOperator expected({{1, "Z"}, {2, "X"}, {3, "X"}}, -0.00944179);
  expected += PauliOperator({{2, "Z"}, {3, "Z"}}, 0.0816923);

  EXPECT_TRUE(op == expected);

  auto s2 = "X0X1";
  PauliOperator op2("X0X1");
  //    op2.fromString(s2);
  std::cout << "Op2: " << op2.toString() << "\n";

  PauliOperator op3("5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1");
  std::cout << "Op3: " << op3.toString() << "\n";
}

TEST(PauliOperatorTester, checkContainsTerm) {
  const std::string s = "(-.00944179,0) Z1 X2 X3 + (0.0816923,0) Z2 Z3";
  PauliOperator op;
  op.fromString(s);

  PauliOperator subOp({{1, "Z"}, {2, "X"}, {3, "X"}}, -0.00944179);
  PauliOperator subOp2({{2, "X"}, {3, "X"}}, -0.00944179);

  EXPECT_TRUE(op.contains(subOp));
  EXPECT_FALSE(op.contains(subOp2));
}

TEST(PauliOperatorTester, checkCommutes) {
  PauliOperator op;
  op.fromString("I + Z0 + Z1 + X0 X1 + Y0 Y1 + Z0 Z1");

  PauliOperator zz({{0, "Z"}, {1, "Z"}});
  PauliOperator y({{0, "Y"}});

  EXPECT_TRUE(op.commutes(zz));
  EXPECT_FALSE(PauliOperator({{0, "X"}}).commutes(y));
}

TEST(PauliOperatorTester, checkSciNot) {
  PauliOperator op;
  op.fromString("(1.234e-4, 0) Z0");
  std::cout << op.toString() << "\n";

  op.fromString("1.234e-4 Z0");
  std::cout << op.toString() << "\n";

  op.fromString("5e-11 Z0");
  std::cout << (1.0e11 * op).toString() << "\n";

}


TEST(PauliOperatorTester, checkNormalize) {
  PauliOperator op;
  op.fromString("(0, -1) Z0 + (0, 2) X0");
  std::cout << op.toString() << "\n";

  op.normalize();
  std::cout << op.toString() << "\n";

}

TEST(PauliOperatorTester, checkGroupingCommuteCheck) {
  PauliOperator op0;
  op0.fromString("(0, -1) Z0 Z1 + (0, -1) Z1 Z2 + (0, -1) Z2 Z0");
  std::cout << op0.toString() << "\n";

  PauliOperator op1;
  op1.fromString("(0, -1) Z0 + (0, -1) Z1 + (0, -1) X2");
  std::cout << op1.toString() << "\n";

  PauliOperator op2;
  op2.fromString("(0, -1) Z0 Z1 + (0, -1) Z1 Z2 + (0, -1) X3 Y4");
  std::cout << op2.toString() << "\n";

  PauliOperator op3;
  op3.fromString("(0, -1) Z0 X1 + (0, -1) X1 Y2 + (0, -1) Y2 Z3");
  std::cout << op3.toString() << "\n";

  // No optimization allowed here:
  PauliOperator op4_no_opt_1;
  op4_no_opt_1.fromString(
      "(0, -1) Z0 X1 + (0, -1) X1 Y2 + (0, -1) Y2 Z3 + (0, -1) X0");
  std::cout << op4_no_opt_1.toString() << "\n";

  PauliOperator op4_no_opt_2;
  op4_no_opt_2.fromString("(0, -1) Z0 Z1 + (0, -1) Z1 Z2 + (0, -1) Z2 X0");
  std::cout << op4_no_opt_2.toString() << "\n";

  std::vector<PauliOperator> opt_cases{op0, op1, op2, op3};
  for (auto &test_case : opt_cases) {
    auto qpu = xacc::getAccelerator("qpp", {{"shots", 1024}});
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    auto f = gateRegistry->createComposite("f");
    auto h0 = gateRegistry->createInstruction("H", 0);
    auto h1 = gateRegistry->createInstruction("H", 1);
    auto h2 = gateRegistry->createInstruction("H", 2);
    f->addInstructions({h0, h1, h2});
    auto observed = test_case.observe(f, {{"accelerator", qpu}});
    EXPECT_EQ(observed.size(), 1);
  }

  // Check non-commute as well:
  std::vector<PauliOperator> no_opt_cases{op4_no_opt_1, op4_no_opt_2};
  for (auto &test_case : no_opt_cases) {
    auto qpu = xacc::getAccelerator("qpp", {{"shots", 1024}});
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    auto f = gateRegistry->createComposite("f");
    auto h0 = gateRegistry->createInstruction("H", 0);
    auto h1 = gateRegistry->createInstruction("H", 1);
    auto h2 = gateRegistry->createInstruction("H", 2);
    f->addInstructions({h0, h1, h2});
    auto observed = test_case.observe(f, {{"accelerator", qpu}});
    EXPECT_EQ(observed.size(), test_case.getNonIdentitySubTerms().size());
  }
}

TEST(PauliOperatorTester, checkGroupingQaoaPostProcessLSB) {
  PauliOperator op;
  op.fromString("(1.5,0) + (-0.5,0) Z0 Z1 + (-0.5,0) Z0 Z2 + (-0.5,0) Z1 Z2");
  std::cout << op.toString() << "\n";
  xacc::Observable *obs = &op;
  auto qpu = xacc::getAccelerator("qpp", {{"shots", 1024}});
  auto qaoa_ansatz =
      xacc::createComposite("qaoa", {{"nbQubits", 3},
                                     {"nbSteps", 1},
                                     {"cost-ham", obs},
                                     {"parameter-scheme", "Standard"}});
  const std::vector<double> opt_params{0.308, 0.614205};
  auto f = (*qaoa_ansatz)(opt_params);
  auto observed = op.observe(f, {{"accelerator", qpu}});
  EXPECT_EQ(observed.size(), 1);
  auto buffer = xacc::qalloc(3);
  qpu->execute(buffer, observed);
  buffer->print();
  auto exp_val = op.postProcess(
      buffer, xacc::Observable::PostProcessingTask::EXP_VAL_CALC, {});
  EXPECT_NEAR(exp_val, 2.0, 0.1);
}

TEST(PauliOperatorTester, checkGroupingQaoaPostProcessMSB) {
  PauliOperator op;
  op.fromString("(1.5,0) + (-0.5,0) Z0 Z1 + (-0.5,0) Z0 Z2 + (-0.5,0) Z1 Z2");
  std::cout << op.toString() << "\n";
  xacc::Observable *obs = &op;
  // Aer => MSB
  auto qpu = xacc::getAccelerator("aer", {{"shots", 1024}});
  auto qaoa_ansatz =
      xacc::createComposite("qaoa", {{"nbQubits", 3},
                                     {"nbSteps", 1},
                                     {"cost-ham", obs},
                                     {"parameter-scheme", "Standard"}});
  const std::vector<double> opt_params{0.308, 0.614205};
  auto f = (*qaoa_ansatz)(opt_params);
  auto observed = op.observe(f, {{"accelerator", qpu}});
  EXPECT_EQ(observed.size(), 1);
  auto buffer = xacc::qalloc(3);
  qpu->execute(buffer, observed);
  buffer->print();
  auto exp_val = op.postProcess(
      buffer, xacc::Observable::PostProcessingTask::EXP_VAL_CALC, {});
  EXPECT_NEAR(exp_val, 2.0, 0.1);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
