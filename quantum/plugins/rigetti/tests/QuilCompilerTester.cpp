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
#include "xacc.hpp"
#include "QuilVisitor.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "xacc_service.hpp"
#include "qalloc"

using namespace xacc;

using namespace xacc::quantum;

TEST(QuilCompileTester, checkRuntimeArgs) {

  const std::string src =
      R"src(__qpu__ void test(qreg q, double t) {
RY(t) 0
})src";

  auto compiler = xacc::getService<Compiler>("quil");

  auto ir = compiler->compile(src);
  std::cout << "TEST:\n" << ir->getComposites()[0]->toString() << "\n\n";

  auto f = ir->getComposites()[0];
  xacc::internal_compiler::qreg q(1);

  for (auto val : {2.2, 3.3, 4.5}) {
    f->updateRuntimeArguments(q, val);
    std::cout<< "howdy:\n" << f->toString() <<"\n";
  }
}

TEST(QuilCompilerTester, checkSimple) {
  const std::string src =
      R"src(__qpu__ void rotate(qbit qreg) {
RX(3.141592653589793) 0
H 0
CX 1 0
MEASURE 0 [0]
})src";
  auto compiler = xacc::getService<Compiler>("quil");

  auto ir = compiler->compile(src);

  auto function = ir->getComposite("rotate");

  std::cout << "HELLO\n" << function->toString() << "\n";

  EXPECT_TRUE(ir->getComposites().size() == 1);

  EXPECT_TRUE(function->nInstructions() == 4);
}

TEST(QuilCompilerTester, checkXY) {
  const std::string src =
      R"src(__qpu__ void rotate(qbit qreg) {
RX(3.141592653589793) 0
H 0
XY(pi) 1 0
MEASURE 0 [0]
})src";
  auto compiler = xacc::getService<Compiler>("quil");

  auto ir = compiler->compile(src);

  auto function = ir->getComposite("rotate");

  std::cout << "HELLO\n" << function->toString() << "\n";

  EXPECT_TRUE(ir->getComposites().size() == 1);

  EXPECT_TRUE(function->nInstructions() == 4);
}

TEST(QuilCompilerTester, checkVariableParameter) {

  const std::string src =
      R"src(__qpu__ void statePrep2x2(qbit qreg, double theta1) {
RY(theta1) 0
})src";

  auto compiler = xacc::getService<Compiler>("quil");

  auto ir = compiler->compile(src);
  std::cout << "TEST:\n" << ir->getComposites()[0]->toString() << "\n\n";

  std::vector<double> v{2.2};
  auto evaled = ir->getComposites()[0]->operator()(v); //({2.2});
  std::cout << "TEST:\n" << evaled->toString() << "\n\n";
}
int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
