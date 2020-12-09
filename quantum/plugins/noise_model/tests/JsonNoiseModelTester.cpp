#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "NoiseModel.hpp"

TEST(JsonNoiseModelTester, checkSimple) {
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}