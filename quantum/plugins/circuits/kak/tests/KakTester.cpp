#include "xacc.hpp"
#include <gtest/gtest.h>
#include "Circuit.hpp"
#include "xacc_service.hpp"
#include <Eigen/Dense>

using namespace xacc;
namespace {
constexpr std::complex<double> I { 0.0, 1.0 };
}

TEST(KakTester, checkSimple) 
{
  // Unitary matrix
  Eigen::Matrix4cd m;
  m << 1, 1 , 1, 1,
      1, I, -1, -I,
      1, -1, 1, -1,
      1, -I, -1, I;
  m = 0.5 * m;
  
  auto tmp = xacc::getService<Instruction>("kak");
  auto kak = std::dynamic_pointer_cast<quantum::Circuit>(tmp);
  kak->expand({ 
    std::make_pair("unitary", m)
  });
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
