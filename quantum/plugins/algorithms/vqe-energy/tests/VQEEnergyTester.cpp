#include <gtest/gtest.h>

#include "XACC.hpp"
#include "xacc_service.hpp"
#include "Optimizer.hpp"
#include "Observable.hpp"
#include "Algorithm.hpp"
#include "PauliOperator.hpp"

using namespace xacc;
const std::string ansatz = R"a(def f(buffer, theta):
    X(0)
    Ry(theta, 1)
    CNOT(1,0)
    )a";

TEST(VQEEnergyTester, checkSimple) {
  if (xacc::hasAccelerator("tnqvm") && xacc::hasCompiler("xacc-py")) {
    auto acc = xacc::getAccelerator("tnqvm");
    auto buffer = acc->createBuffer("q", 2);

    auto compiler = xacc::getService<xacc::Compiler>("xacc-py");

    auto ir = compiler->compile(ansatz, nullptr);
    auto ruccsd = ir->getKernel("f");

    auto optimizer = xacc::getOptimizer("nlopt");
    xacc::quantum::PauliOperator observable;
    observable.fromString("(-6.125,0) Z1 + (0.21829,0) Z0 + (-2.1433,0) Y0 Y1 + (5.90671,0) + (-2.1433,0) X0 X1");

    auto vqe_energy = xacc::getService<Algorithm>("vqe-energy");
    EXPECT_TRUE(vqe_energy->initialize({{"ansatz",ruccsd}, {"accelerator",acc}, {"observable", &observable},{"initial-parameters", {0.5}}}));//
    vqe_energy->execute(buffer);
    EXPECT_NEAR(-1.74916, mpark::get<double>(buffer->getInformation("opt-val")), 1e-4);
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
