#include <gtest/gtest.h>
#include "xacc.hpp"
#include "Optimizer.hpp"
#include "xacc_service.hpp"
#include "Algorithm.hpp"
#include "xacc_observable.hpp"

TEST(QlmAcceleratorTester, testIR) {
  auto accelerator = xacc::getAccelerator("atos-qlm", {{"shots", 1024}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program1 = xasmCompiler
                      ->compile(R"(__qpu__ void test1(qbit q) {
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

// TEST(QlmAcceleratorTester, testBell) {
//   auto accelerator = xacc::getAccelerator("atos-qlm", {{"shots", 1024}});
//   auto xasmCompiler = xacc::getCompiler("xasm");
//   auto ir = xasmCompiler->compile(R"(__qpu__ void bell(qbit q) {
//       H(q[0]);
//       CX(q[0], q[1]);
//       CX(q[1], q[2]);
//       Measure(q[0]);
//       Measure(q[1]);
//       Measure(q[2]);
//     })",
//                                   accelerator);

//   auto program = ir->getComposite("bell");
//   auto buffer = xacc::qalloc(3);
//   accelerator->execute(buffer, program);
//   EXPECT_EQ(buffer->getMeasurementCounts().size(), 2);
//   auto prob0 = buffer->computeMeasurementProbability("000");
//   auto prob1 = buffer->computeMeasurementProbability("111");
//   buffer->print();
//   EXPECT_NEAR(prob0 + prob1, 1.0, 1e-9);
//   EXPECT_NEAR(prob0, 0.5, 0.2);
//   EXPECT_NEAR(prob1, 0.5, 0.2);
// }

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();
  xacc::Finalize();
  return result;
}
