#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"

TEST(AutodiffTester, checkExpValCalc) {
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));
  // JIT map Quil QASM Ansatz to IR
  xacc::qasm(R"(
.compiler quil
.circuit deuteron_ansatz
.parameters theta
X 0
Ry(theta) 1
CNOT 1 0
)");
  auto ansatz = xacc::getCompiled("deuteron_ansatz");
  auto autodiff = xacc::getService<xacc::Differentiable>("autodiff");
  autodiff->fromObservable(H_N_2);
  // Use VQE to compute the expectation:
  auto vqe = xacc::getService<xacc::Algorithm>("vqe");
  auto acc = xacc::getAccelerator("qpp", {std::make_pair("vqe-mode", true)});
  auto buffer = xacc::qalloc(2);
  auto optimizer = xacc::getOptimizer("nlopt");
  vqe->initialize({{"ansatz", ansatz},
                   {"accelerator", acc},
                   {"observable", H_N_2},
                   {"optimizer", optimizer}});
  for (const auto& angle: xacc::linspace(-M_PI, M_PI, 20)) {
    const std::vector<double> params{ angle };
    // Autodiff:
    auto result = autodiff->derivative(ansatz, params);
    // VQE:
    auto energy = vqe->execute(buffer, params);
    EXPECT_NEAR(energy[0], result.first, 1e-3);
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}