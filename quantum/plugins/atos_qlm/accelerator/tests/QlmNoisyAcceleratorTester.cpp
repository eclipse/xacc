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

TEST(QlmAcceleratorTester, testParametricGate) {
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

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();
  xacc::Finalize();
  return result;
}
