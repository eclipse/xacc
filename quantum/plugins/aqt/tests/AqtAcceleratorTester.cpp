#include <gtest/gtest.h>
#include "xacc.hpp"
#include "Optimizer.hpp"
#include "xacc_service.hpp"
#include "Algorithm.hpp"
#include "xacc_observable.hpp"

namespace {
const std::string YAML_DIR = std::string(RESOURCE_DIR);
}

TEST(AqtAcceleratorTester, testBell) {
  auto accelerator =
      xacc::getAccelerator("aqt", {{"var-yaml", YAML_DIR + "/Variables.yaml"},
                                   {"pulse-yaml", YAML_DIR + "/Pulses.yaml"},
                                   {"adc-yaml", YAML_DIR + "/ADC.yaml"},
                                   {"dac-yaml", YAML_DIR + "/DAC.yaml"}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program = xasmCompiler
                     ->compile(R"(__qpu__ void bell(qbit q) {
      H(q[0]);
      Measure(q[0]);
    })",
                               accelerator)
                     ->getComposites()[0];

  auto buffer = xacc::qalloc(1);
  accelerator->execute(buffer, program);
  buffer->print();
  // The result will always be zero (default shots = 1024)
  EXPECT_EQ(buffer->getMeasurementCounts()["0"], 1024);
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();
  xacc::Finalize();
  return result;
}
