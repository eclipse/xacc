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

int main(int argc, char **argv) {
  xacc::Initialize();

  ::testing::InitGoogleTest(&argc, argv);
  const auto result = RUN_ALL_TESTS();

  xacc::Finalize();

  return result;
}