#include "xacc.hpp"
#include <gtest/gtest.h>
#include "xacc_service.hpp"
#include "NoiseModel.hpp"

// Note: this test can only be run if having IBMQ credential.
TEST(AerNoiseModelTester, checkSimple) {
  auto noiseModel = xacc::getService<xacc::NoiseModel>("IBM");
  noiseModel->initialize({{"backend", "ibmq_ourense"}});
  // There are 5 qubits.
  EXPECT_EQ(noiseModel->readoutErrors().size(), 5);
  for (const auto &[meas0Prep1, meas1Prep0] : noiseModel->readoutErrors()) {
    // Check readout error parsing
    EXPECT_GT(meas0Prep1, 0.0);
    EXPECT_LT(meas0Prep1, 0.1);
    EXPECT_GT(meas1Prep0, 0.0);
    EXPECT_LT(meas1Prep0, 0.1);
  }
}

TEST(AerNoiseModelTester, checkNoiseModelJson) {
  auto noiseModel = xacc::getService<xacc::NoiseModel>("IBM");
  noiseModel->initialize({{"backend", "ibmq_ourense"}});
  // There are 5 qubits.
  EXPECT_EQ(noiseModel->readoutErrors().size(), 5);
  const std::string noiseJson = noiseModel->toJson();
  std::cout << "JSON:\n" << noiseJson << "\n";

  // Load the *toJson()* noise model to Aer
  auto accelerator = xacc::getAccelerator(
      "aer", {{"noise-model", noiseJson}, {"shots", 8192}});

  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program = xasmCompiler
                     ->compile(R"(__qpu__ void testBell(qbit q) {
        H(q[0]);
        CNOT(q[0],q[1]);                
        Measure(q[0]);
        Measure(q[1]);
      })",
                               accelerator)
                     ->getComposite("testBell");

  auto buffer = xacc::qalloc(2);
  accelerator->execute(buffer, program);
  buffer->print();
  // Must have some errors
  EXPECT_GT(buffer->computeMeasurementProbability("10"), 0.01);
  EXPECT_GT(buffer->computeMeasurementProbability("01"), 0.01);
  // Majority is still Bell state:
  EXPECT_GT(buffer->computeMeasurementProbability("00"), 0.4);
  EXPECT_GT(buffer->computeMeasurementProbability("11"), 0.4);
}

int main(int argc, char **argv) {
  xacc::Initialize();

  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();

  xacc::Finalize();

  return result;
}