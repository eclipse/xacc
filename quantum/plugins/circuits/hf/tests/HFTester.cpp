#include "xacc.hpp"
#include <gtest/gtest.h>
#include "Circuit.hpp"
#include "xacc_service.hpp"

using namespace xacc;

TEST(HFTester, checkSimple) {

  auto hf_jw = std::dynamic_pointer_cast<quantum::Circuit>(
      xacc::getService<Instruction>("hf"));
  HeterogeneousMap options_jw = {{"ne", 2}, {"nq", 4}, {"transform", "jw"}};
  EXPECT_TRUE(hf_jw->expand(options_jw));

  auto hf_bk = std::dynamic_pointer_cast<quantum::Circuit>(
      xacc::getService<Instruction>("hf"));
  HeterogeneousMap options_bk = {{"ne", 2}, {"nq", 4}, {"transform", "bk"}};
  EXPECT_TRUE(hf_bk->expand(options_bk));

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
