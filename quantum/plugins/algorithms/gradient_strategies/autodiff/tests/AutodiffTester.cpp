#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "Autodiff.hpp"
#include <random>

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

TEST(AutodiffTester, testVqeDefaultGradient) {
  // Use Qpp accelerator
  auto accelerator = xacc::getAccelerator("qpp");
  // Create the N=2 deuteron Hamiltonian
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));

  // Use a gradient-based optimizer
  auto optimizer = xacc::getOptimizer("mlpack");
  EXPECT_TRUE(optimizer->isGradientBased());
  xacc::qasm(R"(
        .compiler xasm
        .circuit deuteron_ansatz_h2
        .parameters theta
        .qbit q
        X(q[0]);
        Ry(q[1], theta);
        CNOT(q[1],q[0]);
    )");
  auto ansatz = xacc::getCompiled("deuteron_ansatz_h2");

  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe");
  vqe->initialize({{"ansatz", ansatz},
                   {"observable", H_N_2},
                   {"accelerator", accelerator},
                   {"optimizer", optimizer}});

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  vqe->execute(buffer);
  std::cout << "Energy: " << (*buffer)["opt-val"].as<double>() << "\n";
  // Expected result: -1.74886
  EXPECT_NEAR((*buffer)["opt-val"].as<double>(), -1.74886, 0.1);
}

TEST(AutodiffTester, checkQaoaMaxCutGradient) {
  auto acc = xacc::getAccelerator("qpp");
  auto buffer = xacc::qalloc(3);
  const int nbParams = 2;
  std::vector<double> initialParams;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis(-2.0, 2.0);
  // Init random parameters
  for (int i = 0; i < nbParams; ++i) {
    initialParams.emplace_back(dis(gen));
  }

  auto optimizer =
      xacc::getOptimizer("nlopt", {{"algorithm", "l-bfgs"},
                                   {"maximize", true},
                                   {"initial-parameters", initialParams}});
  EXPECT_TRUE(optimizer->isGradientBased());
  auto qaoa = xacc::getAlgorithm("maxcut-qaoa");
  auto graph = xacc::getService<xacc::Graph>("boost-digraph");

  // Triangle graph
  for (int i = 0; i < 3; i++) {
    graph->addVertex();
  }
  graph->addEdge(0, 1);
  graph->addEdge(0, 2);
  graph->addEdge(1, 2);

  const bool initOk = qaoa->initialize({{"accelerator", acc},
                                        {"optimizer", optimizer},
                                        {"graph", graph},
                                        // number of time steps (p) param
                                        {"steps", 1},
                                        // "Standard" or "Extended"
                                        {"parameter-scheme", "Standard"}});
  qaoa->execute(buffer);
  std::cout << "Min Val: " << (*buffer)["opt-val"].as<double>() << "\n";
  EXPECT_NEAR((*buffer)["opt-val"].as<double>(), 2.0, 0.1);
}

TEST(AutodiffTester, checkGradientKernelEvaluator) {
  // Create the N=3 deuteron Hamiltonian
  auto H_N_3 = xacc::quantum::getObservable(
      "pauli",
      std::string("5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1 + "
                  "9.625 - 9.625 Z2 - 3.91 X1 X2 - 3.91 Y1 Y2"));

  // JIT map Quil QASM Ansatz to IR
  xacc::qasm(R"(
.compiler xasm
.circuit ansatz_h3_test
.parameters t0, t1
.qbit q
X(q[0]);
exp_i_theta(q, t0, {{"pauli", "X0 Y1 - Y0 X1"}});
exp_i_theta(q, t1, {{"pauli", "X0 Z1 Y2 - X2 Z1 Y0"}});
)");
  auto ansatz = xacc::getCompiled("ansatz_h3_test");
  // Create a kernel evaluator
  std::function<std::shared_ptr<xacc::CompositeInstruction>(
      std::vector<double>)>
      kernel_evaluator =
          [&](std::vector<double> x) { return ansatz->operator()(x); };

  auto autodiff = std::make_shared<xacc::quantum::Autodiff>();
  autodiff->initialize(
      {{"observable", H_N_3}, {"kernel-evaluator", kernel_evaluator}});

  const std::vector<double> initialParams{0.0, 0.0};
  const int nbIterms = 100;
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
    autodiff->getGradientExecutions(kernel_evaluator(currentParams),
                                    currentParams);
    autodiff->compute(grad, {});
    EXPECT_EQ(grad.size(), 2);
    autodiff->derivative(kernel_evaluator(currentParams), {}, &energy);
    std::cout << "Energy: " << energy << "; Grads = ";
    for (const auto &g : grad) {
      std::cout << g << " ";
    }
    std::cout << "\n";
  }
  std::cout << "Energy: " << energy << "\n";
  EXPECT_NEAR(energy, -2.044, 0.1);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}