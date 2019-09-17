#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "nlopt_optimizer.hpp"

using namespace xacc;

TEST(NLOptimizerTester, checkSimple) {

  auto optimizer =
      xacc::getService<Optimizer>("nlopt"); // NLOptimizer optimizer;

  OptFunction f([](const std::vector<double> &x, std::vector<double>& g) { return x[0] * x[0] + 5; },
                1);

  EXPECT_EQ(1, f.dimensions());

  optimizer->setOptions(HeterogeneousMap{std::make_pair("nlopt-maxeval", 20)});

  auto result = optimizer->optimize(f);

  EXPECT_EQ(5.0, result.first);
  EXPECT_EQ(result.second, std::vector<double>{0.0});
}

TEST(NLOptimizerTester, checkGradient) {

  auto optimizer =
      xacc::getService<Optimizer>("nlopt"); // NLOptimizer optimizer;

  OptFunction f(
      [](const std::vector<double> &x, std::vector<double> &grad) {
        if (!grad.empty()) {
          std::cout << "GRAD\n";
          grad[0] = 2. * x[0];
        }
        return x[0] * x[0] + 5;
      },
      1);

  EXPECT_EQ(1, f.dimensions());

  optimizer->setOptions(
      HeterogeneousMap{std::make_pair("nlopt-maxeval", 20),
                       std::make_pair("nlopt-optimizer", "l-bfgs")});

  auto result = optimizer->optimize(f);

  EXPECT_EQ(result.first, 5.0);
  EXPECT_EQ(result.second, std::vector<double>{0.0});
}
int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
