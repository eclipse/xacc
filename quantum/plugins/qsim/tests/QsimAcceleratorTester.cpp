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
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "Optimizer.hpp"
#include "xacc_service.hpp"
#include "Algorithm.hpp"
#include "xacc_observable.hpp"
#include <random>
#include "CommonGates.hpp"

namespace {
template <typename T> std::vector<T> linspace(T a, T b, size_t N) {
  T h = (b - a) / static_cast<T>(N - 1);
  std::vector<T> xs(N);
  typename std::vector<T>::iterator x;
  T val;
  for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h) {
    *x = val;
  }
  return xs;
}
} // namespace

TEST(QsimAcceleratorTester, testExpVal) {
  auto accelerator = xacc::getAccelerator("qsim");
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program1 = xasmCompiler
                      ->compile(R"(__qpu__ void test1(qbit q) {
      H(q[0]);
      Measure(q[0]);
    })",
                                accelerator)
                      ->getComposites()[0];
  auto program2 = xasmCompiler
                      ->compile(R"(__qpu__ void test2(qbit q) {
      X(q[0]);
      Measure(q[0]);
    })",
                                accelerator)
                      ->getComposites()[0];

  auto program3 = xasmCompiler
                      ->compile(R"(__qpu__ void test3(qbit q) {
      X(q[0]);
      X(q[0]);
      Measure(q[0]);
    })",
                                accelerator)
                      ->getComposites()[0];

  auto buffer1 = xacc::qalloc(1);
  accelerator->execute(buffer1, program1);
  EXPECT_EQ(buffer1->getMeasurementCounts().size(), 0);
  EXPECT_NEAR(buffer1->getExpectationValueZ(), 0.0, 1e-9);

  auto buffer2 = xacc::qalloc(1);
  accelerator->execute(buffer2, program2);
  EXPECT_EQ(buffer2->getMeasurementCounts().size(), 0);
  EXPECT_NEAR(buffer2->getExpectationValueZ(), -1.0, 1e-9);

  auto buffer3 = xacc::qalloc(1);
  accelerator->execute(buffer3, program3);
  EXPECT_EQ(buffer3->getMeasurementCounts().size(), 0);
  EXPECT_NEAR(buffer3->getExpectationValueZ(), 1.0, 1e-9);
}

TEST(QsimAcceleratorTester, testBell) {
  auto accelerator = xacc::getAccelerator("qsim", {{"shots", 1024}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void bell(qbit q) {
      H(q[0]);
      CX(q[0], q[1]);
      CX(q[1], q[2]);
      Measure(q[0]);
      Measure(q[1]);
      Measure(q[2]);
    })",
                                  accelerator);

  auto program = ir->getComposite("bell");
  auto buffer = xacc::qalloc(3);
  accelerator->execute(buffer, program);
  EXPECT_EQ(buffer->getMeasurementCounts().size(), 2);
  auto prob0 = buffer->computeMeasurementProbability("000");
  auto prob1 = buffer->computeMeasurementProbability("111");
  buffer->print();
  EXPECT_NEAR(prob0 + prob1, 1.0, 1e-9);
  EXPECT_NEAR(prob0, 0.5, 0.2);
  EXPECT_NEAR(prob1, 0.5, 0.2);
}

TEST(QsimAcceleratorTester, testDeuteron) {
  auto accelerator = xacc::getAccelerator("qsim");
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

  const auto angles = linspace(-xacc::constants::pi, xacc::constants::pi, 20);
  for (size_t i = 0; i < angles.size(); ++i) {
    auto buffer = xacc::qalloc(2);
    auto evaled = program->operator()({angles[i]});
    accelerator->execute(buffer, evaled);
    std::cout << "Angle = " << angles[i]
              << "; result = " << buffer->getExpectationValueZ() << " vs. "
              << expectedResults[i] << "\n";
    EXPECT_NEAR(buffer->getExpectationValueZ(), expectedResults[i], 1e-6);
  }
}

TEST(QsimAcceleratorTester, testISwap) {
  // Get reference to the Accelerator
  xacc::set_verbose(false);
  const int nbShots = 100;
  auto accelerator = xacc::getAccelerator("qsim", {{"shots", nbShots}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void testISwap(qbit q) {
        X(q[0]);
        iSwap(q[0], q[3]);
        Measure(q[0]);
        Measure(q[1]);
        Measure(q[2]);
        Measure(q[3]);
        Measure(q[4]);
    })",
                                  accelerator);

  auto program = ir->getComposite("testISwap");
  // Allocate some qubits (5)
  auto buffer = xacc::qalloc(5);
  accelerator->execute(buffer, program);
  // 10000 => i00010 after iswap
  buffer->print();
  EXPECT_EQ(buffer->getMeasurementCounts()["00010"], nbShots);
}

TEST(QsimAcceleratorTester, testFsim) {
  // Get reference to the Accelerator
  const int nbShots = 1000;
  auto accelerator = xacc::getAccelerator("qsim", {{"shots", nbShots}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(
      R"(__qpu__ void testFsim(qbit q, double x, double y) {
        X(q[0]);
        fSim(q[0], q[2], x, y);
        Measure(q[0]);
        Measure(q[2]);
    })",
      accelerator);

  auto program = ir->getComposites()[0];
  const auto angles =
      xacc::linspace(-xacc::constants::pi, xacc::constants::pi, 10);

  for (const auto &a : angles) {
    auto buffer = xacc::qalloc(3);
    auto evaled = program->operator()({a, 0.0});
    accelerator->execute(buffer, evaled);
    const auto expectedProb = std::sin(a) * std::sin(a);
    std::cout << "Angle = " << a << "\n";
    buffer->print();
    // fSim mixes 01 and 10 states w.r.t. the theta angle.
    EXPECT_NEAR(buffer->computeMeasurementProbability("01"), expectedProb, 0.1);
    EXPECT_NEAR(buffer->computeMeasurementProbability("10"), 1.0 - expectedProb,
                0.1);
  }
}

TEST(QsimAcceleratorTester, testVqeMode) {
  auto accelerator = xacc::getAccelerator("qsim");
  // Create the N=2 deuteron Hamiltonian
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));

  auto optimizer = xacc::getOptimizer("nlopt", {{"maxeval", 40}});
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

TEST(QsimAcceleratorTester, testConditional) {
  const int nbTests = 100;
  auto accelerator = xacc::getAccelerator("qsim", {{"shots", nbTests}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void teleport(qbit q) {
        // State preparation (Bob)
        // Rotate an arbitrary angle
        Rx(q[0], -0.123);
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
        // Rotate back (-theta) on the teleported qubit
        Rx(q[2], 0.123);
        // Measure teleported qubit
        // This should always be zero (perfect teleportation)
        Measure(q[2]);
    })",
                                  accelerator);

  auto program = ir->getComposite("teleport");
  // Allocate some qubits
  auto buffer = xacc::qalloc(3);
  // Create a classical buffer to store measurement results (for conditional)
  buffer->setName("q");
  xacc::storeBuffer(buffer);
  accelerator->execute(buffer, program);
  buffer->print();
  int resultCount = 0;

  for (const auto &bitStrToCount : buffer->getMeasurementCounts()) {
    const std::string &bitString = bitStrToCount.first;
    // MSB bit must be zero (teleported)
    EXPECT_TRUE(bitString.front() == '0');
    resultCount += bitStrToCount.second;
  }

  EXPECT_EQ(resultCount, nbTests);
}

TEST(QsimAcceleratorTester, testMultiControlledGateNativeSim) {
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  auto x = std::make_shared<xacc::quantum::X>(0);
  std::shared_ptr<xacc::CompositeInstruction> comp =
      gateRegistry->createComposite("__COMPOSITE__X");
  comp->addInstruction(x);
  auto mcx = std::dynamic_pointer_cast<xacc::CompositeInstruction>(
      xacc::getService<xacc::Instruction>("C-U"));
  // Testing many controls, only possible (complete in reasonable time) with
  // custom C-U handler
  const std::vector<int> ctrl_idxs{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  const auto nQubits = ctrl_idxs.size() + 1;
  mcx->expand({{"U", comp}, {"control-idx", ctrl_idxs}});
  std::cout << "HOWDY: Gate count: " << mcx->nInstructions() << "\n";
  // Test truth table
  auto acc = xacc::getAccelerator("qsim", {std::make_pair("shots", 8192)});
  std::vector<std::shared_ptr<xacc::Instruction>> xGateVec;
  std::vector<std::shared_ptr<xacc::Instruction>> measGateVec;
  for (size_t i = 0; i < nQubits; ++i) {
    xGateVec.emplace_back(gateRegistry->createInstruction("X", {i}));
    measGateVec.emplace_back(gateRegistry->createInstruction("Measure", {i}));
  }

  const auto runTestCase = [&](const std::vector<bool> &bitVals) {
    static int counter = 0;
    auto composite = gateRegistry->createComposite("__TEMP_COMPOSITE__" +
                                                   std::to_string(counter));
    counter++;
    // State prep:
    assert(bitVals.size() == nQubits);
    std::string inputBitString;
    for (int i = 0; i < bitVals.size(); ++i) {
      if (bitVals[i]) {
        composite->addInstruction(xGateVec[i]);
      }
      inputBitString.append((bitVals[i] ? "1" : "0"));
    }

    // Add mcx
    composite->addInstruction(mcx);
    // Mesurement:
    composite->addInstructions(measGateVec);
    auto buffer = xacc::qalloc(nQubits);
    acc->execute(buffer, composite);
    // std::cout << "Input bitstring: " << inputBitString << "\n";
    // buffer->print();
    // MCX gate:
    const auto expectedBitString = [&inputBitString]() -> std::string {
      // If all control bits are 1's
      // q0q1q2q3q4
      const std::string pattern1(inputBitString.size(), '1');
      const std::string pattern0 = [&]() {
        std::string tmp(inputBitString.size(), '1');
        tmp[0] = '0';
        return tmp;
      }();
      if (inputBitString == pattern0) {
        return pattern1;
      }
      if (inputBitString == pattern1) {
        return pattern0;
      }
      // Otherwise, no changes
      return inputBitString;
    }();
    // Check bit string
    EXPECT_NEAR(buffer->computeMeasurementProbability(expectedBitString), 1.0,
                0.1);
    // std::cout << "Circuit: \n" << composite->toString() << "\n";
  };

  // Run some test cases randomly (since there are many)
  std::random_device rd;
  std::mt19937 gen(rd());
  // Max run
  const int N_RUNS = 32;
  const int N_STATES = 1 << nQubits;
  const double prob = static_cast<double>(N_RUNS) / N_STATES;
  std::bernoulli_distribution d(prob);
  for (int i = 0; i < (1 << nQubits); ++i) {
    if (d(gen)) {
      std::vector<bool> bits;
      for (int q = 0; q < nQubits; ++q) {
        bits.emplace_back((i & (1 << q)) == (1 << q));
      }
      runTestCase(bits);
    }
  }
}

TEST(QsimAcceleratorTester, checkRandomSeed) {
  auto accelerator =
      xacc::getAccelerator("qsim", {{"shots", 8192}, {"seed", 123}});
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
  EXPECT_EQ(buffer->getMeasurementCounts()["00"], 4059);
  EXPECT_EQ(buffer->getMeasurementCounts()["11"], 4133);
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();
  xacc::Finalize();
  return result;
}
