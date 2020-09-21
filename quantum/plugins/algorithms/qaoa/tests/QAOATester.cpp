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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include <gtest/gtest.h>

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "Algorithm.hpp"
#include "xacc_observable.hpp"

using namespace xacc;

TEST(QAOATester, checkSimple) 
{
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(2);

  auto optimizer = xacc::getOptimizer("nlopt");
  auto qaoa = xacc::getService<Algorithm>("QAOA");
  // Create deuteron Hamiltonian
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));
  EXPECT_TRUE(
    qaoa->initialize({
                        std::make_pair("accelerator", acc),
                        std::make_pair("optimizer", optimizer),
                        std::make_pair("observable", H_N_2),
                        // number of time steps (p) param
                        std::make_pair("steps", 2)
                      }));
  qaoa->execute(buffer);
  
  // The optimal value (minimal energy) must be less than -1.74886 
  // which is the result from VQE using a physical ansatz.
  // In QAOA, we don't have any physical constraints, hence,
  // it can find a solution that gives a lower cost function value.
  std::cout << "Opt-val = " << (*buffer)["opt-val"].as<double>() << "\n";
  // EXPECT_LT((*buffer)["opt-val"].as<double>(), -1.74886);
}

TEST(QAOATester, checkStandardParamterizedScheme) 
{
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(2);

  auto optimizer = xacc::getOptimizer("nlopt");
  auto qaoa = xacc::getService<Algorithm>("QAOA");
  // Create deuteron Hamiltonian
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));
  EXPECT_TRUE(
    qaoa->initialize({
                        std::make_pair("accelerator", acc),
                        std::make_pair("optimizer", optimizer),
                        std::make_pair("observable", H_N_2),
                        // number of time steps (p) param
                        std::make_pair("steps", 4),
                        std::make_pair("parameter-scheme", "Standard")
                      }));
  qaoa->execute(buffer);
  std::cout << "Opt-val = " << (*buffer)["opt-val"].as<double>() << "\n";
  // EXPECT_LT((*buffer)["opt-val"].as<double>(), -1.74886);
}

TEST(QAOATester, checkMaxCut) {
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(3);
  xacc::set_verbose(true);
  auto optimizer = xacc::getOptimizer("nlopt");
  auto qaoa = xacc::getService<Algorithm>("QAOA");
  auto graph = xacc::getService<xacc::Graph>("boost-digraph");

  // Triangle graph
  for (int i = 0; i < 3; i++) {
    graph->addVertex();
  }
  graph->addEdge(0, 1);
  graph->addEdge(0, 2);
  graph->addEdge(1, 2);

  const bool initOk = qaoa->initialize(
      {std::make_pair("accelerator", acc),
       std::make_pair("optimizer", optimizer), std::make_pair("graph", graph),
       // number of time steps (p) param
       std::make_pair("steps", 1),
       // "Standard" or "Extended"
       std::make_pair("parameter-scheme", "Standard")});
  qaoa->execute(buffer);
  std::cout << "Min Val: " << (*buffer)["opt-val"].as<double>() << "\n";
  EXPECT_NEAR((*buffer)["opt-val"].as<double>(), 2.0, 1e-3);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
