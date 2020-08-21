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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/

#include "xacc.hpp"
#include <gtest/gtest.h>
#include "xacc_observable.hpp"

TEST(AerAcceleratorTester, checkSimple) {
  auto accelerator =
      xacc::getAccelerator("aer", {std::make_pair("shots", 2048)});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void bell(qbit q) {
      H(q[0]);
      CX(q[0], q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })",
                                  accelerator);

  auto program = ir->getComposite("bell");

  auto buffer = xacc::qalloc(2);
  accelerator->execute(buffer, program);

  buffer->print(std::cout);

  accelerator->updateConfiguration({std::make_pair("sim-type", "statevector")});

  buffer->resetBuffer();
  accelerator->execute(buffer, program);
  buffer->print(std::cout);
}

TEST(AerAcceleratorTester, checkBell) {
  auto accelerator =
      xacc::getAccelerator("aer", {std::make_pair("shots", 2048)});
  auto xasmCompiler = xacc::getCompiler("xasm");
  // Tests circuit that has non-continuous use of qubits.
  auto ir = xasmCompiler->compile(R"(__qpu__ void bell1(qbit q) {
      H(q[0]);
      CX(q[0], q[4]);
      Measure(q[0]);
      Measure(q[4]);
    })",
                                  accelerator);

  auto program = ir->getComposite("bell1");

  auto buffer = xacc::qalloc(5);
  accelerator->execute(buffer, program);

  buffer->print(std::cout);
  // The bitstring result maps back to a two-qubit basis since we only measure two of them.
  EXPECT_NEAR(buffer->computeMeasurementProbability("00"), 0.5, 0.1);
  EXPECT_NEAR(buffer->computeMeasurementProbability("11"), 0.5, 0.1);
}

TEST(AerAcceleratorTester, checkDeuteron) {
  auto accelerator =
      xacc::getAccelerator("aer", {std::make_pair("sim-type", "statevector")});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void ansatz(qbit q, double t) {
      X(q[0]);
      Ry(q[1], t);
      CX(q[1], q[0]);
      H(q[0]);
      H(q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })",
                                  accelerator);

  auto program = ir->getComposite("ansatz");
  // Expected results from deuteron_2qbit_xasm_X0X1
  const std::vector<double> expectedResults{
      0.0,       -0.324699, -0.614213, -0.837166, -0.9694,
      -0.996584, -0.915773, -0.735724, -0.475947, -0.164595,
      0.164595,  0.475947,  0.735724,  0.915773,  0.996584,
      0.9694,    0.837166,  0.614213,  0.324699,  0.0};

  const auto angles =
      xacc::linspace(-xacc::constants::pi, xacc::constants::pi, 20);
  for (size_t i = 0; i < angles.size(); ++i) {
    auto buffer = xacc::qalloc(2);
    auto evaled = program->operator()({angles[i]});
    accelerator->execute(buffer, evaled);
    EXPECT_NEAR(buffer->getExpectationValueZ(), expectedResults[i], 1e-6);
  }
}
TEST(AerAcceleratorTester, checkDeuteronVqeH2) {
  // Use Qpp accelerator
  auto accelerator =
      xacc::getAccelerator("aer", {std::make_pair("sim-type", "statevector")});

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

  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe");
  vqe->initialize({std::make_pair("ansatz", ansatz),
                   std::make_pair("observable", H_N_2),
                   std::make_pair("accelerator", accelerator),
                   std::make_pair("optimizer", optimizer)});

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  vqe->execute(buffer);

  // Expected result: -1.74886
  EXPECT_NEAR((*buffer)["opt-val"].as<double>(), -1.74886, 1e-4);
}

TEST(AerAcceleratorTester, checkNoise) {
  // CI test don't have access to IBM, turn this on if you want to run
  //   auto accelerator =
  //       xacc::getAccelerator("aer:ibmq_vigo", {std::make_pair("shots",
  //       100)});

  //   auto xasmCompiler = xacc::getCompiler("xasm");
  //   auto ir = xasmCompiler->compile(R"(__qpu__ void bell2(qbit q) {
  //       H(q[0]);
  //       CX(q[0], q[1]);
  //       Measure(q[0]);
  //       Measure(q[1]);
  //     })",
  //                                   accelerator);

  //   auto program = ir->getComposite("bell2");

  //   auto buffer = xacc::qalloc(2);
  //   accelerator->execute(buffer, program);

  //   buffer->print(std::cout);
}

TEST(AerAcceleratorTester, checkShots) {
  const int nbShots = 100;
  {
    auto accelerator =
        xacc::getAccelerator("aer", {std::make_pair("shots", nbShots),
                                     std::make_pair("sim-type", "qasm")});
    // Allocate some qubits
    auto buffer = xacc::qalloc(2);
    auto quilCompiler = xacc::getCompiler("quil");
    // Nothing, should be 00
    auto ir = quilCompiler->compile(R"(__qpu__ void test1(qbit q) {
MEASURE 0 [0]
MEASURE 1 [1]
})",
                                    accelerator);
    accelerator->execute(buffer, ir->getComposites()[0]);
    EXPECT_EQ(buffer->getMeasurementCounts().size(), 1);
    EXPECT_EQ(buffer->getMeasurementCounts()["00"], nbShots);
    buffer->print();
  }
  {
    auto accelerator =
        xacc::getAccelerator("aer", {std::make_pair("shots", nbShots)});
    auto buffer = xacc::qalloc(2);
    auto quilCompiler = xacc::getCompiler("quil");
    // Expected "11"
    auto ir = quilCompiler->compile(R"(__qpu__ void test2(qbit q) {
X 0
CX 0 1
MEASURE 0 [0]
MEASURE 1 [1]
})",
                                    accelerator);
    accelerator->execute(buffer, ir->getComposites()[0]);

    EXPECT_EQ(buffer->getMeasurementCounts().size(), 1);
    EXPECT_EQ(buffer->getMeasurementCounts()["11"], nbShots);
  }
  {
    auto accelerator =
        xacc::getAccelerator("aer", {std::make_pair("shots", nbShots)});
    auto buffer = xacc::qalloc(2);
    auto quilCompiler = xacc::getCompiler("quil");
    // Bell states
    auto ir = quilCompiler->compile(R"(__qpu__ void test3(qbit q) {
H 0
CX 0 1
MEASURE 0 [0]
MEASURE 1 [1]
})",
                                    accelerator);
    accelerator->execute(buffer, ir->getComposites()[0]);
    EXPECT_EQ(buffer->getMeasurementCounts().size(), 2);
    // Only 00 and 11 states
    EXPECT_EQ(buffer->getMeasurementCounts()["11"] +
                  buffer->getMeasurementCounts()["00"],
              nbShots);
  }
}

TEST(AerAcceleratorTester, checkNoiseJson) {
  // Single-qubit noise model Json (IBMQ armonk)
  const std::string noiseModelJson =
      R"({"errors": [{"type": "qerror", "operations": ["id"], "instructions": [[{"name": "kraus", "qubits": [0], "params": [[[[-0.9999288015600207, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.999543331186472, 0.0]]], [[[-0.011590063902425698, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.01159453356984534, 0.0]]], [[[0.0, 0.0], [0.0, 0.0]], [[0.0028394065361603033, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.027905122682909723, 0.0]], [[0.0, 0.0], [0.0, 0.0]]]]}]], "probabilities": [1.0], "gate_qubits": [[0]]}, {"type": "qerror", "operations": ["u2"], "instructions": [[{"name": "kraus", "qubits": [0], "params": [[[[-0.9999288015600207, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.999543331186472, 0.0]]], [[[-0.011590063902425698, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.01159453356984534, 0.0]]], [[[0.0, 0.0], [0.0, 0.0]], [[0.0028394065361603033, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.027905122682909723, 0.0]], [[0.0, 0.0], [0.0, 0.0]]]]}]], "probabilities": [1.0], "gate_qubits": [[0]]}, {"type": "qerror", "operations": ["u3"], "instructions": [[{"name": "kraus", "qubits": [0], "params": [[[[-0.9998576313895062, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.9990866804895971, 0.0]]], [[[-0.016385345211158933, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.016397989055664672, 0.0]]], [[[0.0, 0.0], [0.0, 0.0]], [[0.0040295675297832316, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.039457709300053316, 0.0]], [[0.0, 0.0], [0.0, 0.0]]]]}]], "probabilities": [1.0], "gate_qubits": [[0]]}, {"type": "roerror", "operations": ["measure"], "probabilities": [[0.9534, 0.046599999999999975], [0.057, 0.943]], "gate_qubits": [[0]]}], "x90_gates": []})";

  auto accelerator = xacc::getAccelerator(
      "aer", {{"noise-model", noiseModelJson}, {"shots", 8192}});

  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program1 = xasmCompiler
                      ->compile(R"(__qpu__ void testRo(qbit q) {
        Measure(q[0]);
      })",
                                accelerator)
                      ->getComposite("testRo");
  auto program2 = xasmCompiler
                      ->compile(R"(__qpu__ void testRbH(qbit q) {
        // Randomized benchmarking
        for (int i = 0; i < 100; i++) {
            H(q[0]);
        }
        Measure(q[0]);
      })",
                                accelerator)
                      ->getComposite("testRbH");
  auto program3 = xasmCompiler
                      ->compile(R"(__qpu__ void testRbX(qbit q) {
        // Randomized benchmarking
        for (int i = 0; i < 100; i++) {
            X(q[0]);
        }
        Measure(q[0]);
      })",
                                accelerator)
                      ->getComposite("testRbX");

  // Readout error test:
  auto buffer1 = xacc::qalloc(1);
  accelerator->execute(buffer1, program1);
  // Randomized benchmarking of Hadamard gate
  auto buffer2 = xacc::qalloc(1);
  accelerator->execute(buffer2, program2);
  // Randomized benchmarking of X gate
  auto buffer3 = xacc::qalloc(1);
  accelerator->execute(buffer3, program3);
  // Check readout error:
  EXPECT_NEAR(buffer1->computeMeasurementProbability("1"), 0.046599999999999975,
              1e-2);
  // More errors due to gates (not perfectly cancelled)
  EXPECT_GT(buffer2->computeMeasurementProbability("1"),
            buffer1->computeMeasurementProbability("1"));
  // X gate is longer than H gate.
  EXPECT_GT(buffer3->computeMeasurementProbability("1"),
            buffer2->computeMeasurementProbability("1"));
}

int main(int argc, char **argv) {
  xacc::Initialize();

  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();

  xacc::Finalize();

  return result;
}
