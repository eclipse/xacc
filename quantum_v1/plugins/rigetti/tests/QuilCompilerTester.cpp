/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#include <gtest/gtest.h>
#include "XACC.hpp"
#include "QuilVisitor.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "xacc_service.hpp"
using namespace xacc;

using namespace xacc::quantum;

TEST(QuilCompilerTester, checkTeleportQuil) {

  const std::string src("__qpu__ teleport (AcceleratorBuffer qreg) {\n"
                        "   # Prepare a bell state\n"
                        "   X 0\n"
                        "   H 1\n"
                        "   CNOT 1 2\n"
                        "   # Teleport qubit 0 to qubit 2\n"
                        "   CNOT 0 1\n"
                        "   H 0\n"
                        "   MEASURE 0 [0]\n"
                        "   MEASURE 1 [1]\n"
                        "   JUMP-UNLESS @NOX [1]\n"
                        "   X 2\n"
                        "   LABEL @NOX\n"
                        "   JUMP-UNLESS @NOZ [0]\n"
                        "   Z 2\n"
                        "   LABEL @NOZ\n"
                        "}\n");

  auto compiler = xacc::getService<Compiler>("quil");
  auto ir = compiler->compile(src);

  auto function = ir->getKernel("teleport");
  std::cout << "HELLO\n" << function->toString("qreg") << "\n";

  EXPECT_TRUE(ir->getKernels().size() == 1);

  EXPECT_TRUE(function->nInstructions() == 9);

  auto hadamardVisitor =
      std::make_shared<CountGatesOfTypeVisitor<Hadamard>>(function);
  auto cnotVisitor = std::make_shared<CountGatesOfTypeVisitor<CNOT>>(function);
  auto measureVisitor =
      std::make_shared<CountGatesOfTypeVisitor<Measure>>(function);
  // auto conditionalVisitor =
  // std::make_shared<CountGatesOfTypeVisitor<ConditionalFunction>>(function);
  auto xVisitor = std::make_shared<CountGatesOfTypeVisitor<X>>(function);

  EXPECT_TRUE(hadamardVisitor->countGates() == 2);
  EXPECT_TRUE(cnotVisitor->countGates() == 2);
  EXPECT_TRUE(measureVisitor->countGates() == 2);
  // EXPECT_TRUE(conditionalVisitor->countGates() == 2);
  EXPECT_TRUE(xVisitor->countGates() == 2);
}

// Note, conditionals are not currently supported by the compiler so this does
// not actually check them
TEST(QuilCompilerTester, checkIFELSE) {
  const std::string src =
      R"src(__qpu__ teleport(AcceleratorBuffer qreg) {
X 0
H 2
CNOT 2 1
CNOT 0 1
H 0
MEASURE 0 [0]
MEASURE 1 [1]
JUMP-WHEN @THEN1 [1]
JUMP @END2
LABEL @THEN1
X 2
LABEL @END2
JUMP-WHEN @THEN3 [0]
JUMP @END4
LABEL @THEN3
Z 2
LABEL @END4
MEASURE 2 [2]
})src";
  auto compiler = xacc::getService<Compiler>("quil");

  auto ir = compiler->compile(src);

  auto function = ir->getKernel("teleport");

  std::cout << "HELLO\n" << function->toString("qreg") << "\n";
  std::cout << "N: " << function->nInstructions() << "\n";

  EXPECT_TRUE(ir->getKernels().size() == 1);

  EXPECT_TRUE(function->nInstructions() == 10);

  auto hadamardVisitor =
      std::make_shared<CountGatesOfTypeVisitor<Hadamard>>(function);
  auto cnotVisitor = std::make_shared<CountGatesOfTypeVisitor<CNOT>>(function);
  auto measureVisitor =
      std::make_shared<CountGatesOfTypeVisitor<Measure>>(function);
  // auto conditionalVisitor =
  // std::make_shared<CountGatesOfTypeVisitor<ConditionalFunction>>(function);
  auto xVisitor = std::make_shared<CountGatesOfTypeVisitor<X>>(function);

  EXPECT_TRUE(hadamardVisitor->countGates() == 2);
  EXPECT_TRUE(cnotVisitor->countGates() == 2);
  EXPECT_TRUE(measureVisitor->countGates() == 3);
  // EXPECT_TRUE(conditionalVisitor->countGates() == 2);
  EXPECT_TRUE(xVisitor->countGates() == 2);
}

TEST(QuilCompilerTester, checkRotations) {
  const std::string src =
      R"src(__qpu__ rotate(AcceleratorBuffer qreg) {
RX(3.141592653589793) 0
})src";
  auto compiler = xacc::getService<Compiler>("quil");

  auto ir = compiler->compile(src);

  auto function = ir->getKernel("rotate");

  std::cout << "HELLO\n" << function->toString("qreg") << "\n";

  EXPECT_TRUE(ir->getKernels().size() == 1);

  EXPECT_TRUE(function->nInstructions() == 1);

  auto rxVisitor = std::make_shared<CountGatesOfTypeVisitor<Rx>>(function);

  EXPECT_TRUE(rxVisitor->countGates() == 1);
}

TEST(QuilCompilerTester, checkTranslateIR) {
  auto compiler = xacc::getService<Compiler>("quil");

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

  auto rz = std::make_shared<Rz>(0, 3.1415);
  auto swap = std::make_shared<Swap>(0, 1);
  auto cphase = std::make_shared<CPhase>(1, 2, 3.1415926);

  InstructionParameter p("theta");
  auto varCphase = std::make_shared<CPhase>(std::vector<int>{1, 2});
  varCphase->setParameter(0, p);

  f->addInstruction(x);
  f->addInstruction(h);
  f->addInstruction(cn1);
  f->addInstruction(cn2);
  f->addInstruction(h2);
  f->addInstruction(m0);
  f->addInstruction(m1);
  f->addInstruction(cond1);
  f->addInstruction(cond2);

  std::string src = compiler->translate("", f);

  const std::string expected("X 0\n"
                             "H 1\n"
                             "CNOT 1 2\n"
                             "CNOT 0 1\n"
                             "H 0\n"
                             "MEASURE 0 [0]\n"
                             "MEASURE 1 [1]\n"
                             "JUMP-UNLESS @conditional_0 [0]\n"
                             "Z 2\n"
                             "LABEL @conditional_0\n"
                             "JUMP-UNLESS @conditional_1 [1]\n"
                             "X 2\n"
                             "LABEL @conditional_1\n");

  std::cout << "SRC:\n" << src << "\n";
//   EXPECT_TRUE(expected == src);
}

TEST(QuilCompilerTester, checkMultipleKernels) {

  const std::string src = R"src(__qpu__ term0(AcceleratorBuffer qreg) {
}
__qpu__ term1(AcceleratorBuffer qreg) {
H 0
MEASURE 0 [0]
}
__qpu__ term2(AcceleratorBuffer qreg) {
MEASURE 0 [0]
})src";

  auto compiler = xacc::getService<Compiler>("quil");

  auto ir = compiler->compile(src);

  EXPECT_TRUE(ir->getKernels().size() == 3);

  std::cout << "TEST:\n" << ir->getKernels()[0]->toString("qreg") << "\n\n";
  std::cout << "TEST:\n" << ir->getKernels()[1]->toString("qreg") << "\n\n";
  std::cout << "TEST:\n" << ir->getKernels()[2]->toString("qreg") << "\n\n";
}

TEST(QuilCompilerTester, checkVariableParameter) {

  const std::string src =
      R"src(__qpu__ statePrep2x2(AcceleratorBuffer qreg, double theta1) {
RY(theta1) 0
})src";

  auto compiler = xacc::getService<Compiler>("quil");

  auto ir = compiler->compile(src);
  std::cout << "TEST:\n" << ir->getKernels()[0]->toString("qreg") << "\n\n";

  EXPECT_TRUE(ir->getKernels()[0]->nParameters() == 1);

  const std::string src2 =
      R"src(__qpu__ statePrep2x2(AcceleratorBuffer qreg, double theta1) {
RY(theta1) 0
})src";

  ir = compiler->compile(src2);
  std::cout << "TEST:\n" << ir->getKernels()[0]->toString("qreg") << "\n\n";

  EXPECT_TRUE(ir->getKernels()[0]->nParameters() == 1);
  std::vector<double> v{2.2};
  auto evaled = ir->getKernels()[0]->operator()(v); //({2.2});
  std::cout << "TEST:\n" << evaled->toString("qreg") << "\n\n";
}
int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
