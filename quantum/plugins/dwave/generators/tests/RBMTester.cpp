#include "xacc.hpp"
#include <gtest/gtest.h>
#include "xacc_service.hpp"
#include "AnnealingProgram.hpp"

using namespace xacc;

TEST(RBMTester, checkSimple) {
 auto rbm =std::dynamic_pointer_cast<quantum::AnnealingProgram>(
      xacc::getService<Instruction>("rbm"));

  EXPECT_TRUE(rbm->expand(
      {std::make_pair("nv", 4), std::make_pair("nh", 4)}));

  std::cout << "F:\n" << rbm->toString() << "\n";
  EXPECT_EQ(4+4+16, rbm->nInstructions());

}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
