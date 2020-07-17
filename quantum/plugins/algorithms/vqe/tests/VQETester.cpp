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
#include "Optimizer.hpp"
#include "Observable.hpp"
#include "Algorithm.hpp"
#include "PauliOperator.hpp"

using namespace xacc;
const std::string rucc = R"rucc(__qpu__ void f(qbit q, double t0) {
    X(q[0]);
    X(q[1]);
    Rx(q[0],1.5707);
    H(q[1]);
    H(q[2]);
    H(q[3]);
    CNOT(q[0],q[1]);
    CNOT(q[1],q[2]);
    CNOT(q[2],q[3]);
    Rz(q[3], t0);
    CNOT(q[2],q[3]);
    CNOT(q[1],q[2]);
    CNOT(q[0],q[1]);
    Rx(q[0],-1.5707);
    H(q[1]);
    H(q[2]);
    H(q[3]);
})rucc";

TEST(VQETester, checkSimple) {
  //   if (xacc::hasAccelerator("qpp")) {
  auto acc = xacc::getAccelerator("qpp", {std::make_pair("vqe-mode", true)});
  auto buffer = xacc::qalloc(4); //->createBuffer("q", 4);

  auto compiler = xacc::getCompiler("xasm");

  auto ir = compiler->compile(rucc, nullptr);
  auto ruccsd = ir->getComposite("f");

  auto optimizer = xacc::getOptimizer("nlopt");
  std::shared_ptr<Observable> observable = std::make_shared<
      xacc::quantum::
          PauliOperator>(); //=
                            // std::make_shared<xacc::quantum::PauliOperator>();
  observable->fromString(
      "(0.174073,0) Z2 Z3 + (0.1202,0) Z1 Z3 + (0.165607,0) Z1 Z2 + "
      "(0.165607,0) Z0 Z3 + (0.1202,0) Z0 Z2 + (-0.0454063,0) Y0 Y1 X2 X3 + "
      "(-0.220041,0) Z3 + (-0.106477,0) + (0.17028,0) Z0 + (-0.220041,0) Z2 "
      "+ (0.17028,0) Z1 + (-0.0454063,0) X0 X1 Y2 Y3 + (0.0454063,0) X0 Y1 "
      "Y2 X3 + (0.168336,0) Z0 Z1 + (0.0454063,0) Y0 X1 X2 Y3");

  auto vqe = xacc::getService<Algorithm>("vqe");
  EXPECT_TRUE(vqe->initialize({{"ansatz", ruccsd},
                               {"accelerator", acc},
                               {"observable", observable},
                               {"optimizer", optimizer}}));
  vqe->execute(buffer);
  EXPECT_NEAR(-1.13717, mpark::get<double>(buffer->getInformation("opt-val")),
              1e-4);
  EXPECT_NEAR(-1.13717,
              vqe->execute(
                  buffer, (*buffer)["opt-params"].as<std::vector<double>>())[0],
              1e-4);
  // std::cout << "EVALED: " << vqe->execute(buffer,
  // (*buffer)["opt-params"].as<std::vector<double>>()) << "\n";
  //   }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
