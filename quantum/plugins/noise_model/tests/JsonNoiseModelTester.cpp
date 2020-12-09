#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "NoiseModel.hpp"

namespace {
    // A sample Json for testing
    const std::string test_json = R"({"gate_noise": [{"gate_name": "H", "register_location": ["0"], "noise_kraus_ops": [{"matrix": [[[0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0]]], "noise_qubits": ["0"]}, {"matrix": [[[0.0, 0.0], [0.05773502691896258, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0]]], "noise_qubits": ["0"]}, {"matrix": [[[0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.05773502691896258], [0.0, 0.0]]], "noise_qubits": ["0"]}, {"matrix": [[[0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.05773502691896258, 0.0]]], "noise_qubits": ["0"]}]}, {"gate_name": "X", "register_location": ["0"], "noise_kraus_ops": [{"matrix": [[[0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0]]], "noise_qubits": ["0"]}, {"matrix": [[[0.0, 0.0], [0.05773502691896258, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0]]], "noise_qubits": ["0"]}, {"matrix": [[[0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.05773502691896258], [0.0, 0.0]]], "noise_qubits": ["0"]}, {"matrix": [[[0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.05773502691896258, 0.0]]], "noise_qubits": ["0"]}]}], "bit_order": "MSB"})";
}

TEST(JsonNoiseModelTester, checkSimple) {
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", test_json}});
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}