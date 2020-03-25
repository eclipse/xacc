#include <gtest/gtest.h>
#include "xacc.hpp"
#include "Optimizer.hpp"
#include <Eigen/Dense>

TEST(GrapeOptimTester, testSimple)
{
    const double tMax = 10;
    const double dt = 1;
    const int dimension = 1;
    // Mirror Qutip 'test_01_1_unitary_hadamard' in test_control_pulseoptim.py
    // Target Unitary: Hadamard
    Eigen::MatrixXcd Hadamard { Eigen::MatrixXcd::Zero(2, 2) };
    Hadamard << 1.0/sqrt(2.0), 1.0/sqrt(2.0), 1.0/sqrt(2.0), -1.0/sqrt(2.0);
   
    // GRAPE pulse optimization configs:
    xacc::HeterogeneousMap configs {
        std::make_pair("method", "GRAPE"),
        std::make_pair("dimension", dimension),
        // Target unitary is Hadamard matrix
        std::make_pair("target-U", Hadamard),
        // QuTip Hamiltonian:
        // Note: effectively, this is an off-resonance driving to achieve Hadamard
        // since we are in lab-frame, not rotating frame (with sigma_z), but the control field
        // (driving sigma_x) is not modulated (at sigma_z freq.)
        // H_d = sigmaz()
        // H_c = [sigmax()]
        std::make_pair("static-H", "Z0"),
        // The list of Hamiltonian terms that are modulated by the control functions
        std::make_pair("control-H", std::vector<std::string> { "X0" }),
        std::make_pair("dt", dt),
        std::make_pair("max-time", tMax)
    };
    
    auto optimizer = xacc::getOptimizer("quantum-control", configs);
    const auto result = optimizer->optimize();
    std::cout << "Final fidelity error = " << result.first << "\n";
    // Near zero
    EXPECT_NEAR(result.first, 0.0, 1e-9);
}

int main(int argc, char **argv) 
{
    xacc::Initialize();
    ::testing::InitGoogleTest(&argc, argv);
    const auto result = RUN_ALL_TESTS();
    xacc::Finalize();
    return result;
}