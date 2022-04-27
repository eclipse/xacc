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

#include "AcceleratorDecorator.hpp"
#include "xacc.hpp"
#include <gtest/gtest.h>
#include <memory>
#include "xacc_service.hpp"
#include "NoiseModel.hpp"
#include "xacc_observable.hpp"

using namespace xacc;

namespace {
const std::string ro_error_noise_model =
    R"({"gate_noise": [], "bit_order": "MSB", "readout_errors": [{"register_location": "0", "prob_meas0_prep1": 0.025, "prob_meas1_prep0": 0.015}, {"register_location": "1", "prob_meas0_prep1": 0.035, "prob_meas1_prep0": 0.02}]})";
}

// If we want to run remote tests (required IBMQ credentials)
// Turn off by default for CI testing.
#define RUN_REMOTE_TESTS 0

#if RUN_REMOTE_TESTS
TEST(AssignmentErrorKernelDecoratorTest, checkBasic) {
  xacc::set_verbose(true);
  if (xacc::hasAccelerator("aer")) {
    auto accelerator = xacc::getAccelerator(
        "aer", {std::make_pair("shots", 2048),
                std::make_pair("backend", "ibmq_johannesburg"),
                std::make_pair("readout_error", true),
                std::make_pair("gate_error", false),
                std::make_pair("thermal_relaxation", false)});

    int num_qubits = 2;

    auto compiler = xacc::getService<xacc::Compiler>("xasm");
    xacc::qasm(R"(
.compiler xasm
.circuit bell
.qbit q
H(q[0]);
CX(q[0], q[1]);
Measure(q[0]);
Measure(q[1]);
)");
    auto bell = xacc::getCompiled("bell");
    auto decBuffer = xacc::qalloc(num_qubits);
    auto decorator =
        xacc::getService<AcceleratorDecorator>("assignment-error-kernel");
    decorator->initialize({std::make_pair("gen-kernel", true)});
    decorator->setDecorated(accelerator);
    decorator->execute(decBuffer, bell);
    decBuffer->print();
  } else {
    std::cout << "you do not have aer accelerator installed, please install "
                 "qiskit if you wish to run this test"
              << std::endl;
  }
}

TEST(AssignmentErrorKernelDecoratorTest, checkVectorize) {
  xacc::set_verbose(true);
  if (xacc::hasAccelerator("aer")) {
    auto accelerator = xacc::getAccelerator(
        "aer", {std::make_pair("shots", 2048),
                std::make_pair("backend", "ibmq_johannesburg"),
                std::make_pair("readout_error", true),
                std::make_pair("gate_error", true),
                std::make_pair("thermal_relaxation", true)});
    int num_qbits = 2;
    auto compiler = xacc::getService<xacc::Compiler>("xasm");
    xacc::qasm(R"(
.compiler xasm
.circuit hadamard
.qbit q
H(q[0]);
Measure(q[0]);
Measure(q[1]);
)");
    xacc::qasm(R"(
.compiler xasm
.circuit bell1
.qbit q
H(q[1]);
CX(q[1], q[0]);
Measure(q[0]);
Measure(q[1]);
)");

    auto hadamard = xacc::getCompiled("hadamard");
    auto bell1 = xacc::getCompiled("bell1");
    std::vector<std::shared_ptr<CompositeInstruction>> circuits = {hadamard,
                                                                   bell1};
    auto buffer = xacc::qalloc(num_qbits);
    auto decorator =
        xacc::getService<AcceleratorDecorator>("assignment-error-kernel");
    decorator->initialize({std::make_pair("gen-kernel", true)});
    decorator->setDecorated(accelerator);
    decorator->execute(buffer, circuits);
    buffer->print();
  }
}

TEST(AssignmentErrorKernelDecoratorTest, checkLayout) {
  xacc::set_verbose(true);
  if (xacc::hasAccelerator("aer")) {
    auto accelerator = xacc::getAccelerator(
        "aer", {std::make_pair("shots", 2048),
                std::make_pair("backend", "ibmq_tokyo"),
                std::make_pair("readout_error", true),
                std::make_pair("gate_error", true),
                std::make_pair("thermal_relaxation", true)});
    int num_qbits = 2;
    auto compiler = xacc::getService<xacc::Compiler>("xasm");
    xacc::qasm(R"(
.compiler xasm
.circuit hadamard1
.qbit q
H(q[1]);
Measure(q[0]);
Measure(q[1]);
)");
    auto hadamard = xacc::getCompiled("hadamard1");
    std::shared_ptr<CompositeInstruction> circuit = hadamard;
    auto buffer = xacc::qalloc(num_qbits);
    auto decorator =
        xacc::getService<AcceleratorDecorator>("assignment-error-kernel");
    decorator->initialize({std::make_pair("gen-kernel", true), std::make_pair("layout", std::vector<std::size_t> {6,7})});
    decorator->setDecorated(accelerator);
    decorator->execute(buffer, circuit);
    buffer->print();
  }
}
#endif


TEST(AssignmentErrorKernelDecoratorTest, checkSimple) {
  xacc::set_verbose(true);
  const int shots = 8192;
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", ro_error_noise_model}});
  const std::string ibmNoiseJson = noiseModel->toJson();
  auto acc = xacc::getAccelerator(
      "aer", {{"noise-model", ibmNoiseJson}, {"shots", shots}});
  auto buffer = xacc::qalloc(2);
  auto compiler = xacc::getService<xacc::Compiler>("xasm");
  const std::string src = R"src(__qpu__ void foo(qbit q) {
       H(q[0]);
       CX(q[0], q[1]);
       Measure(q[0]);
       Measure(q[1]);
       }
       )src";

  auto ir = compiler->compile(src, acc);
  auto decorator = xacc::getAcceleratorDecorator(
      "assignment-error-kernel", acc, {{"gen-kernel", true}});
  decorator->execute(buffer, ir->getComposites()[0]);
  buffer->print();
  EXPECT_TRUE(buffer->hasExtraInfoKey("unmitigated-counts"));
  auto unmitigated_counts = buffer->getInformation("unmitigated-counts")
                                .as<std::map<std::string, double>>();
  // Expect some improvements in statistics
  EXPECT_GT(static_cast<double>(buffer->getMeasurementCounts()["00"]),
            unmitigated_counts["00"]);
  EXPECT_GT(static_cast<double>(buffer->getMeasurementCounts()["11"]),
            unmitigated_counts["11"]);
}

TEST(AssignmentErrorKernelDecoratorTest, checkVecExecution) {
  const int shots = 8192;
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", ro_error_noise_model}});
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
  auto decorator = xacc::getAcceleratorDecorator("assignment-error-kernel", acc,
                                                 {{"gen-kernel", true}});
  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe");
  vqe->initialize({{"ansatz", ansatz},
                   {"observable", H_N_2},
                   {"accelerator", decorator},
                   {"optimizer", optimizer}});

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  xacc::set_verbose(true);
  vqe->execute(buffer, {0.594});
  buffer->print();
  const double energy = H_N_2->postProcess(buffer);
  std::cout << "Energy = " << energy << "\n";
  EXPECT_NEAR(energy, -1.74886, 0.25);
}

int main(int argc, char **argv) {
  int ret = 0;
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
