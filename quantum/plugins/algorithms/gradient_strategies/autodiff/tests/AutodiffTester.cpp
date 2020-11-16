#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "Autodiff.hpp"

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
  auto autodiff = std::make_shared<xacc::quantum::Autodiff>();
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
  for (const auto &angle : xacc::linspace(-M_PI, M_PI, 20)) {
    const std::vector<double> params{angle};
    // Autodiff:
    double adEnergy = 0.0;
    auto grad = autodiff->derivative(ansatz, params, &adEnergy);
    // VQE:
    auto energy = vqe->execute(buffer, params);
    EXPECT_NEAR(energy[0], adEnergy, 1e-3);
  }
}

TEST(AutodiffTester, checkGates) {
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));
  // JIT map Quil QASM Ansatz to IR
  xacc::qasm(R"(
.compiler quil
.circuit test1
.parameters theta0, theta1
X 0
H 1
Ry(theta0) 1
Rx(theta1) 0
CNOT 1 0
)");
  auto ansatz = xacc::getCompiled("test1");
  auto autodiff = std::make_shared<xacc::quantum::Autodiff>();
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
  for (const auto &angle1 : xacc::linspace(-M_PI, M_PI, 6)) {
    for (const auto &angle2 : xacc::linspace(-M_PI, M_PI, 6)) {
      const std::vector<double> params{angle1, angle2};
      // Autodiff:
      double adEnergy = 0.0;
      auto grad = autodiff->derivative(ansatz, params, &adEnergy);
      // VQE:
      auto energy = vqe->execute(buffer, params);
      std::cout << "(" << angle1 << ", " << angle2 << "): " << adEnergy
                << " vs " << energy[0] << "\n";
      EXPECT_NEAR(energy[0], adEnergy, 1e-3);
    }
  }
}

// Test a simple gradient-descent using autodiff gradient value:
TEST(AutodiffTester, checkGradient) {
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));
  // JIT map Quil QASM Ansatz to IR
  xacc::qasm(R"(
.compiler quil
.circuit ansatz
.parameters theta
X 0
Ry(theta) 1
CNOT 1 0
)");
  auto ansatz = xacc::getCompiled("ansatz");
  auto autodiff = std::make_shared<xacc::quantum::Autodiff>();
  autodiff->fromObservable(H_N_2);
  const double initialParam = 0.0;
  const int nbIterms = 200;
  // gradient-descent step size
  const double stepSize = 0.01;
  double grad = 0.0;
  double currentParam = initialParam;
  double energy = 0.0;
  for (int i = 0; i < nbIterms; ++i) {
    currentParam = currentParam - stepSize * grad;
    const std::vector<double> newParams{currentParam};
    auto grads = autodiff->derivative(ansatz, newParams);
    EXPECT_EQ(grads.size(), 1);
    grad = grads[0];
  }

  EXPECT_NEAR(currentParam, 0.594, 1e-3);
  double adEnergy;
  autodiff->derivative(ansatz, {currentParam}, &adEnergy);
  EXPECT_NEAR(adEnergy, -1.74886, 1e-3);
}

// Test a more complicated case:
TEST(AutodiffTester, checkGradientH3) {
  // Create the N=3 deuteron Hamiltonian
  auto H_N_3 = xacc::quantum::getObservable(
      "pauli",
      std::string("5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1 + "
                  "9.625 - 9.625 Z2 - 3.91 X1 X2 - 3.91 Y1 Y2"));

  // JIT map Quil QASM Ansatz to IR
  xacc::qasm(R"(
.compiler xasm
.circuit ansatz_h3
.parameters t0, t1
.qbit q
X(q[0]);
exp_i_theta(q, t0, {{"pauli", "X0 Y1 - Y0 X1"}});
exp_i_theta(q, t1, {{"pauli", "X0 Z1 Y2 - X2 Z1 Y0"}});
)");
  auto ansatz = xacc::getCompiled("ansatz_h3");
  auto autodiff = std::make_shared<xacc::quantum::Autodiff>();
  autodiff->fromObservable(H_N_3);
  const std::vector<double> initialParams{0.0, 0.0};
  const int nbIterms = 200;
  // gradient-descent step size
  const double stepSize = 0.01;
  std::vector<double> grad{0.0, 0.0};
  auto currentParams = initialParams;
  double energy = 0.0;
  for (int i = 0; i < nbIterms; ++i) {
    for (int paramId = 0; paramId < 2; ++paramId) {
      currentParams[paramId] =
          currentParams[paramId] - stepSize * grad[paramId];
    }
    grad = autodiff->derivative(ansatz, currentParams, &energy);
    EXPECT_EQ(grad.size(), 2);
    std::cout << "Energy: " << energy << "; Grads = ";
    for (const auto &g : grad) {
      std::cout << g << " ";
    }
    std::cout << "\n";
  }

  EXPECT_NEAR(energy, -2.04482, 1e-3);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}