#include "xacc.hpp"
#include <gtest/gtest.h>
#include "Circuit.hpp"
#include "xacc_service.hpp"
#include <Eigen/Dense>

using namespace xacc;
namespace {
constexpr std::complex<double> I { 0.0, 1.0 };

Eigen::Matrix4cd generateRandomUnitary()
{
  // Using QR decomposition to generate a random unitary
  Eigen::Matrix4cd mat = Eigen::Matrix4cd::Random();
  auto QR = mat.householderQr();
  Eigen::Matrix4cd qMat = QR.householderQ() * Eigen::Matrix4cd::Identity();
  return qMat;
}
}

TEST(KakTester, checkSimple) 
{
  const int nbTests = 1000;
  for (int i = 0; i < nbTests; ++i)
  {
    auto unitaryMat = generateRandomUnitary();
    auto tmp = xacc::getService<Instruction>("kak");
    auto kak = std::dynamic_pointer_cast<quantum::Circuit>(tmp);
    const bool expandOk = kak->expand({ 
      std::make_pair("unitary", unitaryMat)
    });
    EXPECT_TRUE(expandOk);
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
