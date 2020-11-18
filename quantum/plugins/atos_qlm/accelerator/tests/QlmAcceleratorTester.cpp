#include <gtest/gtest.h>
#include "xacc.hpp"
#include "Optimizer.hpp"
#include "xacc_service.hpp"
#include "Algorithm.hpp"
#include "xacc_observable.hpp"

TEST(QlmAcceleratorTester, testBell) {
  auto accelerator = xacc::getAccelerator("atos-qlm", {{"shots", 1024}});
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
  EXPECT_EQ(buffer1->getMeasurementCounts().size(), 2);
  buffer1->print();
  EXPECT_NEAR(buffer1->computeMeasurementProbability("00"), 0.5, 0.1);
  EXPECT_NEAR(buffer1->computeMeasurementProbability("11"), 0.5, 0.1);
}

TEST(QlmAcceleratorTester, testExpVal) {
  auto accelerator = xacc::getAccelerator("atos-qlm");
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

TEST(QlmAcceleratorTester, testParametricGate) {
  auto accelerator = xacc::getAccelerator("atos-qlm");
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program = xasmCompiler
                      ->compile(R"(__qpu__ void rotation(qbit q, double theta) {
      Rx(q[0], theta);
      Measure(q[0]);
    })",
                                accelerator)
                      ->getComposites()[0];

  const auto angles = xacc::linspace(-xacc::constants::pi, xacc::constants::pi, 20);
  for (size_t i = 0; i < angles.size(); ++i) {
    auto buffer = xacc::qalloc(1);
    auto evaled = program->operator()({angles[i]});
    accelerator->execute(buffer, evaled);
    const double expectedResult = 1.0 - 2.0 * std::sin(angles[i]/2.0) * std::sin(angles[i]/2.0);
    std::cout << "Angle = " << angles[i]
              << "; result = " << buffer->getExpectationValueZ() << " vs expected = " << expectedResult << "\n";
    EXPECT_NEAR(buffer->getExpectationValueZ(), expectedResult, 1e-6);
  }
}

TEST(QlmAcceleratorTester, testControlGate) {
  auto accelerator = xacc::getAccelerator("atos-qlm");
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program = xasmCompiler
                      ->compile(R"(__qpu__ void rotationControl(qbit q, double theta) {
      X(q[0]);
      H(q[1]);
      CRZ(q[0], q[1], theta);
      H(q[1]);
      Measure(q[1]);
    })",
                                accelerator)
                      ->getComposites()[0];

  const auto angles = xacc::linspace(-xacc::constants::pi, xacc::constants::pi, 20);
  for (size_t i = 0; i < angles.size(); ++i) {
    auto buffer = xacc::qalloc(2);
    auto evaled = program->operator()({angles[i]});
    accelerator->execute(buffer, evaled);
    const double expectedResult = 1.0 - 2.0 * std::sin(angles[i]/2.0) * std::sin(angles[i]/2.0);
    std::cout << "Angle = " << angles[i]
              << "; result = " << buffer->getExpectationValueZ() << " vs expected = " << expectedResult << "\n";
    EXPECT_NEAR(buffer->getExpectationValueZ(), expectedResult, 1e-6);
  }
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();
  xacc::Finalize();
  return result;
}
