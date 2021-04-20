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
#include <random>
#include <gtest/gtest.h>

#include "xacc.hpp"
#include "Algorithm.hpp"
#include "Observable.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"

using namespace xacc;

TEST(QAOATester, checkSimple) {
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(2);

  auto optimizer = xacc::getOptimizer("nlopt");
  auto qaoa = xacc::getService<Algorithm>("QAOA");
  // Create deuteron Hamiltonian
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));
  EXPECT_TRUE(qaoa->initialize({std::make_pair("accelerator", acc),
                                std::make_pair("optimizer", optimizer),
                                std::make_pair("observable", H_N_2),
                                // number of time steps (p) param
                                std::make_pair("steps", 2)}));
  qaoa->execute(buffer);

  // The optimal value (minimal energy) must be less than -1.74886
  // which is the result from VQE using a physical ansatz.
  // In QAOA, we don't have any physical constraints, hence,
  // it can find a solution that gives a lower cost function value.
  std::cout << "Opt-val = " << (*buffer)["opt-val"].as<double>() << "\n";
  EXPECT_LT((*buffer)["opt-val"].as<double>(), -1.54);
}

TEST(QAOATester, checkStandardParamterizedScheme) {
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
      qaoa->initialize({std::make_pair("accelerator", acc),
                        std::make_pair("optimizer", optimizer),
                        std::make_pair("observable", H_N_2),
                        // number of time steps (p) param
                        std::make_pair("steps", 4),
                        std::make_pair("parameter-scheme", "Standard")}));
  qaoa->execute(buffer);
  std::cout << "Opt-val = " << (*buffer)["opt-val"].as<double>() << "\n";
  EXPECT_LT((*buffer)["opt-val"].as<double>(), -1.58);
}

// Generate rando
auto random_vector(const double l_range, const double r_range,
                   const std::size_t size) {
  // Generate a random initial parameter set
  std::random_device rnd_device;
  std::mt19937 mersenne_engine{rnd_device()}; // Generates random integers
  std::uniform_real_distribution<double> dist{l_range, r_range};
  auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };
  std::vector<double> vec(size);
  std::generate(vec.begin(), vec.end(), gen);
  return vec;
}

TEST(QAOATester, checkP1TriangleGraph) {
  auto acc = xacc::getAccelerator("aer", {{"sim-type", "statevector"}});
  auto optimizer = xacc::getOptimizer(
      "nlopt",
      {{"maximize", true}, {"initial-parameters", random_vector(-2., 2., 2)}});
  auto H = xacc::quantum::getObservable(
      "pauli", std::string("1.5 I - 0.5 Z0 Z1 - 0.5 Z0 Z2 - 0.5 Z1 Z2"));
  auto qaoa = xacc::getAlgorithm("QAOA", {{"accelerator", acc},
                                          {"optimizer", optimizer},
                                          {"observable", H},
                                          {"steps", 1},
                                          {"parameter-scheme", "Standard"}});
 auto all_betas =
      xacc::linspace(-xacc::constants::pi / 4., xacc::constants::pi / 4., 20);
  auto all_gammas = xacc::linspace(-xacc::constants::pi, xacc::constants::pi, 20);
  for (auto gamma : all_gammas) {
    for (auto beta : all_betas) {
      auto buffer = xacc::qalloc(3);
      auto cost =
          qaoa->execute(buffer, std::vector<double>{gamma, beta})[0];
      auto d = 1;
      auto e = 1;
      auto f = 1;
      auto theory = 3 * (.5 +
                         .25 * std::sin(4 * beta) * std::sin(gamma) *
                             (std::cos(gamma) + std::cos(gamma)) -
                         .25 * std::sin(2 * beta) * std::sin(2 * beta) *
                             (std::pow(std::cos(gamma), d + e - 2 * f)) *
                             (1 - std::cos(2 * gamma)));
      EXPECT_NEAR(cost, theory, 1e-3);
    }
  }
}

// Making sure that a set of Hadamards can be passed
// as the "initial-state" to the QAOA algorithm and 
// the proper result is returned
TEST(QAOATester, checkInitialStateConstruction) {
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(2);

  auto optimizer = xacc::getOptimizer("nlopt",  {{"initial-parameters", random_vector(-2., 2., 8)}});
  auto qaoa = xacc::getService<Algorithm>("QAOA");
  // Create deuteron Hamiltonian
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));

  auto provider = getIRProvider("quantum");
  auto initial_program = provider->createComposite("qaoaKernel");
  for (size_t i = 0; i < buffer->size(); i++) {
    initial_program->addInstruction(provider->createInstruction("H", { i }));
  }
  EXPECT_TRUE(
      qaoa->initialize({std::make_pair("accelerator", acc),
                        std::make_pair("optimizer", optimizer),
                        std::make_pair("observable", H_N_2),
                        // number of time steps (p) param
                        std::make_pair("steps", 4),
                        std::make_pair("initial-state", initial_program),
                        std::make_pair("parameter-scheme", "Standard")}));
  qaoa->execute(buffer);
  std::cout << "Opt-val = " << (*buffer)["opt-val"].as<double>() << "\n";
  EXPECT_LT((*buffer)["opt-val"].as<double>(), -1.5);
}


// Making sure that warm-starts can be initialized and run.
// TODO: Check that there are the same amount of either Rx or Rz
// gates as there are qubits. (Each qubit is assigned an Rx and Rz)
TEST(QAOATester, checkWarmStarts) {
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(3);
  int size = buffer->size();
  auto optimizer = xacc::getOptimizer("nlopt", {{"maximize",true},{"initial-parameters", random_vector(-2., 2., 2)}});
  auto qaoa = xacc::getService<Algorithm>("maxcut-qaoa");
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
       std::make_pair("optimizer", optimizer), 
       std::make_pair("graph", graph),
       // number of time steps (p) param
       std::make_pair("steps", 1),
       // "Standard" or "Extended"
       std::make_pair("initialization", "warm-start"),
       std::make_pair("parameter-scheme", "Standard")});
  qaoa->execute(buffer);
  std::cout << "Opt-val: " << (*buffer)["opt-val"].as<double>() << "\n";
  // EXPECT_LT(nbRxgates, buffer->size())
}

// Testing if a weighted graph can be constructed and passed
// to the maxcut-qaoa algorithm with the correct result returned.
TEST(QAOATester, checkWeightedQAOA) {
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(3);
  //   xacc::set_verbose(true);
  int size = buffer->size();
  auto optimizer = xacc::getOptimizer("nlopt", {{"maximize", true}, {"initial-parameters", random_vector(-2., 2., 2)}});
  auto qaoa = xacc::getService<Algorithm>("maxcut-qaoa");
  auto graph = xacc::getService<xacc::Graph>("boost-digraph");

  // Triangle graph
  for (int i = 0; i < 3; i++) {
    graph->addVertex();
  }
  graph->addEdge(0, 1, 0.15);
  graph->addEdge(0, 2, 0.85);

  const bool initOk = qaoa->initialize(
      {std::make_pair("accelerator", acc),
       std::make_pair("optimizer", optimizer), 
       std::make_pair("graph", graph),
       // number of time steps (p) param
       std::make_pair("steps", 1),
       // "Standard" or "Extended"
       std::make_pair("parameter-scheme", "Standard")});
  qaoa->execute(buffer);
  std::cout << "Min Val: " << (*buffer)["opt-val"].as<double>() << "\n";
}

TEST(QAOATester, checkMaxCut) {
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(3);
  //   xacc::set_verbose(true);
  auto optimizer = xacc::getOptimizer("nlopt", {{"maximize", true}, {"initial-parameters", random_vector(-2., 2., 2)}});
  auto qaoa = xacc::getService<Algorithm>("maxcut-qaoa");
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
       std::make_pair("optimizer", optimizer), 
       std::make_pair("graph", graph),
       // number of time steps (p) param
       std::make_pair("steps", 1),
       // "Standard" or "Extended"
       std::make_pair("parameter-scheme", "Standard")});
  qaoa->execute(buffer);
  std::cout << "Opt-val: " << (*buffer)["opt-val"].as<double>() << "\n";
  // EXPECT_NEAR((*buffer)["opt-val"].as<double>(), 2.0, 1e-3);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
