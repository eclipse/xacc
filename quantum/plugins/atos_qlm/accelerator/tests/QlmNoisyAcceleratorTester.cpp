#include <gtest/gtest.h>
#include "xacc.hpp"
#include "Optimizer.hpp"
#include "xacc_service.hpp"
#include "Algorithm.hpp"
#include "xacc_observable.hpp"

TEST(QlmNoisyAcceleratorTester, testBell) {
  auto accelerator = xacc::getAccelerator("atos-qlm:ibmq_casablanca", {{"shots", 1024}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program1 = xasmCompiler
                      ->compile(R"(__qpu__ void bell(qbit q) {
      H(q[0]);
      CX(q[0], q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })",
                                accelerator)
                      ->getComposites()[0];

  auto buffer1 = xacc::qalloc(2);
  accelerator->execute(buffer1, program1);
  // Due to noise, we should get all 4 possible states:
  EXPECT_EQ(buffer1->getMeasurementCounts().size(), 4);
  buffer1->print();
  EXPECT_NEAR(buffer1->computeMeasurementProbability("00"), 0.5, 0.1);
  EXPECT_NEAR(buffer1->computeMeasurementProbability("11"), 0.5, 0.1);
}

TEST(QlmNoisyAcceleratorTester, testReadout) {
  auto accelerator = xacc::getAccelerator("atos-qlm:ibmq_casablanca", {{"shots", 1024}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program = xasmCompiler
                      ->compile(R"(__qpu__ void test(qbit q) {
      X(q[0]);
      Measure(q[1]);
      Measure(q[0]);
    })",
                                accelerator)
                      ->getComposites()[0];

  auto buffer = xacc::qalloc(3);
  accelerator->execute(buffer, program);
  buffer->print();
  // Majority is `01`
  EXPECT_NEAR(buffer->computeMeasurementProbability("01"), 1.0, 0.2);
}

TEST(QlmNoisyAcceleratorTester, testParametricGate) {
  auto accelerator =
      xacc::getAccelerator("atos-qlm:ibmq_casablanca");
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program = xasmCompiler
                     ->compile(R"(__qpu__ void rotation(qbit q, double theta) {
      Rx(q[0], theta);
      Measure(q[0]);
    })",
                               accelerator)
                     ->getComposites()[0];

  const auto angles =
      xacc::linspace(-xacc::constants::pi, xacc::constants::pi, 20);
  for (size_t i = 0; i < angles.size(); ++i) {
    auto buffer = xacc::qalloc(1);
    auto evaled = program->operator()({angles[i]});
    accelerator->execute(buffer, evaled);
    const double expectedResult =
        1.0 - 2.0 * std::sin(angles[i] / 2.0) * std::sin(angles[i] / 2.0);
    std::cout << "Angle = " << angles[i]
              << "; result = " << buffer->getExpectationValueZ()
              << " vs expected = " << expectedResult << "\n";
    EXPECT_NEAR(buffer->getExpectationValueZ(), expectedResult, 0.25);
  }
}

// Test batch mode:
TEST(QlmNoisyAcceleratorTester, testVqe) {
  auto accelerator = xacc::getAccelerator("atos-qlm:ibmq_casablanca");
  // Create the N=2 deuteron Hamiltonian
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));
  // Set a relaxed `ftol` due to noise
  auto optimizer = xacc::getOptimizer("nlopt", {{"nlopt-ftol", 0.01}});
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
  EXPECT_NEAR((*buffer)["opt-val"].as<double>(), -1.74886, 0.5);
}

// Check connectivity graph
TEST(QlmNoisyAcceleratorTester, testConnectivity) {
  auto accelerator = xacc::getAccelerator("atos-qlm:ibmq_casablanca");
  auto connectivity = accelerator->getConnectivity();
  for (const auto &[q1, q2] : connectivity) {
    std::cout << q1 << " -- " << q2 << "\n";
  }
  EXPECT_EQ(connectivity.size(), 6);
}

TEST(QlmNoisyAcceleratorTester, testNonNativeGate) {
  auto accelerator = xacc::getAccelerator("atos-qlm:ibmq_casablanca");
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program = xasmCompiler
                     ->compile(R"(__qpu__ void testCh(qbit q) {
      X(q[0]);
      CH(q[0], q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })",
                               accelerator)
                     ->getComposites()[0];

  auto buffer = xacc::qalloc(2);
  accelerator->execute(buffer, program);
  buffer->print();
  EXPECT_NEAR(buffer->computeMeasurementProbability("10"), 0.5, 0.2);
  EXPECT_NEAR(buffer->computeMeasurementProbability("11"), 0.5, 0.2);
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();
  xacc::Finalize();
  return result;
}
