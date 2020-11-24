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

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();
  xacc::Finalize();
  return result;
}
