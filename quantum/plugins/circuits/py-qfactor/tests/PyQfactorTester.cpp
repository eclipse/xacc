#include "xacc.hpp"
#include <gtest/gtest.h>
#include "Circuit.hpp"
#include "Optimizer.hpp"
#include "xacc_observable.hpp"
#include "xacc_service.hpp"
#include <Eigen/Dense>
using namespace xacc;

TEST(QfactorTester, checkDefault) {
  Eigen::MatrixXcd ccnotMat = Eigen::MatrixXcd::Identity(8, 8);
  ccnotMat(6, 6) = 0.0;
  ccnotMat(7, 7) = 0.0;
  ccnotMat(6, 7) = 1.0;
  ccnotMat(7, 6) = 1.0;
  auto qfactor_circuit =
      xacc::createComposite("qfactor", {{"unitary", ccnotMat}});
  EXPECT_TRUE(qfactor_circuit->nInstructions() > 0);
  std::cout << "HOWDY:\n" << qfactor_circuit->toString() << "\n";
  auto provider = xacc::getIRProvider("quantum");

  for (std::size_t i = 0; i < 3; i++) {
    qfactor_circuit->addInstruction(
        provider->createInstruction("Measure", std::vector<std::size_t>{i}));
  }

  for (std::size_t i = 0; i < 3; i++) {
    qfactor_circuit->insertInstruction(
        0, provider->createInstruction("X", std::vector<std::size_t>{i}));
  }

  auto qpp = xacc::getAccelerator("qpp", {{"shots", 1024}});
  auto buffer = xacc::qalloc(3);
  qpp->execute(buffer, qfactor_circuit);
  buffer->print();

  auto count = buffer->getMeasurementCounts()["110"];

  EXPECT_TRUE(count == 1024);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  xacc::set_verbose(true);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}