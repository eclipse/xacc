#include <gtest/gtest.h>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "NoiseModel.hpp"
#include "PauliOperator.hpp"
#include "xacc_observable.hpp"

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

// Noise model JSON that have multiple qubits.
// Testing to IBM JSON conversion whereby the noise qubit indexing is relative.
const std::string depol_json_2q =
    R"({"gate_noise": [{"gate_name": "H", "register_location": ["0"], "noise_channels": [{"matrix": [[[[0.999499874937461, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.999499874937461, 0.0]]], [[[0.0, 0.0], [0.018257418583505537, 0.0]], [[0.018257418583505537, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, -0.018257418583505537]], [[0.0, 0.018257418583505537], [0.0, 0.0]]], [[[0.018257418583505537, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.018257418583505537, 0.0]]]]}]}, {"gate_name": "H", "register_location": ["1"], "noise_channels": [{"matrix": [[[[0.999499874937461, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.999499874937461, 0.0]]], [[[0.0, 0.0], [0.018257418583505537, 0.0]], [[0.018257418583505537, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, -0.018257418583505537]], [[0.0, 0.018257418583505537], [0.0, 0.0]]], [[[0.018257418583505537, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.018257418583505537, 0.0]]]]}]}], "bit_order": "MSB"})";

const std::string depol_json_id =
    R"({"gate_noise": [{"gate_name": "I", "register_location": ["0"], "noise_channels": [{"matrix": [[[[0.99498743710662, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.99498743710662, 0.0]]], [[[0.0, 0.0], [0.05773502691896258, 0.0]], [[0.05773502691896258, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, -0.05773502691896258]], [[0.0, 0.05773502691896258], [0.0, 0.0]]], [[[0.05773502691896258, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.05773502691896258, 0.0]]]]}]}], "bit_order": "MSB"})";

const std::string depol_json_id_multi_qubits =
    R"({"gate_noise": [{"gate_name": "I", "register_location": ["0"], "noise_channels": [{"matrix": [[[[0.9486832980505138, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.9486832980505138, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.9486832980505138, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.9486832980505138, 0.0]]], [[[0.0, 0.0], [0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.08164965809277261, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.08164965809277261, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, -0.08164965809277261], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.08164965809277261], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, -0.08164965809277261]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.08164965809277261], [0.0, 0.0]]], [[[0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.08164965809277261, 0.0], [0.0, 0.0], [-0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.08164965809277261, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.0, 0.0], [0.0, 0.0], [-0.08164965809277261, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.08164965809277261, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.08164965809277261, 0.0]], [[0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.0, -0.08164965809277261], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, -0.08164965809277261]], [[0.0, 0.08164965809277261], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.08164965809277261], [0.0, 0.0], [0.0, 0.0]]], [[[0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [-0.08164965809277261, 0.0], [-0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [-0.0, 0.0], [-0.08164965809277261, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.08164965809277261, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.08164965809277261, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, -0.08164965809277261]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.08164965809277261], [0.0, 0.0]], [[0.0, 0.0], [0.0, -0.08164965809277261], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.08164965809277261], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.08164965809277261, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [-0.08164965809277261, 0.0]], [[0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, -0.08164965809277261]], [[0.0, 0.0], [0.0, 0.0], [0.0, -0.08164965809277261], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.08164965809277261], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.08164965809277261], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [-0.08164965809277261, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.08164965809277261, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0]], [[-0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.0, -0.08164965809277261], [0.0, 0.0]], [[0.0, 0.0], [-0.0, 0.0], [0.0, 0.0], [0.0, 0.08164965809277261]], [[0.0, 0.08164965809277261], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, -0.08164965809277261], [0.0, 0.0], [-0.0, 0.0]]], [[[0.0, 0.0], [0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [-0.08164965809277261, 0.0]], [[0.0, 0.0], [0.0, 0.0], [-0.08164965809277261, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, -0.08164965809277261], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.08164965809277261], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [-0.0, 0.0], [0.0, 0.08164965809277261]], [[0.0, 0.0], [0.0, 0.0], [0.0, -0.08164965809277261], [-0.0, 0.0]]], [[[0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [-0.08164965809277261, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [-0.08164965809277261, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.08164965809277261, 0.0]]]], "noise_qubits": ["0", "1"]}]}], "bit_order": "MSB"})";

const std::string noise_json_3q =
    R"({"gate_noise": [{"gate_name": "I", "register_location": ["0"], "noise_channels": [{"matrix": [[[[0.9486832980505138, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.9486832980505138, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.9486832980505138, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.9486832980505138, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.9486832980505138, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.9486832980505138, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.9486832980505138, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.9486832980505138, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.22360679774997896, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.22360679774997896, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.22360679774997896, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.22360679774997896, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.22360679774997896, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.22360679774997896, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.22360679774997896, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.22360679774997896, 0.0], [0.0, 0.0], [0.0, 0.0]]], [[[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.22360679774997896, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.22360679774997896, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.22360679774997896, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.22360679774997896, 0.0]], [[0.22360679774997896, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.22360679774997896, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.22360679774997896, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]], [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.22360679774997896, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]]]], "noise_qubits": ["0", "1", "2"]}]}], "bit_order": "MSB"})";
} // namespace

TEST(JsonNoiseModelTester, checkSimple) {
  // Check depolarizing channels
  {
    auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
    noiseModel->initialize({{"noise-model", depol_json}});
    const std::string ibmNoiseJson = noiseModel->toJson();
    std::cout << "IBM Equiv: \n" << ibmNoiseJson << "\n";
    auto accelerator = xacc::getAccelerator(
        "aer", {{"noise-model", ibmNoiseJson}, {"sim-type", "density_matrix"}});
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
    const std::string ibmNoiseJson = noiseModel->toJson();
    std::cout << "IBM Equiv: \n" << ibmNoiseJson << "\n";
    auto accelerator =
        xacc::getAccelerator("aer", {{"noise-model", ibmNoiseJson}});
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

TEST(JsonNoiseModelTester, checkBitOrdering) {
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
    const std::string ibmNoiseJson = noiseModel->toJson();
    std::cout << "IBM Equiv: \n" << ibmNoiseJson << "\n";
    auto accelerator = xacc::getAccelerator(
        "aer", {{"noise-model", ibmNoiseJson}, {"sim-type", "density_matrix"}});

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
        xacc::getAccelerator("aer", {{"noise-model", noiseModel->toJson()},
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

TEST(JsonNoiseModelTester, checkBitOrderingMeasure) {
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
    const std::string ibmNoiseJson = noiseModel->toJson();
    auto accelerator = xacc::getAccelerator(
        "aer", {{"noise-model", ibmNoiseJson}, {"shots", 8192}});

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
    const std::string ibmNoiseJson = noiseModel->toJson();
    auto accelerator = xacc::getAccelerator(
        "aer", {{"noise-model", ibmNoiseJson}, {"shots", 8192}});

    auto buffer = xacc::qalloc(2);
    accelerator->execute(buffer, program);
    buffer->print();
    // No effect on Q1 (in this noise model)
    EXPECT_EQ(buffer->getMeasurements().size(), 1);
    EXPECT_EQ(buffer->getMeasurementCounts()["0"], 8192);
  }
}

TEST(JsonNoiseModelTester, checkRoError) {
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", ro_error_noise_model}});
  const std::string ibmNoiseJson = noiseModel->toJson();
  std::cout << "IBM Equiv: \n" << ibmNoiseJson << "\n";
  auto accelerator = xacc::getAccelerator(
      "aer", {{"noise-model", ibmNoiseJson}, {"shots", 8192}});
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

TEST(JsonNoiseModelTester, checkIbmNoiseJsonIndexing) {
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", depol_json_2q}});
  const std::string ibmNoiseJson = noiseModel->toJson();
  std::cout << "IBM Equiv: \n" << ibmNoiseJson << "\n";
  auto accelerator = xacc::getAccelerator(
      "aer", {{"noise-model", ibmNoiseJson}, {"sim-type", "density_matrix"}});
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto program = xasmCompiler
                     ->compile(R"(__qpu__ void testHadamard2q(qbit q) {
        H(q[0]);
        H(q[1]);
        Measure(q[0]);
        Measure(q[1]);
      })",
                               accelerator)
                     ->getComposite("testHadamard2q");
  auto buffer = xacc::qalloc(1);
  accelerator->execute(buffer, program);
  buffer->print();
  auto densityMatrix =
      (*buffer)["density_matrix"].as<std::vector<std::pair<double, double>>>();
  EXPECT_EQ(densityMatrix.size(), 16);
}

// Test that when doing noisy simulation w/ Aer using density matrix,
// the result is consistent (no sampling involved)
TEST(JsonNoiseModelTester, testVqeModeWithDensityMatrixSim) {
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", depol_json_2q}});
  const std::string ibmNoiseJson = noiseModel->toJson();
  std::cout << "IBM Equiv: \n" << ibmNoiseJson << "\n";
  auto accelerator = xacc::getAccelerator(
      "aer", {{"noise-model", ibmNoiseJson}, {"sim-type", "density_matrix"}});
  auto xasmCompiler = xacc::getCompiler("xasm");

  auto ir =
      xasmCompiler->compile(R"(__qpu__ void ansatz_temp(qbit q, double t) {
      X(q[0]);
      Ry(q[1], t);
      CX(q[1], q[0]);
      H(q[0]);
      H(q[1]);
      Measure(q[0]);
      Measure(q[1]);
    })",
                            accelerator);

  auto program = ir->getComposite("ansatz_temp");
  // Data for first run:
  std::vector<double> firstRuns;
  const auto angles =
      xacc::linspace(-xacc::constants::pi, xacc::constants::pi, 20);
  for (size_t i = 0; i < angles.size(); ++i) {
    auto buffer = xacc::qalloc(2);
    auto evaled = program->operator()({angles[i]});
    accelerator->execute(buffer, evaled);
    firstRuns.emplace_back(buffer->getExpectationValueZ());
  }

  // Run a second-time, the exp-val must be consistent...
  for (size_t i = 0; i < angles.size(); ++i) {
    auto buffer = xacc::qalloc(2);
    auto evaled = program->operator()({angles[i]});
    accelerator->execute(buffer, evaled);
    EXPECT_NEAR(buffer->getExpectationValueZ(), firstRuns[i], 1e-9);
  }
}

// Make sure the exp-val calc. is correct by running VQE opt.
TEST(JsonNoiseModelTester, testVqe) {
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", depol_json_2q}});
  const std::string ibmNoiseJson = noiseModel->toJson();
  std::cout << "IBM Equiv: \n" << ibmNoiseJson << "\n";
  auto accelerator = xacc::getAccelerator(
      "aer", {{"noise-model", ibmNoiseJson}, {"sim-type", "density_matrix"}});

  // Create the N=2 deuteron Hamiltonian
  auto H_N_2 = xacc::quantum::getObservable(
      "pauli", std::string("5.907 - 2.1433 X0X1 "
                           "- 2.1433 Y0Y1"
                           "+ .21829 Z0 - 6.125 Z1"));

  auto optimizer = xacc::getOptimizer("nlopt");
  xacc::qasm(R"(
        .compiler xasm
        .circuit deuteron_ansatz
        .parameters theta
        .qbit q
        X(q[0]);
        Ry(q[1], theta);
        CNOT(q[1],q[0]);
    )");
  auto ansatz = xacc::getCompiled("deuteron_ansatz");

  // Get the VQE Algorithm and initialize it
  auto vqe = xacc::getAlgorithm("vqe");
  vqe->initialize({std::make_pair("ansatz", ansatz),
                   std::make_pair("observable", H_N_2),
                   std::make_pair("accelerator", accelerator),
                   std::make_pair("optimizer", optimizer)});

  // Allocate some qubits and execute
  auto buffer = xacc::qalloc(2);
  vqe->execute(buffer);

  // Expected result: -1.74886
  EXPECT_NEAR((*buffer)["opt-val"].as<double>(), -1.74886, 0.1);
}

TEST(JsonNoiseModelTester, checkIdNoise) {
  xacc::set_verbose(true);
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void testRzz(qbit q) {
        H(q[0]);
        H(q[1]);
        CX(q[0], q[1]);
        Rz(q[1], pi/2);   
        CX(q[0], q[1]);
        I(q[0]);
        I(q[1]);
      })");
  auto program = ir->getComposites()[0];
  std::cout << "HOWDY: \n" << program->toString() << "\n";
  // Test no noise
  {
    auto accelerator =
        xacc::getAccelerator("aer", {{"sim-type", "density_matrix"}});
    auto buffer = xacc::qalloc(2);
    accelerator->execute(buffer, program);
    buffer->print();
    auto densityMatrix = (*buffer)["density_matrix"]
                             .as<std::vector<std::pair<double, double>>>();
    for (int row = 0; row < 4; ++row) {
      for (int col = 0; col < 4; ++col) {
        const int idx = row * 4 + col;
        std::cout << "(" << densityMatrix[idx].first << ", "
                  << densityMatrix[idx].second << ") ";
      }
      std::cout << "\n";
    }
  }

  // Test with noise in Id gates
  {
    auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
    noiseModel->initialize({{"noise-model", depol_json_id}});
    const std::string ibmNoiseJson = noiseModel->toJson();
    std::cout << "IBM Equiv: \n" << ibmNoiseJson << "\n";
    auto accelerator = xacc::getAccelerator(
        "aer", {{"noise-model", ibmNoiseJson}, {"sim-type", "density_matrix"}});
    auto buffer = xacc::qalloc(2);
    accelerator->execute(buffer, program);
    buffer->print();
    auto densityMatrix = (*buffer)["density_matrix"]
                             .as<std::vector<std::pair<double, double>>>();
    for (int row = 0; row < 4; ++row) {
      for (int col = 0; col < 4; ++col) {
        const int idx = row * 4 + col;
        std::cout << "(" << densityMatrix[idx].first << ", "
                  << densityMatrix[idx].second << ") ";
      }
      std::cout << "\n";
    }
  }
}

TEST(JsonNoiseModelTester, checkIdNoiseMultiQubits1) {
  xacc::set_verbose(true);
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void testId2q(qbit q) {
        I(q[0]);
      })");
  auto program = ir->getComposites()[0];
  std::cout << "HOWDY: \n" << program->toString() << "\n";
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", depol_json_id_multi_qubits}});
  const std::string ibmNoiseJson = noiseModel->toJson();
  std::cout << "IBM Equiv: \n" << ibmNoiseJson << "\n";
  auto accelerator = xacc::getAccelerator(
      "aer", {{"noise-model", ibmNoiseJson}, {"sim-type", "density_matrix"}});
  auto buffer = xacc::qalloc(2);
  accelerator->execute(buffer, program);
  buffer->print();
  auto densityMatrix =
      (*buffer)["density_matrix"].as<std::vector<std::pair<double, double>>>();
  double norm = 0.0;
  for (int row = 0; row < 4; ++row) {
    for (int col = 0; col < 4; ++col) {
      const int idx = row * 4 + col;
      if (row == col) {
        norm += densityMatrix[idx].first;
      }
      std::cout << "(" << densityMatrix[idx].first << ", "
                << densityMatrix[idx].second << ") ";
    }
    std::cout << "\n";
  }
  EXPECT_NEAR(norm, 1.0, 1e-6);
}

TEST(JsonNoiseModelTester, checkIdNoiseMultiQubits2) {
  xacc::set_verbose(true);
  auto xasmCompiler = xacc::getCompiler("xasm");
  auto ir = xasmCompiler->compile(R"(__qpu__ void testId3q(qbit q) {
        I(q[0]);
      })");
  auto program = ir->getComposites()[0];
  std::cout << "HOWDY: \n" << program->toString() << "\n";
  auto noiseModel = xacc::getService<xacc::NoiseModel>("json");
  noiseModel->initialize({{"noise-model", noise_json_3q}});
  const std::string ibmNoiseJson = noiseModel->toJson();
  std::cout << "IBM Equiv: \n" << ibmNoiseJson << "\n";
  auto accelerator = xacc::getAccelerator(
      "aer", {{"noise-model", ibmNoiseJson}, {"sim-type", "density_matrix"}});
  auto buffer = xacc::qalloc(3);
  accelerator->execute(buffer, program);
  buffer->print();
  auto densityMatrix =
      (*buffer)["density_matrix"].as<std::vector<std::pair<double, double>>>();
  double norm = 0.0;
  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      const int idx = row * 8 + col;
      if (row == col) {
        norm += densityMatrix[idx].first;
      }
      std::cout << "(" << densityMatrix[idx].first << ", "
                << densityMatrix[idx].second << ") ";
    }
    std::cout << "\n";
  }
  EXPECT_NEAR(norm, 1.0, 1e-6);
}

int main(int argc, char **argv) {
  xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}