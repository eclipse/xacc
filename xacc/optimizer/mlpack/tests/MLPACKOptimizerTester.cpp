#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
// #include "nlopt_optimizer.hpp"

using namespace xacc;

TEST(MLPACKOptimizerTester, checkSimple) {

  auto optimizer =
      xacc::getService<Optimizer>("mlpack"); // NLOptimizer optimizer;

  OptFunction f([](const std::vector<double> &x, std::vector<double>& g) { return x[0] * x[0] + 5; },
                1);

  EXPECT_EQ(1, f.dimensions());

  auto result = optimizer->optimize(f);

  EXPECT_EQ(5.0, result.first);
  EXPECT_EQ(result.second, std::vector<double>{0.0});
}

TEST(MLPACKOptimizerTester, checkGradient) {

  auto optimizer =
      xacc::getService<Optimizer>("mlpack"); // NLOptimizer optimizer;

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

  auto result = optimizer->optimize(f);

  EXPECT_EQ(result.first, 5.0);
  EXPECT_EQ(result.second, std::vector<double>{0.0});
}



TEST(NLOptimizerTester, checkGradientRosenbrock) {

  auto optimizer =
      xacc::getService<Optimizer>("mlpack"); // NLOptimizer optimizer;

  OptFunction f(
      [](const std::vector<double> &x, std::vector<double> &grad) {
        if (!grad.empty()) {
        //   std::cout << "GRAD\n";
          grad[0] = -2 * (1 - x[0]) + 400 * (std::pow(x[0], 3) - x[1] * x[0]);
          grad[1] = 200 * (x[1] - std::pow(x[0],2));
        }
        auto xx = 100 * std::pow(x[1] - std::pow(x[0], 2), 2) + std::pow(1 - x[0], 2);
        std::cout << xx << ", " << x << "\n";
        return xx;
      },
      2);

  EXPECT_EQ(2, f.dimensions());

  optimizer->setOptions(
      HeterogeneousMap{std::make_pair("nlopt-maxeval", 200),
                       std::make_pair("mlpack-optimizer", "l-bfgs")});

  auto result = optimizer->optimize(f);

  EXPECT_NEAR(result.first, 0.0, 1e-4);
  EXPECT_NEAR(result.second[0], 1.0, 1e-4);
  EXPECT_NEAR(result.second[1], 1.0, 1e-4);

}
int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
