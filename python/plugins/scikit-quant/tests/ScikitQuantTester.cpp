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
#include "xacc_observable.hpp"

TEST(ScikitQuantTester, checkSimple) {

  auto optimizer = xacc::getService<xacc::Optimizer>("skquant");

  xacc::OptFunction f([](const std::vector<double> &x,
                         std::vector<double> &g) { return x[0] * x[0] + 5; },
                      1);

  EXPECT_EQ(1, f.dimensions());

  auto result = optimizer->optimize(f);

  EXPECT_EQ(5.0, result.first);
  EXPECT_EQ(result.second, std::vector<double>{0.0});
}

TEST(ScikitQuantTester, testVqe) {
  auto accelerator = xacc::getAccelerator("qpp");
  // Create the N=2 deuteron Hamiltonian
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));

  auto optimizer = xacc::getOptimizer("skquant");
  xacc::qasm(R"(
        .compiler xasm
        .circuit deuteron_ansatz
        .parameters theta
        .qbit q
        X(q[0]);
        Ry(q[1], theta);
        CNOT(q[1],q[0]);
    )");
  auto ansatz = xacc::getCompiled("deuteron_ansatz");

  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe");
  vqe->initialize({std::make_pair("ansatz", ansatz),
                   std::make_pair("observable", H_N_2),
                   std::make_pair("accelerator", accelerator),
                   std::make_pair("optimizer", optimizer)});

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  vqe->execute(buffer);
  std::cout << "Energy: " << (*buffer)["opt-val"].as<double>() << "\n";
  // Expected result: -1.74886
  EXPECT_NEAR((*buffer)["opt-val"].as<double>(), -1.74886, 1e-3);
}

TEST(ScikitQuantTester, checkRosenbrock) {
  auto optimizer = xacc::getService<xacc::Optimizer>("skquant");

  // return (1.-x[0])**2 + 100*(x[1]-x[0]**2)**2

  xacc::OptFunction f(
      [](const std::vector<double> &x, std::vector<double> &grad) {
        return 100 * std::pow(x[1] - std::pow(x[0], 2), 2) +
               std::pow(1 - x[0], 2);
      },
      2);

  EXPECT_EQ(2, f.dimensions());

  optimizer->setOptions(
      {{"budget",
        10}}); //, {"bounds",
               // std::vector<std::vector<double>>{{-.5,.5},{-.5,.5}}}});
  auto result = optimizer->optimize(f);

  EXPECT_NEAR(result.first, 1.0, 1e-4);
  EXPECT_NEAR(result.second[0], 0.0, 1e-4);
  EXPECT_NEAR(result.second[1], 0.0, 1e-4);
}

TEST(ScikitQuantTester, checkSnobFitTest) {
  auto optimizer = xacc::getService<xacc::Optimizer>("skquant");

  xacc::OptFunction f(
      [](const std::vector<double> &x, std::vector<double> &grad) {
        double fv = x[0] * x[0] + x[1] * x[1];
        fv *= 1. + 0.1 * std::sin(10. * (x[0] + x[1]));
        return fv;
      },
      2);

  EXPECT_EQ(2, f.dimensions());

  std::vector<double> x0; //{.5, .5};
  int budget = 40;
  std::vector<std::vector<double>> bounds{{-1., 1.}, {-1., 1.}};
  optimizer->setOptions({{"budget", budget},
                         {"maxmp", 8},
                         {"initial-parameters", x0},
                         {"method", "snobfit"},
                         {"bounds", bounds}});

  auto [optval, optpar] = optimizer->optimize(f);

  auto diff = optpar[0] + optpar[1] - (-.00112 - .00078);
  EXPECT_NEAR(diff, 0.0, 1e-4);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
