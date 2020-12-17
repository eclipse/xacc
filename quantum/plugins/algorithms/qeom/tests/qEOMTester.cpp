/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Daniel Claudino - initial API and implementation
 ******************************************************************************/
#include <gtest/gtest.h>
#include <memory>
#include <utility>
#include <vector>

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "Algorithm.hpp"
#include "xacc_observable.hpp"
#include "OperatorPool.hpp"
#include "FermionOperator.hpp"
#include "ObservableTransform.hpp"

using namespace xacc;
using namespace xacc::quantum;

TEST(qEOMTester, checkqEOM) {

  xacc::set_verbose(true);
  auto str = std::string(
      "(-0.165606823582,-0)  1^ 2^ 1 2 + (0.120200490713,0)  1^ 0^ 0 1 + "
      "(-0.0454063328691,-0)  0^ 3^ 1 2 + (0.168335986252,0)  2^ 0^ 0 2 + "
      "(0.0454063328691,0)  1^ 2^ 3 0 + (0.168335986252,0)  0^ 2^ 2 0 + "
      "(0.165606823582,0)  0^ 3^ 3 0 + (-0.0454063328691,-0)  3^ 0^ 2 1 + "
      "(-0.0454063328691,-0)  1^ 3^ 0 2 + (-0.0454063328691,-0)  3^ 1^ 2 0 + "
      "(0.165606823582,0)  1^ 2^ 2 1 + (-0.165606823582,-0)  0^ 3^ 0 3 + "
      "(-0.479677813134,-0)  3^ 3 + (-0.0454063328691,-0)  1^ 2^ 0 3 + "
      "(-0.174072892497,-0)  1^ 3^ 1 3 + (-0.0454063328691,-0)  0^ 2^ 1 3 + "
      "(0.120200490713,0)  0^ 1^ 1 0 + (0.0454063328691,0)  0^ 2^ 3 1 + "
      "(0.174072892497,0)  1^ 3^ 3 1 + (0.165606823582,0)  2^ 1^ 1 2 + "
      "(-0.0454063328691,-0)  2^ 1^ 3 0 + (-0.120200490713,-0)  2^ 3^ 2 3 + "
      "(0.120200490713,0)  2^ 3^ 3 2 + (-0.168335986252,-0)  0^ 2^ 0 2 + "
      "(0.120200490713,0)  3^ 2^ 2 3 + (-0.120200490713,-0)  3^ 2^ 3 2 + "
      "(0.0454063328691,0)  1^ 3^ 2 0 + (-1.2488468038,-0)  0^ 0 + "
      "(0.0454063328691,0)  3^ 1^ 0 2 + (-0.168335986252,-0)  2^ 0^ 2 0 + "
      "(0.165606823582,0)  3^ 0^ 0 3 + (-0.0454063328691,-0)  2^ 0^ 3 1 + "
      "(0.0454063328691,0)  2^ 0^ 1 3 + (-1.2488468038,-0)  2^ 2 + "
      "(0.0454063328691,0)  2^ 1^ 0 3 + (0.174072892497,0)  3^ 1^ 1 3 + "
      "(-0.479677813134,-0)  1^ 1 + (-0.174072892497,-0)  3^ 1^ 3 1 + "
      "(0.0454063328691,0)  3^ 0^ 1 2 + (-0.165606823582,-0)  3^ 0^ 3 0 + "
      "(0.0454063328691,0)  0^ 3^ 2 1 + (-0.165606823582,-0)  2^ 1^ 2 1 + "
      "(-0.120200490713,-0)  0^ 1^ 0 1 + (-0.120200490713,-0)  1^ 0^ 1 0 + "
      "(0.7080240981,0)");
  auto H = xacc::quantum::getObservable("fermion", str);
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(4);

  auto pool = xacc::getService<OperatorPool>("singlet-adapted-uccsd");
  pool->optionalParameters({{"n-electrons", 2}});
  pool->generate(buffer->size());
  auto ansatz = xacc::getIRProvider("quantum")->createComposite("ansatz");
  ansatz->addInstruction(
      xacc::getIRProvider("quantum")->createInstruction("X", {0}));
  ansatz->addInstruction(
      xacc::getIRProvider("quantum")->createInstruction("X", {2}));
  ansatz->addVariable("x0");
  for (auto &k : pool->getOperatorInstructions(2, 0)->getInstructions()) {
    ansatz->addInstruction(k);
  }
  auto kernel = ansatz->operator()({0.0808});

  auto qeom = xacc::getService<xacc::Algorithm>("qeom");
  EXPECT_TRUE(qeom->initialize({
      {"accelerator", acc},
      {"observable", H},
      {"n-electrons", 2},
      {"ansatz", kernel},
  }));
  qeom->execute(buffer);

  auto e =
      buffer->getInformation("excitation-energies").as<std::vector<double>>();

  EXPECT_NEAR(0.597466, e[0], 1e-4);
  EXPECT_NEAR(0.960717, e[1], 1e-4);
  EXPECT_NEAR(1.60298, e[2], 1e-4);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}