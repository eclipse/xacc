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
#include "xacc_service.hpp"
#include "qalloc.hpp"

using namespace xacc;

TEST(QuilcTester, checkSimple) {
  auto quilc = xacc::getCompiler("quilc");

  const std::string src = R"##(H 0
CNOT 0 1
)##";

  if (quilc->canParse(src)) {
    auto ir = quilc->compile(src);
    auto k = ir->getComposites()[0];

    std::cout << "K:\n" << k->toString() << "\n";
  }
}
TEST(QuilcTester, checkParameterized) {
  auto quilc = xacc::getCompiler("quilc");

  const std::string src = R"##(__qpu__ void ansatz(qbit q, double theta) {
X 0
CNOT 1 0
RY(theta) 1
})##";

  if (quilc->canParse(src)) {
    auto ir = quilc->compile(src);
    auto k = ir->getComposites()[0];

    std::cout << "Ansatz:\n" << k->toString() << "\n";

    xacc::internal_compiler::qreg q(2);

    for (auto val : {2.2, 3.3, 4.4}) {
      k->updateRuntimeArguments(q, val);
      std::cout << "Ansatz:\n" << k->toString() << "\n";
    }
  }
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
