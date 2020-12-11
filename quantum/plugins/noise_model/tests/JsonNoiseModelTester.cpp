#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "NoiseModel.hpp"

namespace {
// A sample Json for testing
const std::string test_json =
    R"({"gate_noise": [{"gate_name": "X", "register_location": ["0"], "noise_channels": [{"matrix": [[[[0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0]]], [[[0.0, 0.0], [0.05773502691896258, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.05773502691896258], [0.0, 0.0]]], [[[0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.05773502691896258, 0.0]]]]}]}], "bit_order": "MSB"})";
// Two-qubit noise channel (on a CNOT gate) in MSB and LSB order convention (matrix representation)
const std::string msb_noise_model = R"({"gate_noise": [{"gate_name": "CNOT", "register_location": ["0", "1"], "noise_channels": [{"matrix": [[[[0.99498743710662, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.99498743710662, 0.0]]], [[[0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, -0.05773502691896258], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.05773502691896258], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.05773502691896258], [0.0, 0.0]]], [[[0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.05773502691896258, 0.0], [0.0, 0.0], [-0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.0, 0.0], [0.0, 0.0], [-0.05773502691896258, 0.0]]]]}]}], "bit_order": "MSB"})";
const std::string lsb_noise_model = R"({"gate_noise": [{"gate_name": "CNOT", "register_location": ["0", "1"], "noise_channels": [{"matrix": [[[[0.99498743710662, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.99498743710662, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.0, -0.05773502691896258], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.05773502691896258], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.05773502691896258], [0.0, 0.0], [0.0, 0.0]]], [[[0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [-0.05773502691896258, 0.0], [-0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [-0.0, 0.0], [-0.05773502691896258, 0.0]]]]}]}], "bit_order": "LSB"})";
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
  // Expected result:
  // 0.00666667+0.j 0.        +0.j
  // 0.        +0.j 0.99333333+0.j
  // Check real part
  EXPECT_NEAR(densityMatrix[0].first, 0.00666667, 1e-6);
  EXPECT_NEAR(densityMatrix[1].first, 0.0, 1e-6);
  EXPECT_NEAR(densityMatrix[2].first, 0.0, 1e-6);
  EXPECT_NEAR(densityMatrix[3].first, 0.99333333, 1e-6);
  // Check imag part
  for (const auto &[real, imag] : densityMatrix) {
    EXPECT_NEAR(imag, 0.0, 1e-6);
  }
}

TEST(JsonNoiseModelTester, checkMsb) {
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", msb_noise_model}});
  const std::string ibmNoiseJson = noiseModel->toJson();
  std::cout << "IBM Equiv: \n" << ibmNoiseJson << "\n";
  auto accelerator = xacc::getAccelerator(
      "aer", {{"noise-model", ibmNoiseJson}, {"sim-type", "density_matrix"}});
  // auto accelerator = xacc::getAccelerator("aer", {{"shots", 8192}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program = xasmCompiler
                     ->compile(R"(__qpu__ void testCX(qbit q) {
        CX(q[0], q[1]);
        Measure(q[0]);
        Measure(q[1]);
      })",
                               accelerator)
                     ->getComposites()[0];
  auto buffer = xacc::qalloc(2);
  accelerator->execute(buffer, program);
  auto densityMatrix =
      (*buffer)["density_matrix"].as<std::vector<std::pair<double, double>>>();
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      const int idx = row * 4 + col;
      std::cout << "(" << densityMatrix[idx].first << ", "
                << densityMatrix[idx].second << ") ";
    }
    std::cout << "\n";
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}