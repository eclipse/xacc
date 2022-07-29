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

#include "Accelerator.hpp"
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
  // The bitstring result maps back to a two-qubit basis since we only measure
  // two of them.
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

TEST(AerAcceleratorTester, testDeuteronVqeH3) {
  auto accelerator = xacc::getAccelerator("aer", {{"sim-type", "statevector"}});

  // Create the N=3 deuteron Hamiltonian
  auto H_N_3 = xacc::quantum::getObservable(
      "pauli",
      std::string("5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1 + "
                  "9.625 - 9.625 Z2 - 3.91 X1 X2 - 3.91 Y1 Y2"));

  auto optimizer = xacc::getOptimizer("nlopt");

  // JIT map Quil QASM Ansatz to IR
  xacc::qasm(R"(
        .compiler xasm
        .circuit deuteron_ansatz_h3
        .parameters t0, t1
        .qbit q
        X(q[0]);
        exp_i_theta(q, t1, {{"pauli", "X0 Y1 - Y0 X1"}});
        exp_i_theta(q, t0, {{"pauli", "X0 Z1 Y2 - X2 Z1 Y0"}});
    )");
  auto ansatz = xacc::getCompiled("deuteron_ansatz_h3");

  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe");
  vqe->initialize({std::make_pair("ansatz", ansatz),
                   std::make_pair("observable", H_N_3),
                   std::make_pair("accelerator", accelerator),
                   std::make_pair("optimizer", optimizer)});

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(3);
  vqe->execute(buffer);

  // Expected result: -2.04482
  EXPECT_NEAR((*buffer)["opt-val"].as<double>(), -2.04482, 1e-4);
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

TEST(AerAcceleratorTester, checkApply) {
  auto accelerator = xacc::getAccelerator("aer");
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void bell(qbit q) {
      H(q[0]);
      CX(q[0], q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })",
                                  accelerator);

  auto program = ir->getComposite("bell");
  const int NB_TESTS = 1000;
  int result_00 = 0;
  int result_11 = 0;

  for (int i = 0; i < NB_TESTS; ++i) {
    auto buffer = xacc::qalloc(2);
    for (size_t i = 0; i < program->nInstructions(); ++i) {
      auto curInst = program->getInstruction(i);
      accelerator->apply(buffer, curInst);
    }
    const bool resultQ0 = (*buffer)[0];
    const bool resultQ1 = (*buffer)[1];
    EXPECT_EQ(resultQ0, resultQ1);
    if (resultQ0) {
      ++result_00;
    } else {
      ++result_11;
    }
  }
  std::cout << "HOWDY " << result_00 << "--" << result_11 << "\n";
  EXPECT_EQ(result_00 + result_11, NB_TESTS);
  EXPECT_NEAR((double)result_00 / NB_TESTS, 0.5, 0.1);
  EXPECT_NEAR((double)result_11 / NB_TESTS, 0.5, 0.1);
}

TEST(AerAcceleratorTester, checkConditional) {
  auto accelerator = xacc::getAccelerator("aer");
  xacc::set_verbose(true);
  auto xasmCompiler = xacc::getCompiler("xasm");
  {
    auto ir = xasmCompiler->compile(R"(__qpu__ void conditionalCirc(qbit q) {
      X(q[0]);
      Measure(q[0]);
      Measure(q[1]);
      // Should apply
      if (q[0]) {
        X(q[2]);
      }
      // Not apply
      if (q[1]) {
        X(q[3]);
      }
      // Apply
      X(q[4]);
      Measure(q[2]);
      Measure(q[3]);
      Measure(q[4]);
    })",
                                    accelerator);

    auto program = ir->getComposite("conditionalCirc");

    auto buffer = xacc::qalloc(5);
    accelerator->execute(buffer, program);
    buffer->print();
    // Expected: q0 = 1, q1 = 0, q2 = 1, q3 = 0, q4 = 1
    EXPECT_EQ(buffer->computeMeasurementProbability("10101"), 1.0);
  }
  {
    // Check Teleport
    auto ir = xasmCompiler->compile(R"(__qpu__ void teleportOneState(qbit q) {
      // State to be transported
      X(q[0]);
      // Bell channel setup
      H(q[1]);
      CX(q[1], q[2]);
      // Alice Bell measurement
      CX(q[0], q[1]);
      H(q[0]);
      Measure(q[0]);
      Measure(q[1]);
      // Correction
      if (q[0])
      {
        Z(q[2]);
      }
      if (q[1])
      {
        X(q[2]);
      }
      // Measure teleported qubit
      Measure(q[2]);
    })",
                                    accelerator);

    auto program = ir->getComposite("teleportOneState");
    auto buffer = xacc::qalloc(3);
    accelerator->execute(buffer, program);
    buffer->print();
    for (const auto &bitStr : buffer->getMeasurements()) {
      EXPECT_EQ(bitStr.length(), 3);
      // q[2] (MSB) must be '1' (teleported)
      EXPECT_EQ(bitStr[0], '1');
    }
  }

  {
    // Check Teleport
    auto ir = xasmCompiler->compile(R"(__qpu__ void teleportZeroState(qbit q) {
      // Bell channel setup
      H(q[1]);
      CX(q[1], q[2]);
      // Alice Bell measurement
      CX(q[0], q[1]);
      H(q[0]);
      Measure(q[0]);
      Measure(q[1]);
      // Correction
      if (q[0])
      {
        Z(q[2]);
      }
      if (q[1])
      {
        X(q[2]);
      }
      // Measure teleported qubit
      Measure(q[2]);
    })",
                                    accelerator);

    auto program = ir->getComposite("teleportZeroState");
    auto buffer = xacc::qalloc(3);
    accelerator->execute(buffer, program);
    buffer->print();
    for (const auto &bitStr : buffer->getMeasurements()) {
      EXPECT_EQ(bitStr.length(), 3);
      // q[2] (MSB) must be '0' (teleported)
      EXPECT_EQ(bitStr[0], '0');
    }
  }

  {
    // Check Teleport (50-50)
    auto ir = xasmCompiler->compile(
        R"(__qpu__ void teleportSuperpositionState(qbit q) {
      // State to be transported
      // superposition of 0 and 1
      H(q[0]);
      // Bell channel setup
      H(q[1]);
      CX(q[1], q[2]);
      // Alice Bell measurement
      CX(q[0], q[1]);
      H(q[0]);
      Measure(q[0]);
      Measure(q[1]);
      // Correction
      if (q[0])
      {
        Z(q[2]);
      }
      if (q[1])
      {
        X(q[2]);
      }
      // Measure teleported qubit
      Measure(q[2]);
    })",
        accelerator);

    auto program = ir->getComposite("teleportSuperpositionState");
    auto buffer = xacc::qalloc(3);
    accelerator->execute(buffer, program);
    buffer->print();
    double zeroProb = 0.0;
    double oneProb = 0.0;
    for (const auto &bitStr : buffer->getMeasurements()) {
      EXPECT_EQ(bitStr.length(), 3);
      // q[2] (MSB) is the teleported qubit
      if (bitStr[0] == '0') {
        zeroProb += buffer->computeMeasurementProbability(bitStr);
      } else {
        oneProb += buffer->computeMeasurementProbability(bitStr);
      }
    }
    EXPECT_NEAR(zeroProb, 0.5, 0.1);
    EXPECT_NEAR(oneProb, 0.5, 0.1);
  }

  xacc::set_verbose(false);
}

// Test ability to set random seed
// For deterministic testing.
TEST(AerAcceleratorTester, checkRandomSeed) {
  auto accelerator =
      xacc::getAccelerator("aer", {{"shots", 8192}, {"seed", 123}});
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
  buffer->print();
  // The result will be deterministic since we seed the simulator
  EXPECT_EQ(buffer->getMeasurementCounts()["00"], 4073);
  EXPECT_EQ(buffer->getMeasurementCounts()["11"], 4119);
}

TEST(AerAcceleratorTester, testExecutionInfoStateVec) {
  auto accelerator =
      xacc::getAccelerator("aer", {std::make_pair("sim-type", "statevector")});

  xacc::qasm(R"(
        .compiler xasm
        .circuit test_bell_exe
        .qbit q
        H(q[0]);
        CNOT(q[0],q[1]);
    )");
  auto bell = xacc::getCompiled("test_bell_exe");

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  accelerator->execute(buffer, bell);

  auto exeInfo = accelerator->getExecutionInfo();
  EXPECT_GT(exeInfo.size(), 0);
  auto waveFn =
      accelerator->getExecutionInfo<xacc::ExecutionInfo::WaveFuncPtrType>(
          xacc::ExecutionInfo::WaveFuncKey);
  for (const auto &elem : *waveFn) {
    std::cout << elem << "\n";
  }
  // 2 qubits => 4 elements
  EXPECT_EQ(waveFn->size(), 4);
  EXPECT_NEAR(std::abs((*waveFn)[0] - 1.0 / std::sqrt(2.0)), 0.0, 1e-9);
  EXPECT_NEAR(std::abs((*waveFn)[3] - 1.0 / std::sqrt(2.0)), 0.0, 1e-9);
}

TEST(AerAcceleratorTester, testExecutionInfoDensityMat) {
  auto accelerator = xacc::getAccelerator(
      "aer", {std::make_pair("sim-type", "density_matrix")});

  xacc::qasm(R"(
        .compiler xasm
        .circuit test_bell_exe
        .qbit q
        H(q[0]);
        CNOT(q[0],q[1]);
    )");
  auto bell = xacc::getCompiled("test_bell_exe");

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  accelerator->execute(buffer, bell);

  auto exeInfo = accelerator->getExecutionInfo();
  EXPECT_GT(exeInfo.size(), 0);
  auto dm =
      accelerator->getExecutionInfo<xacc::ExecutionInfo::DensityMatrixPtrType>(
          xacc::ExecutionInfo::DmKey);
  for (const auto &row : *dm) {
    for (const auto &x : row) {
      std::cout << x << " ";
    }
    std::cout << "\n";
  }
  // 2 qubits => 4 x 4 matrix
  EXPECT_EQ(dm->size(), 4);
  for (const auto &row : *dm) {
    EXPECT_EQ(row.size(), 4);
  }
  EXPECT_NEAR(std::abs((*dm)[0][0] - 0.5), 0.0, 1e-9);
  EXPECT_NEAR(std::abs((*dm)[3][3] - 0.5), 0.0, 1e-9);
}

TEST(AerAcceleratorTester, checkDeuteronVqeH2DensityMatrix) {
  auto accelerator =
      xacc::getAccelerator("aer", {{"sim-type", "density_matrix"}});

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

TEST(AerAcceleratorTester, testDeuteronVqeH3DensityMatrix) {
  auto accelerator = xacc::getAccelerator("aer", {{"sim-type", "density_matrix"}});

  // Create the N=3 deuteron Hamiltonian
  auto H_N_3 = xacc::quantum::getObservable(
      "pauli",
      std::string("5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1 + "
                  "9.625 - 9.625 Z2 - 3.91 X1 X2 - 3.91 Y1 Y2"));

  auto optimizer = xacc::getOptimizer("nlopt");

  // JIT map Quil QASM Ansatz to IR
  xacc::qasm(R"(
        .compiler xasm
        .circuit deuteron_ansatz_h3
        .parameters t0, t1
        .qbit q
        X(q[0]);
        exp_i_theta(q, t1, {{"pauli", "X0 Y1 - Y0 X1"}});
        exp_i_theta(q, t0, {{"pauli", "X0 Z1 Y2 - X2 Z1 Y0"}});
    )");
  auto ansatz = xacc::getCompiled("deuteron_ansatz_h3");

  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe");
  vqe->initialize({std::make_pair("ansatz", ansatz),
                   std::make_pair("observable", H_N_3),
                   std::make_pair("accelerator", accelerator),
                   std::make_pair("optimizer", optimizer)});

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(3);
  vqe->execute(buffer);

  // Expected result: -2.04482
  EXPECT_NEAR((*buffer)["opt-val"].as<double>(), -2.04482, 1e-4);
}

TEST(AerAcceleratorTester, checkMatrixProductState) {
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void test1(qbit q) {
            H(q[0]);
            for (int i = 0; i < 99; i++) {
                CNOT(q[i], q[i + 1]);
            }
            for (int i = 0; i < 100; i++) {
                Measure(q[i]);
            }
        })");

  auto program = ir->getComposite("test1");
  auto accelerator = xacc::getAccelerator(
      "aer", {{"sim-type", "matrix_product_state"}, {"shots", 1024}});
  // Use a large enough qubit register to check memory constraint 
  // We can run with MPS!
  auto qreg = xacc::qalloc(100);
  accelerator->execute(qreg, program);
  qreg->print();
  // We expect to create a entangle state (1000 qubits): |000000 ... 00> +
  // |111.... 111>
  const auto prob0 =
      qreg->computeMeasurementProbability(std::string(qreg->size(), '0'));
  const auto prob1 =
      qreg->computeMeasurementProbability(std::string(qreg->size(), '1'));
  EXPECT_NEAR(prob0 + prob1, 1.0, 1e-12);
  EXPECT_NEAR(prob0, 0.5, 0.2);
  EXPECT_NEAR(prob1, 0.5, 0.2);
}

TEST(AerAcceleratorTester, checkInitialState1) {
  const std::vector<std::complex<double>> initial_states{0.0, 1.0};
  auto accelerator = xacc::getAccelerator(
      "aer", {{"sim-type", "statevector"}, {"initial_state", initial_states}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void test(qbit q) {
      H(q[0]);
    })",
                                  accelerator);

  auto program = ir->getComposite("test");

  auto buffer = xacc::qalloc(1);
  accelerator->execute(buffer, program);

  auto exeInfo = accelerator->getExecutionInfo();
  EXPECT_GT(exeInfo.size(), 0);
  auto waveFn =
      accelerator->getExecutionInfo<xacc::ExecutionInfo::WaveFuncPtrType>(
          xacc::ExecutionInfo::WaveFuncKey);
  for (const auto &elem : *waveFn) {
    std::cout << elem << "\n";
  }
  EXPECT_EQ(waveFn->size(), 2);
  // Expect |-> state: |0> - |1> since we set the initial state to |1>
  EXPECT_NEAR(std::abs((*waveFn)[0] - 1.0 / std::sqrt(2.0)), 0.0, 1e-9);
  EXPECT_NEAR(std::abs((*waveFn)[1] + 1.0 / std::sqrt(2.0)), 0.0, 1e-9);
}

TEST(AerAcceleratorTester, checkInitialState2) {
  const std::vector<std::complex<double>> initial_states{1.0 / std::sqrt(2.0),
                                                         -1.0 / std::sqrt(2.0)};
  const int nbShots = 8192;
  auto accelerator = xacc::getAccelerator(
      "aer", {{"initial_state", initial_states}, {"shots", nbShots}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void test(qbit q) {
      H(q[0]);
      Measure(q[0]);
    })",
                                  accelerator);

  auto program = ir->getComposite("test");

  auto buffer = xacc::qalloc(1);
  accelerator->execute(buffer, program);
  buffer->print();
  // H|-> => |1> since we set the initial state to |-> state
 EXPECT_EQ(buffer->getMeasurementCounts()["1"], nbShots);
}

TEST(AerAcceleratorTester, checkInitialState3) {
  const std::vector<std::complex<double>> initial_states{0.0, 1.0};
  auto accelerator =
      xacc::getAccelerator("aer", {{"sim-type", "density_matrix"},
                                   {"initial_state", initial_states}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void test(qbit q) {
      H(q[0]);
    })",
                                  accelerator);

  auto program = ir->getComposite("test");

  auto buffer = xacc::qalloc(1);
  accelerator->execute(buffer, program);

  auto exeInfo = accelerator->getExecutionInfo();
  EXPECT_GT(exeInfo.size(), 0);
  auto dm =
      accelerator->getExecutionInfo<xacc::ExecutionInfo::DensityMatrixPtrType>(
          xacc::ExecutionInfo::DmKey);
  for (const auto &row : *dm) {
    for (const auto &x : row) {
      std::cout << x << " ";
    }
    std::cout << "\n";
  }
  EXPECT_EQ(dm->size(), 2);
  for (const auto &row : *dm) {
    EXPECT_EQ(row.size(), 2);
  }
  EXPECT_NEAR(std::abs((*dm)[0][0] - 0.5), 0.0, 1e-9);
  EXPECT_NEAR(std::abs((*dm)[1][1] - 0.5), 0.0, 1e-9);
  EXPECT_NEAR(std::abs((*dm)[0][1] + 0.5), 0.0, 1e-9);
  EXPECT_NEAR(std::abs((*dm)[1][0] + 0.5), 0.0, 1e-9);
}

int main(int argc, char **argv) {
  xacc::Initialize();

  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();

  xacc::Finalize();

  return result;
}
