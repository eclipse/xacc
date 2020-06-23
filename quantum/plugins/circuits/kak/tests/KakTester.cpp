#include "xacc.hpp"
#include <gtest/gtest.h>
#include "Circuit.hpp"
#include "xacc_service.hpp"

using namespace xacc;

TEST(KakTester, checkSimple) 
{
  auto tmp = xacc::getService<Instruction>("kak");
  auto kak = std::dynamic_pointer_cast<quantum::Circuit>(tmp);
  kak->expand({ });
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
