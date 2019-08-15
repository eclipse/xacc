#include "XACC.hpp"
#include <gtest/gtest.h>

using namespace xacc;

TEST(ExpTester, checkSimple) {

  // NOW Test it somehow...
  auto exp = xacc::getIRGenerator("exp_i_theta");
  auto f =
      exp->generate({{"pauli","Y0 X1 X2"}});

      std::cout << "F:\n" << f->toString() << "\n";
    f =
      exp->generate({{"pauli","X0 Y1 - X1 Y0"}});

      std::cout << "F:\n" << f->toString() << "\n";
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
//   xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
