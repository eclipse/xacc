
#include <memory>
#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"

using namespace xacc;

// Uncomment this test to test TriQ with remote IBM backend
// It will query information directly from the backend.
// TEST(TriQPlacementTester, checkRemote) {
//   auto irt = xacc::getIRTransformation("triQ");
//   auto compiler = xacc::getCompiler("staq");
//   auto accelerator = xacc::getAccelerator("ibm:ibmq_16_melbourne");
//   auto q = xacc::qalloc(5);
//   q->setName("q");
//   xacc::storeBuffer(q);
//   auto IR = compiler->compile(R"(__qpu__ void f(qreg q) {
//                     OPENQASM 2.0;
//                     include "qelib1.inc";
//                     creg c[16];
//                     cx q[2],q[1];
//                     cx q[1],q[2];
//                     cx q[3],q[2];
//                     cx q[2],q[3];
//                     cx q[4],q[3];
//                     cx q[3],q[4];
//                     h q[0];
//                     t q[4];
//                     t q[3];
//                     t q[0];
//                     cx q[3],q[4];
//                     cx q[0],q[3];
//                     cx q[4],q[0];
//                     tdg q[3];
//                     })");

//   auto program = IR->getComposites()[0];

//   irt->apply(program, accelerator);
//   std::cout << "HOWDY: \n" << program->toString() << "\n";
// }

TEST(TriQPlacementTester, checkSimple) {
  const std::string NOISE_MODEL_JSON_FILE =
      std::string(RESOURCE_DIR) + "/noise.json";
  std::ifstream noiseModelFile;
  noiseModelFile.open(NOISE_MODEL_JSON_FILE);
  std::stringstream noiseStrStream;
  noiseStrStream << noiseModelFile.rdbuf();
  const std::string noiseJsonStr = noiseStrStream.str();
  auto accelerator = xacc::getAccelerator(
      "aer", {{"noise-model", noiseJsonStr}, {"shots", 8192}});
  // Allocate some qubits
  auto buffer = xacc::qalloc(3);
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void bell(qbit q) {
      H(q[0]);
      CX(q[0], q[1]);
      CX(q[0], q[2]);
      Measure(q[0]);
      Measure(q[1]);
      Measure(q[2]);
})",
                                  accelerator);
  auto program = ir->getComposites()[0];
  auto irt = xacc::getIRTransformation("triQ");
  const std::string BACKEND_JSON_FILE =
      std::string(RESOURCE_DIR) + "/backend.json";
  std::ifstream inFile;
  inFile.open(BACKEND_JSON_FILE);
  std::stringstream strStream;
  strStream << inFile.rdbuf();
  const std::string jsonStr = strStream.str();
  irt->apply(program, accelerator, {{"backend-json", jsonStr}});
  std::cout << "HOWDY: \n" << program->toString() << "\n";
  for (size_t instIdx = 0; instIdx < program->nInstructions(); ++instIdx) {
    auto instPtr = program->getInstruction(instIdx);
    // Check routing:
    if (instPtr->bits().size() == 2) {
      // The IBM Melbourne device doesn't have q0-q2 connection,
      // hence checking that the transformation honors that.
      const size_t q0 = instPtr->bits()[0];
      const size_t q1 = instPtr->bits()[1];
      EXPECT_NE(q0, q1);
      if (q0 == 0) {
        EXPECT_NE(q1, 2);
      }
      if (q0 == 2) {
        EXPECT_NE(q1, 0);
      }
    }
  }
  accelerator->execute(buffer, program);
  buffer->print();
  // Validate that the expected GHZ state:
  EXPECT_GT(buffer->computeMeasurementProbability("000"), 0.35);
  EXPECT_GT(buffer->computeMeasurementProbability("111"), 0.35);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
