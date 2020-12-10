#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "NoiseModel.hpp"

namespace {
// A sample Json for testing
const std::string test_json =
    R"({"gate_noise": [{"gate_name": "H", "register_location": ["0"], "noise_kraus_ops": [{"matrix": [[[0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0]]], "noise_qubits": ["0"]}, {"matrix": [[[0.0, 0.0], [0.05773502691896258, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0]]], "noise_qubits": ["0"]}, {"matrix": [[[0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.05773502691896258], [0.0, 0.0]]], "noise_qubits": ["0"]}, {"matrix": [[[0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.05773502691896258, 0.0]]], "noise_qubits": ["0"]}]}, {"gate_name": "X", "register_location": ["0"], "noise_kraus_ops": [{"matrix": [[[0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0]]], "noise_qubits": ["0"]}, {"matrix": [[[0.0, 0.0], [0.05773502691896258, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0]]], "noise_qubits": ["0"]}, {"matrix": [[[0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.05773502691896258], [0.0, 0.0]]], "noise_qubits": ["0"]}, {"matrix": [[[0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.05773502691896258, 0.0]]], "noise_qubits": ["0"]}]}], "bit_order": "MSB"})";
} // namespace

TEST(JsonNoiseModelTester, checkSimple) {
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", test_json}});
  const std::string ibmNoiseJson = noiseModel->toJson();
  std::cout << "IBM Equiv: \n" << ibmNoiseJson << "\n";
  auto accelerator = xacc::getAccelerator(
      "aer", {{"noise-model", ibmNoiseJson}, {"sim-type", "density_matrix"}});
  // auto accelerator = xacc::getAccelerator("aer", {{"shots", 8192}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program = xasmCompiler
                     ->compile(R"(__qpu__ void testX(qbit q) {
        X(q[0]);
        Measure(q[0]);
      })",
                               accelerator)
                     ->getComposite("testX");
  auto buffer = xacc::qalloc(1);
  accelerator->execute(buffer, program);
  buffer->print();
  auto densityMatrix = (*buffer)["density_matrix"].as<std::vector<std::pair<double, double>>>();
  EXPECT_EQ(densityMatrix.size(), 4);
  // Check trace
  EXPECT_NEAR(densityMatrix[0].first + densityMatrix[3].first, 1.0, 1e-6);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}