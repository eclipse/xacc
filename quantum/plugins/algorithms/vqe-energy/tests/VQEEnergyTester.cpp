#include <gtest/gtest.h>

#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "Algorithm.hpp"
#include "PauliOperator.hpp"

using namespace xacc;
const std::string ansatz = R"ansatz(__qpu__ void f(qbit buffer, double theta) {
    X(buffer[0]);
    Ry(buffer[1], theta);
    CNOT(buffer[1],buffer[0]);
    })ansatz";

TEST(VQEEnergyTester, checkSimple) {
  if (xacc::hasAccelerator("tnqvm")) {
    auto acc = xacc::getAccelerator("tnqvm");
    auto buffer = xacc::qalloc(2);

    auto compiler = xacc::getService<xacc::Compiler>("xasm");

    auto ir = compiler->compile(ansatz, nullptr);
    auto a = ir->getComposite("f");

    auto optimizer = xacc::getOptimizer("nlopt");
    std::shared_ptr<Observable> observable = std::make_shared<xacc::quantum::PauliOperator>("(-6.125,0) Z1 + (0.21829,0) Z0 + (-2.1433,0) Y0 Y1 + (5.90671,0) + (-2.1433,0) X0 X1");

    auto vqe_energy = xacc::getService<Algorithm>("vqe-energy");
    EXPECT_TRUE(vqe_energy->initialize({std::make_pair("ansatz",a),
                                std::make_pair("accelerator",acc),
                                std::make_pair("observable", observable),
                                std::make_pair("parameters", std::vector<double>{0.594298})}));
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
