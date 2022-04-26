/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#include "AcceleratorDecorator.hpp"
#include "NoiseModel.hpp"
#include "xacc_observable.hpp"

using namespace xacc;

namespace {
const std::string msb_noise_model =
    R"({"gate_noise": [{"gate_name": "CNOT", "register_location": ["0", "1"], "noise_channels": [{"matrix": [[[[0.99498743710662, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.99498743710662, 0.0]]], [[[0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, -0.05773502691896258], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.05773502691896258], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.05773502691896258], [0.0, 0.0]]], [[[0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.05773502691896258, 0.0], [0.0, 0.0], [-0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.0, 0.0], [0.0, 0.0], [-0.05773502691896258, 0.0]]]]}]}], "bit_order": "MSB"})";
}

TEST(RichExtrapDecoratorTester, checkSimple) {
  int shots = 8192;
  int nExecs = 4;

  //   if (xacc::hasAccelerator("q")) {
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(2);

  auto compiler = xacc::getService<xacc::Compiler>("xasm");
  const std::string src = R"src(__qpu__ void foo(qbit q) {
       H(q[0]);
       CNOT(q[0],q[1]);
       Measure(q[0]);
       }
       )src";

  auto ir = compiler->compile(src, acc);
  auto f = ir->getComposite("foo");
  auto decorator = xacc::getService<AcceleratorDecorator>("rich-extrap");
  decorator->initialize({{"rich-extrap-r", 1}});
  decorator->setDecorated(acc);

  decorator->execute(buffer, f);
  //   }
}

TEST(RichExtrapDecoratorTester, checkMultiple) {
  int shots = 8192;
  int nExecs = 2;

  //   if (xacc::hasAccelerator("tnqvm")) {
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(2);

  auto compiler = xacc::getService<xacc::Compiler>("xasm");
  const std::string src = R"src(__qpu__ void f(qbit q) {
       H(q[0]);
       CNOT(q[0],q[1]);
       Measure(q[0]);
       }
       )src";

  const std::string src2 = R"src(__qpu__ void g(qbit q) {
       H(q[0]);
       CNOT(q[0],q[1]);
       Measure(q[0]);
       }
       )src";

  auto ir = compiler->compile(src, acc);
  auto ir2 = compiler->compile(src2, acc);

  auto f = ir->getComposite("f");
  auto g = ir2->getComposite("g");

  auto decorator = xacc::getService<AcceleratorDecorator>("rich-extrap");
  decorator->setDecorated(acc);
  decorator->initialize({{"rich-extrap-r", 3}});
  decorator->execute(buffer, {f, g});
  EXPECT_EQ(buffer->getChildren().size(), 2);  
  //   }
}

TEST(RichExtrapDecoratorTester, checkExtrapolation) {
  const int shots = 8192;
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", msb_noise_model}});
  const std::string ibmNoiseJson = noiseModel->toJson();
  auto acc = xacc::getAccelerator(
      "aer", {{"noise-model", ibmNoiseJson}, {"shots", shots}});
  auto buffer = xacc::qalloc(2);
  auto compiler = xacc::getService<xacc::Compiler>("xasm");
  const std::string src = R"src(__qpu__ void foo(qbit q) {
       H(q[0]);
       CNOT(q[0],q[1]);
       Measure(q[0]);
       Measure(q[1]);
       }
       )src";

  auto ir = compiler->compile(src, acc);
  auto f = ir->getComposites()[0];
  const std::vector<int> scaleFactors {1,3,5,7,9};
  auto decorator = xacc::getAcceleratorDecorator(
      "rich-extrap", acc, {{"scale-factors", scaleFactors}});
  decorator->execute(buffer, f);
  buffer->print();
  EXPECT_TRUE(buffer->hasExtraInfoKey("rich-extrap-exp-vals"));
  const std::vector<double> exptrapVals =
      buffer->getInformation("rich-extrap-exp-vals").as<std::vector<double>>();
  EXPECT_EQ(exptrapVals.size(), scaleFactors.size());
}

TEST(RichExtrapDecoratorTester, checkExtrapolationInVQE) {
  const int shots = 8192;
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", msb_noise_model}});
  const std::string ibmNoiseJson = noiseModel->toJson();
  auto acc = xacc::getAccelerator(
      "aer", {{"noise-model", ibmNoiseJson}, {"shots", shots}});

  // Create the N=2 deuteron Hamiltonian
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));

  auto optimizer = xacc::getOptimizer("nlopt");
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
  auto decorator = xacc::getAcceleratorDecorator("rich-extrap", acc);
  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe");
  vqe->initialize({std::make_pair("ansatz", ansatz),
                   std::make_pair("observable", H_N_2),
                   std::make_pair("accelerator", decorator),
                   std::make_pair("optimizer", optimizer)});

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  xacc::set_verbose(true);
  vqe->execute(buffer, {0.594});
  buffer->print();
  const double energy = H_N_2->postProcess(buffer);
  std::cout << "Energy = " << energy << "\n";
  // Make it very relax just to verify the workflow.
  EXPECT_NEAR(energy, -1.74886, 0.5);
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
