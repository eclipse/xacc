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
#include "gtest/gtest.h"

#include "xacc.hpp"
#include "xacc_service.hpp"
#include <regex>
#include <random>

namespace {
  // CU1 decompose: 5 instructions
  constexpr int CU1_DECOMP_N_INSTS = 5;
}

TEST(StaqCompilerTester, checkCphase) {

  auto src = R"#(
qreg q[4];
x q[3];
h q[0];
h q[1];
h q[2];
cu1(0.785398) q[0], q[3];
creg c[3];
measure q[0] -> c[0];
measure q[1] -> c[1];
measure q[2] -> c[2];)#";

  auto compiler = xacc::getCompiler("staq");
  auto IR = compiler->compile(src);

  auto hello = IR->getComposites()[0];
  std::cout << "HELLO:\n" << hello->toString() << "\n";
  const int expectedNinsts = 7 + CU1_DECOMP_N_INSTS;
  EXPECT_EQ(hello->nInstructions(), expectedNinsts);
}

TEST(StaqCompilerTester, checkSimple) {
  auto compiler = xacc::getCompiler("staq");
  auto IR = compiler->compile(R"(
                      qreg q[2];
                      creg c[2];
                      U(0,0,0) q[0];
                      CX q[0],q[1];
                      rx(3.3) q[0];
                      measure q -> c;
                      )");

  auto hello = IR->getComposites()[0];
  std::cout << "HELLO:\n" << hello->toString() << "\n";
  EXPECT_EQ(hello->nInstructions(), 5);
  
  auto q = xacc::qalloc(2);
  q->setName("q");
  xacc::storeBuffer(q);

  IR = compiler->compile(R"(__qpu__ void f(qreg q) {
                    OPENQASM 2.0;
                    include "qelib1.inc";
                    creg c[2];
                    U(0,0,0) q[0];
                    CX q[0],q[1];
                    rx(3.3) q[0];
                    measure q -> c;
                    })");

  hello = IR->getComposites()[0];
  std::cout << "HELLO:\n" << hello->toString() << "\n";
  EXPECT_EQ(hello->nInstructions(), 5);
}

TEST(StaqCompilerTester, checkCanParse) {
  auto a = xacc::qalloc(2);
  a->setName("qq");
  xacc::storeBuffer(a);

  auto compiler = xacc::getCompiler("staq");
  EXPECT_FALSE(compiler->canParse(
      R"(__qpu__ void bell_test_can_parse(qbit qq, double t0) {
  H(qq[0]);
  CX(qq[0], qq[1]);
  Ry(qq[0], t0);
  Measure(qq[0]);
  Measure(qq[1]);
})"));

  EXPECT_FALSE(compiler->canParse(
      R"(__qpu__ void bell_test_can_parse(qbit qq, double t0) {
  H(qq[0]);
  CX(qq[0], [1]);
  Ry(qq[0], t0);
  Measure(qq[0]);
  Measure(qq[1]);
})"));

  EXPECT_TRUE(compiler->canParse(R"(
                      qreg qq[2];
                      creg c[2];
                      U(0,0,0) qq[0];
                      CX qq[0],qq[1];
                      rx(3.3) qq[0];
                      measure qq -> c;
                      )"));
}

TEST(StaqCompilerTester, checkTranslate) {
  auto compiler = xacc::getCompiler("staq");
  auto IR = compiler->compile(R"(OPENQASM 2.0;
                      include "qelib1.inc";
                      qreg q[2];
                      creg c[2];
                      U(0,0,0) q[0];
                      CX q[0],q[1];
                      rx(3.3) q[0];
                      measure q -> c;
                      )");

  auto hello = IR->getComposites()[0];
  std::cout << "HELLO:\n" << hello->toString() << "\n";

  std::cout << "TRANSLATED: " << compiler->translate(hello) << "\n";
  //   auto q = xacc::qalloc(2);
  //   q->setName("q");
  //   xacc::storeBuffer(q);

  //   IR = compiler->compile(R"(__qpu__ void f(qreg q) {
  //                     OPENQASM 2.0;
  //                     include "qelib1.inc";
  //                     creg c[2];
  //                     U(0,0,0) q[0];
  //                     CX q[0],q[1];
  //                     rx(3.3) q[0];
  //                     measure q -> c;
  //                     })");

  //   hello = IR->getComposites()[0];
  //   std::cout << "HELLO:\n" << hello->toString() << "\n";
}

TEST(StaqCompilerTester, checkOracle) {
  auto a = xacc::qalloc(4);
  a->setName("a");
  xacc::storeBuffer(a);
  auto b = xacc::qalloc(4);
  b->setName("b");
  xacc::storeBuffer(b);
  auto c = xacc::qalloc(4);
  c->setName("c");
  xacc::storeBuffer(c);
  auto src = R"(__qpu__ void add(qreg a, qreg b, qreg c) {
OPENQASM 2.0;
include "qelib1.inc";

oracle adder a0,a1,a2,a3,b0,b1,b2,b3,c0,c1,c2,c3 { "@CMAKE_SOURCE_DIR@/quantum/plugins/staq/compiler/tests/adder_4.v" }

creg result[4];

// a = 3
x a[0];
x a[1];

// b = 5
x b[0];
x b[2];

adder a[0],a[1],a[2],a[3],b[0],b[1],b[2],b[3],c[0],c[1],c[2],c[3];

// measure
measure c -> result;
})";
  auto compiler = xacc::getCompiler("staq");
  auto IR = compiler->compile(src);

  auto hello = IR->getComposites()[0];
  std::cout << hello->toString() << "\n";

  auto src2 = R"(OPENQASM 2.0;
include "qelib1.inc";

oracle adder a0,a1,a2,a3,b0,b1,b2,b3,c0,c1,c2,c3 { "adder_4.v" }

qreg a[4];
qreg b[4];
qreg c[4];
creg result[4];

// a = 3
x a[0];
x a[1];

// b = 5
x b[0];
x b[2];

adder a[0],a[1],a[2],a[3],b[0],b[1],b[2],b[3],c[0],c[1],c[2],c[3];

// measure
measure c -> result;
)";
  //    IR = compiler->compile(src2);

  //    hello = IR->getComposites()[0];
  //   std::cout << hello->toString() << "\n";
}

TEST(StaqCompilerTester, checkCirq) {
  auto compiler = xacc::getCompiler("staq");
  auto IR = compiler->compile(R"(
                      qreg q[2];
                      creg c[2];
                      U(0,0,0) q[0];
                      CX q[0],q[1];
                      rx(3.3) q[0];
                      measure q -> c;
                      )");

  auto hello = IR->getComposites()[0];
  std::cout << "HELLO:\n" << hello->toString() << "\n";

  auto c = compiler->translate(hello, {std::make_pair("lang-type", "cirq")});
  std::cout << "CIRQ: " << c << "\n";
}

TEST(StaqCompilerTester, checkFloatingPointFormat) {
  auto compiler = xacc::getCompiler("staq");
  // Checks that floating point params don't get
  // converted to scientific form during translation.
  auto IR = compiler->compile(R"(OPENQASM 2.0;
                      include "qelib1.inc";
                      qreg q[2];
                      creg c[2];
                      rx(0.00000025) q[0];
                      measure q -> c;
                      )");
  auto hello = IR->getComposites()[0];
  auto rxInst = hello->getInstruction(0);
  // We must not lose any precision.
  EXPECT_NEAR(rxInst->getParameter(0).as<double>(), 2.5e-07, 1e-12);
}

TEST(StaqCompilerTester, checkIfStatementTranslate) {
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void test(qbit q) {
    Measure(q[0], c[0]);
    if(c[0]) {
      Rz(q[0], -pi/2);
    }
    H(q[0]);
    Measure(q[0], c[1]);
    CPhase(q[0], q[1], -5*pi/8);
    if(c[0]) {
        Rz(q[0], -pi/4);
    }
    if(c[1]) {
        Rz(q[0], -pi/2);
    }
  })", nullptr);
  auto staq_compiler = xacc::getCompiler("staq");
  auto qasm = staq_compiler->translate(ir->getComposites()[0]);
  std::cout << "HOWDY:\n" << qasm << "\n";
  // Check that If statements are translated.
  EXPECT_TRUE(qasm.find("if (c[0] == 1) rz") != std::string::npos);
  EXPECT_TRUE(qasm.find("if (c[1] == 1) rz") != std::string::npos);
}

TEST(StaqCompilerTester, checkCustomInclude) {
  auto src = R"#(
    __qpu__ void QBCIRCUIT(qreg q) {
    OPENQASM 2.0;
    include "@CMAKE_SOURCE_DIR@/quantum/plugins/staq/compiler/tests/qelib1.inc"; 
    creg c0[1];
    creg c1[1];
    creg c2[1];
    h q[2];
    h q[1];
    h q[0];
    measure q[0] -> c0[0];
    measure q[1] -> c1[0];
    measure q[2] -> c2[0];
    }
)#";
  auto buffer = xacc::qalloc(3);
  buffer->setName("q");
  xacc::storeBuffer(buffer);

  auto compiler = xacc::getCompiler("staq");
  auto IR = compiler->compile(src);
  auto hello = IR->getComposites()[0];
  std::cout << hello->toString() << "\n";
  EXPECT_EQ(hello->nInstructions(), 6);
}

TEST(StaqCompilerTester, checkCRy) {
  auto q = xacc::qalloc(2);
  q->setName("q");
  xacc::storeBuffer(q);

  auto src = R"(__qpu__ void test(qreg q) {
OPENQASM 2.0;
include "qelib1.inc";
cry(1.2345) q[0], q[1];
})";
  auto staq = xacc::getCompiler("staq");
  auto IR = staq->compile(src);
  auto hello = IR->getComposites()[0];
  std::cout << hello->toString() << "\n";
  EXPECT_EQ(hello->nInstructions(), 4);
}

TEST(StaqCompilerTester, checkCtrlRotationDef) {
  // Functional test to validate CRx, CRy, CRz decomposition
  // in staq compiler.
  const std::vector<std::string> ROTATION_GATE{"rx", "ry", "rz"};
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<> distr(0.0, 2.0 * M_PI);
  const double randomAngle = distr(eng);

  for (const auto &rotation : ROTATION_GATE) {
    // Ctrl rotation (control bit set) then reverse the rotation (no control)
    std::string src = R"(OPENQASM 2.0;
include "qelib1.inc";
qreg q[2];
creg c[2];
x q[0];
cGATE_NAME(ANGLE) q[0], q[1];
GATE_NAME(-ANGLE) q[1];
measure q -> c;
)";
    src = std::regex_replace(src, std::regex("GATE_NAME"), rotation);
    src = std::regex_replace(src, std::regex("ANGLE"),
                             std::to_string(randomAngle));
    std::cout << "HOWDY:\n" << src << "\n";
    auto compiler = xacc::getCompiler("staq");
    auto IR = compiler->compile(src);
    auto hello = IR->getComposites()[0];
    std::cout << hello->toString() << "\n";
    auto accelerator = xacc::getAccelerator("qpp", {{"shots", 1024}});
    auto buffer = xacc::qalloc(2);
    accelerator->execute(buffer, hello);
    buffer->print();
    // Control bit is set to 1; target bit must return to 0.
    EXPECT_EQ(buffer->getMeasurementCounts()["10"], 1024);
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  xacc::set_verbose(true);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
