#include "XACC.hpp"
#include <gtest/gtest.h>

using namespace xacc;

TEST(ExpTester, checkSimple) {

  // NOW Test it somehow...
  auto exp = xacc::getIRGenerator("range");
  auto f =
      exp->generate({{"gate","H"}, {"nqubits",10}});

  std::cout << "F:\n" << f->toString() << "\n";
  EXPECT_EQ(10, f->nInstructions());

  auto g = exp->generate({{"gate","H"}, {"start",3},{"end",10}});
  std::cout << "G:\n" << g->toString() << "\n";
  EXPECT_EQ(7, g->nInstructions());
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
