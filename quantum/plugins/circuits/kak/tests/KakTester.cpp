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

// Decompose CNOT
TEST(KakTester, checkCNOT) 
{
  Eigen::Matrix4cd cnotMat;
  cnotMat << 1, 0, 0, 0,
             0, 1, 0, 0,
             0, 0, 0, 1,
             0, 0, 1, 0;
  
  auto tmp = xacc::getService<Instruction>("kak");
  auto kak = std::dynamic_pointer_cast<quantum::Circuit>(tmp);
  const bool expandOk = kak->expand({ 
    std::make_pair("unitary", cnotMat)
  });
  EXPECT_TRUE(expandOk);
  auto acc = xacc::getAccelerator("qpp", { std::make_pair("shots", 1024)});
  auto gateRegistry = xacc::getService<IRProvider>("quantum");
  auto xGate0 = gateRegistry->createInstruction("X", { 0 });
  auto xGate1 = gateRegistry->createInstruction("X", { 1 });
  auto measureGate0 = gateRegistry->createInstruction("Measure", { 0 });
  auto measureGate1 = gateRegistry->createInstruction("Measure", { 1 });
  //kak->insertInstruction(0, xGate0);
  kak->insertInstruction(0, xGate1);
  kak->addInstructions({ measureGate0, measureGate1 });
  std::cout << "HOWDY: \n" << kak->toString() << "\n";
  auto buffer = xacc::qalloc(2);
  acc->execute(buffer, kak);
  buffer->print();
}
int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
