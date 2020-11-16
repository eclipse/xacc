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

TEST(KakTester, checkEdgeCase) 
{
  Eigen::Matrix4cd unitaryMat;
  unitaryMat << 1, 0, 0, 0, 
                0, 2.67948963e-08, -1, 0, 
                0, 1, 2.67948965e-08, 0, 
                0, 0, 0, 1;
  auto tmp = xacc::getService<Instruction>("kak");
  auto kak = std::dynamic_pointer_cast<quantum::Circuit>(tmp);
  const bool expandOk = kak->expand({std::make_pair("unitary", unitaryMat)});
  std::cout << kak->toString() << "\n";
  EXPECT_TRUE(expandOk);
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
  const auto runTestCase = [](bool in_bit0, bool in_bit1) {
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
    if (in_bit0)
    {
      kak->insertInstruction(0, xGate0);
    }
    if (in_bit1)
    {
      kak->insertInstruction(0, xGate1);
    }

    kak->addInstructions({ measureGate0, measureGate1 });
    auto buffer = xacc::qalloc(2);
    acc->execute(buffer, kak);
    buffer->print();
    // Calculate the expected bitstring
    const std::string ctrlBit = in_bit0 ? "1" : "0";
    const bool targetVal = in_bit0 ? !in_bit1 : in_bit1;
    const std::string targetBit = targetVal ? "1" : "0";
    const std::string expectedBitString = ctrlBit + targetBit;
    EXPECT_NEAR(buffer->computeMeasurementProbability(expectedBitString), 1.0, 1e-12);
  };
  
  // Run all binary combinations:
  runTestCase(false, false);
  runTestCase(false, true);
  runTestCase(true, false);
  runTestCase(true, true);
}

TEST(KakTester, checkTomographyCNOT)
{
  auto acc = xacc::getAccelerator("qpp", { std::make_pair("shots", 4096) });
  auto buffer = xacc::qalloc(2);
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

  auto qpt = xacc::getService<Algorithm>("qpt");
  EXPECT_TRUE(qpt->initialize({ 
    std::make_pair("circuit", std::static_pointer_cast<CompositeInstruction>(kak)), 
    std::make_pair("accelerator", acc)}));
  qpt->execute(buffer);

  const std::vector<double> true_cx_chi {
    1., 0., 0., 1., 1., 0., 0., -1., 0., 0., 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 1., 0., 0.,
    1., 1., 0., 0., -1., 0., 0., 0., 0., 0., 0., 0., 0., 1., 0., 0., 1., 1.,
    0., 0., -1., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., -1., 0., 0., -1., -1., 0., 0., 1., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
    0., 0., 0., 0., 0., 0., 0., 0.};
  const double fidelity = qpt->calculate("fidelity", buffer, {
    std::make_pair("chi-theoretical-real", true_cx_chi) });

  std::cout << "Fidelity: " <<  fidelity << "\n";
  EXPECT_NEAR(1.0, fidelity, 0.1);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
