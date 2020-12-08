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
#include <utility>

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "Algorithm.hpp"
#include "xacc_observable.hpp"
#include "OperatorPool.hpp"

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
      "(-0.120200490713,-0)  0^ 1^ 0 1 + (-0.120200490713,-0)  1^ 0^ 1 0 + (0.0,0.0)");
     // "(0.7080240981,0)");
  auto H = xacc::quantum::getObservable("fermion", str);
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(4);
  auto optimizer = xacc::getOptimizer("nlopt");
  auto vqe = xacc::getService<xacc::Algorithm>("vqe");

  auto ansatz = std::dynamic_pointer_cast<xacc::CompositeInstruction>(
      xacc::getService<xacc::Instruction>("uccsd"));
  ansatz->expand({{"ne", 2}, {"nq", 4}, {"pool", "singlet-adapted-uccsd"}});

  // first optimize VQE circuit
  /*
  vqe->initialize({
      {"accelerator", acc},
      {"observable", H},
      {"optimizer", optimizer},
      {"ansatz", ansatz},
  });
  vqe->execute(buffer);
  auto x = buffer->getInformation("opt-params").as<std::vector<double>>();
  */
  std::cout << "HERE\n\n";

  auto pool = xacc::getService<OperatorPool>("uccsd-pool");
  pool->optionalParameters({{"n-electrons", 2}});
  auto operators = pool->generate(buffer->size());
   //std::cout << operators[0]->toString() <<"\n";
  //std::cout << operators[1]->toString() <<"\n";
  //std::cout << operators[2]->toString() <<"\n";

  //operators.erase(std::remove_if(operators.begin(), operators.end(), [](std::shared_ptr<Observable> op){ return (op->getSubTerms().size() == 0); }), operators.end());

  auto qeom = xacc::getService<xacc::Algorithm>("qeom");
  //auto kernel = ansatz->operator()({0.113882,0.000250428,-0.000356837});
  auto kernel = ansatz->operator()({0.081095,-0.000526981});
  EXPECT_TRUE(qeom->initialize({
      {"accelerator", acc},
      {"observable", H},
      {"operators", operators},
      {"ansatz", kernel},
  }));

  qeom->execute(buffer);
  
  // EXPECT_NEAR(-1.13717, buffer_vqe->getInformation("opt-val").as<double>(),
  // 1e-4);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}