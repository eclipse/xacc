#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "NoiseModel.hpp"

namespace {
// A sample Json for testing
// Single-qubit depolarizing:
const std::string depol_json =
    R"({"gate_noise": [{"gate_name": "X", "register_location": ["0"], "noise_channels": [{"matrix": [[[[0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0]]], [[[0.0, 0.0], [0.05773502691896258, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.05773502691896258], [0.0, 0.0]]], [[[0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.05773502691896258, 0.0]]]]}]}], "bit_order": "MSB"})";
// Single-qubit amplitude damping (25% rate):
const std::string ad_json =
    R"({"gate_noise": [{"gate_name": "X", "register_location": ["0"], "noise_channels": [{"matrix": [[[[1.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.8660254037844386, 0.0]]], [[[0.0, 0.0], [0.5, 0.0]], [[0.0, 0.0], [0.0, 0.0]]]]}]}], "bit_order": "MSB"})";
// Two-qubit noise channel (on a CNOT gate) in MSB and LSB order convention
// (matrix representation)
const std::string msb_noise_model =
    R"({"gate_noise": [{"gate_name": "CNOT", "register_location": ["0", "1"], "noise_channels": [{"matrix": [[[[0.99498743710662, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.99498743710662, 0.0]]], [[[0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, -0.05773502691896258], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.05773502691896258], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.05773502691896258], [0.0, 0.0]]], [[[0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.05773502691896258, 0.0], [0.0, 0.0], [-0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.0, 0.0], [0.0, 0.0], [-0.05773502691896258, 0.0]]]]}]}], "bit_order": "MSB"})";
const std::string lsb_noise_model =
    R"({"gate_noise": [{"gate_name": "CNOT", "register_location": ["0", "1"], "noise_channels": [{"matrix": [[[[0.99498743710662, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.99498743710662, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.05773502691896258, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.0, -0.05773502691896258], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.05773502691896258], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.05773502691896258], [0.0, 0.0], [0.0, 0.0]]], [[[0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.05773502691896258, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [-0.05773502691896258, 0.0], [-0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [-0.0, 0.0], [-0.05773502691896258, 0.0]]]]}]}], "bit_order": "LSB"})";
// Noise model that only has readout errors for validation:
// P(1|0) = 0.1; P(0|1) = 0.2
const std::string ro_error_noise_model =
    R"({"gate_noise": [], "bit_order": "MSB", "readout_errors": [{"register_location": "0", "prob_meas0_prep1": 0.2, "prob_meas1_prep0": 0.1}]})";
} // namespace

TEST(QlmNoiseModelTester, checkSimple) {
  // Check depolarizing channels
  {
    auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
    noiseModel->initialize({{"noise-model", depol_json}});
    auto accelerator = xacc::getAccelerator(
        "atos-qlm", {{"noise-model", noiseModel}, {"sim-type", "density_matrix"}});
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
    auto densityMatrix = (*buffer)["density_matrix"]
                             .as<std::vector<std::pair<double, double>>>();
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

  // Check amplitude damping channels
  {
    auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
    noiseModel->initialize({{"noise-model", ad_json}});
    auto accelerator =
        xacc::getAccelerator("atos-qlm", {{"noise-model", noiseModel}});
    auto xasmCompiler = xacc::getCompiler("xasm");
    auto program = xasmCompiler
                       ->compile(R"(__qpu__ void testX_ad(qbit q) {
        X(q[0]);
        Measure(q[0]);
      })",
                                 accelerator)
                       ->getComposites()[0];
    auto buffer = xacc::qalloc(1);
    accelerator->execute(buffer, program);
    buffer->print();
    // Verify the distribution (25% amplitude damping)
    EXPECT_NEAR(buffer->computeMeasurementProbability("0"), 0.25, 0.1);
    EXPECT_NEAR(buffer->computeMeasurementProbability("1"), 0.75, 0.1);
  }
}

TEST(QlmNoiseModelTester, checkBitOrdering) {
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program = xasmCompiler
                     ->compile(R"(__qpu__ void testCX(qbit q) {
        CX(q[0], q[1]);
        Measure(q[0]);
        Measure(q[1]);
      })",
                               nullptr)
                     ->getComposites()[0];

  std::vector<std::pair<double, double>> densityMatrix_msb, densityMatrix_lsb;
  // Check MSB:
  {
    auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
    noiseModel->initialize({{"noise-model", msb_noise_model}});
    auto accelerator = xacc::getAccelerator(
        "atos-qlm", {{"noise-model", noiseModel}, {"sim-type", "density_matrix"}});

    auto buffer = xacc::qalloc(2);
    accelerator->execute(buffer, program);
    densityMatrix_msb = (*buffer)["density_matrix"]
                            .as<std::vector<std::pair<double, double>>>();
  }

  // Check LSB:
  {
    auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
    noiseModel->initialize({{"noise-model", lsb_noise_model}});
    auto accelerator =
        xacc::getAccelerator("atos-qlm", {{"noise-model", noiseModel},
                                     {"sim-type", "density_matrix"}});
    auto buffer = xacc::qalloc(2);
    accelerator->execute(buffer, program);
    densityMatrix_lsb = (*buffer)["density_matrix"]
                            .as<std::vector<std::pair<double, double>>>();
  }

  // Check:
  EXPECT_EQ(densityMatrix_lsb.size(), 16);
  EXPECT_EQ(densityMatrix_msb.size(), 16);
  for (int i = 0; i < 16; ++i) {
    EXPECT_NEAR(densityMatrix_lsb[i].first, densityMatrix_msb[i].first, 1e-6);
    EXPECT_NEAR(densityMatrix_lsb[i].second, densityMatrix_msb[i].second, 1e-6);
  }

  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      const int idx = row * 4 + col;
      std::cout << "(" << densityMatrix_msb[idx].first << ", "
                << densityMatrix_msb[idx].second << ") ";
    }
    std::cout << "\n";
  }
}

TEST(QlmNoiseModelTester, checkBitOrderingMeasure) {
  auto xasmCompiler = xacc::getCompiler("xasm");
  {
    auto program = xasmCompiler
                       ->compile(R"(__qpu__ void testCX_Q0(qbit q) {
        CX(q[0], q[1]);
        Measure(q[0]);
      })",
                                 nullptr)
                       ->getComposites()[0];
    auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
    noiseModel->initialize({{"noise-model", msb_noise_model}});
    auto accelerator = xacc::getAccelerator(
        "atos-qlm", {{"noise-model", noiseModel}, {"shots", 8192}});

    auto buffer = xacc::qalloc(2);
    accelerator->execute(buffer, program);
    buffer->print();
    // We have depolarization on Q0.
    EXPECT_EQ(buffer->getMeasurements().size(), 2);
    EXPECT_TRUE(buffer->getMeasurementCounts()["1"] > 0);
  }
  {
    auto program = xasmCompiler
                       ->compile(R"(__qpu__ void testCX_Q1(qbit q) {
        CX(q[0], q[1]);
        Measure(q[1]);
      })",
                                 nullptr)
                       ->getComposites()[0];
    auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
    noiseModel->initialize({{"noise-model", msb_noise_model}});
    auto accelerator = xacc::getAccelerator(
        "atos-qlm", {{"noise-model", noiseModel}, {"shots", 8192}});

    auto buffer = xacc::qalloc(2);
    accelerator->execute(buffer, program);
    buffer->print();
    // No effect on Q1 (in this noise model)
    EXPECT_EQ(buffer->getMeasurements().size(), 1);
    EXPECT_EQ(buffer->getMeasurementCounts()["0"], 8192);
  }
}

TEST(QlmNoiseModelTester, checkRoError) {
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", ro_error_noise_model}});
  auto accelerator = xacc::getAccelerator(
      "atos-qlm", {{"noise-model", noiseModel}, {"shots", 8192}});
  {
    auto program = xasmCompiler
                       ->compile(R"(__qpu__ void testId(qbit q) {
        Measure(q[0]);
      })",
                                 nullptr)
                       ->getComposites()[0];

    auto buffer = xacc::qalloc(1);
    accelerator->execute(buffer, program);
    buffer->print();
    // P(1|0) = 0.1
    EXPECT_NEAR(buffer->computeMeasurementProbability("1"), 0.1, 0.05);
  }
  {
    auto program = xasmCompiler
                       ->compile(R"(__qpu__ void testFlip(qbit q) {
        X(q[0]);
        Measure(q[0]);
      })",
                                 nullptr)
                       ->getComposites()[0];

    auto buffer = xacc::qalloc(1);
    accelerator->execute(buffer, program);
    buffer->print();
    // P(0|1) = 0.2
    EXPECT_NEAR(buffer->computeMeasurementProbability("0"), 0.2, 0.05);
  }
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}