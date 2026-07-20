#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include <dlfcn.h>
#include <pybind11/embed.h>

using namespace xacc;
namespace py = pybind11;

TEST(ScipyOptimizerTester, checkSimple) {

  auto optimizer =
      xacc::getService<Optimizer>("scipy");

  OptFunction f([](const std::vector<double> &x,
                   std::vector<double> &g) { return x[0] * x[0] + 5; },
                1);

  EXPECT_EQ(optimizer->name(), "scipy");
  EXPECT_EQ(1, f.dimensions());

  optimizer->setOptions(HeterogeneousMap{std::make_pair("maxeval", 20)});

  auto result = optimizer->optimize(f);
  EXPECT_NEAR(5.0, result.first, 1.0e-6);
  EXPECT_NEAR(result.second[0], 0.0, 1.0e-6);
}

TEST(ScipyOptimizerTester, checkGradient) {

  auto optimizer = xacc::getService<Optimizer>("scipy");

  OptFunction f(
      [](const std::vector<double> &x, std::vector<double> &grad) {
        if (!grad.empty()) {
          std::cout << "GRAD\n";
          grad[0] = 2. * x[0];
        }
        auto xx = x[0] * x[0] + 5;
        std::cout << xx << "\n";
        return xx;
      },
      1);

  EXPECT_EQ(1, f.dimensions());

  optimizer->setOptions(HeterogeneousMap{
      std::make_pair("maxeval", 20),
      std::make_pair("initial-parameters", std::vector<double>{1.0}),
      std::make_pair("optimizer", "bfgs")});

  auto result = optimizer->optimize(f);

  EXPECT_NEAR(result.first, 5.0, 1e-4);
  EXPECT_NEAR(result.second[0], 0.0, 1e-4);
}

TEST(ScipyOptimizerTester, checkGradientRosenbrock) {

  auto optimizer = xacc::getService<Optimizer>("scipy");

  OptFunction f(
      [](const std::vector<double> &x, std::vector<double> &grad) {
        if (!grad.empty()) {
          //   std::cout << "GRAD\n";
          grad[0] = -2 * (1 - x[0]) + 400 * (std::pow(x[0], 3) - x[1] * x[0]);
          grad[1] = 200 * (x[1] - std::pow(x[0], 2));
        }
        auto xx =
            100 * std::pow(x[1] - std::pow(x[0], 2), 2) + std::pow(1 - x[0], 2);
        std::cout << xx << ", " << x << ", " << grad << "\n";

        return xx;
      },
      2);

  EXPECT_EQ(2, f.dimensions());

  optimizer->setOptions(HeterogeneousMap{std::make_pair("maxeval", 200),
                                         std::make_pair("optimizer", "bfgs")});

  auto result = optimizer->optimize(f);

  EXPECT_NEAR(result.first, 0.0, 1e-4);
  EXPECT_NEAR(result.second[0], 1.0, 1e-4);
  EXPECT_NEAR(result.second[1], 1.0, 1e-4);
}

int main(int argc, char **argv) {
  dlopen("libpython3.8.so", RTLD_LAZY | RTLD_GLOBAL);
  xacc::Initialize(argc, argv);
  py::initialize_interpreter();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  py::finalize_interpreter();
  xacc::Finalize();
  return ret;
}
