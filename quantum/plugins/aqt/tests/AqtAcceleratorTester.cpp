#include <gtest/gtest.h>
#include "xacc.hpp"
#include "Optimizer.hpp"
#include "xacc_service.hpp"
#include "Algorithm.hpp"
#include "xacc_observable.hpp"

TEST(AqtAcceleratorTester, testBell) {
  auto accelerator = xacc::getAccelerator(
      "aqt",
      {{"var-yaml",
        "/Users/7tn/dev/qtrl/examples/Config_Example/Variables.yaml"},
       {"pulse-yaml",
        "/Users/7tn/dev/qtrl/tests/ref_config/Pulses.yaml"},
       {"adc-yaml", "/Users/7tn/dev/qtrl/examples/Config_Example/ADC.yaml"},
       {"dac-yaml", "/Users/7tn/dev/qtrl/examples/Config_Example/DAC.yaml"}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program = xasmCompiler
                     ->compile(R"(__qpu__ void bell(qbit q) {
      H(q[0]);
      CX(q[0], q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })",
                               accelerator)
                     ->getComposites()[0];

  auto buffer = xacc::qalloc(2);
  accelerator->execute(buffer, program);
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();
  xacc::Finalize();
  return result;
}
