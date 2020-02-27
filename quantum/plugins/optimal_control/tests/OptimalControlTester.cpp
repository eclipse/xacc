#include <gtest/gtest.h>
#include "xacc.hpp"
#include "Optimizer.hpp"
#include <Eigen/Dense>

TEST(OptimalControlTester, testSimple)
{
    const std::vector<double> initParams { 8.0 };
    const double tMax = 100;
    const int dimension = 1;

    // GOAT pulse optimization configs:
    xacc::HeterogeneousMap configs {
        std::make_pair("method", "GOAT"),
        std::make_pair("dimension", dimension),
        // Target unitary is an X gate on qubit 0
        std::make_pair("target-U", "X0"),
        // Control parameter (used in the control function)
        std::make_pair("control-params", std::vector<std::string> { "sigma" }),
        std::make_pair("control-funcs", std::vector<std::string> { "0.062831853*exp(-t^2/(2*sigma^2))" }),
        // The list of Hamiltonian terms that are modulated by the control functions
        std::make_pair("control-H", std::vector<std::string> { "X0" }),
        // Initial params
        std::make_pair("initial-parameters", initParams),
        std::make_pair("max-time", tMax)
    };
    
    auto optimizer = xacc::getOptimizer("quantum-control", configs);
    const auto result = optimizer->optimize();
    // Optimal param ~ 19.9, cost function value -> 0.0
    EXPECT_EQ(result.second.size(), 1);
    EXPECT_NEAR(result.second[0],  19.9, 0.1);
    EXPECT_NEAR(result.first,  0.0, 0.01);
}

TEST(OptimalControlTester, testTargetUMatrixInput)
{
    const std::vector<double> initParams { 4.0 };
    const double tMax = 100;
    const int dimension = 1;
    // Complex elements of the sqrt(X) unitary matrix (1/2 * (1 +/- i))
    const std::complex<double> c1{0.5, 0.5};
    const std::complex<double> c2{0.5, -0.5};
    // Target Unitary: sqrt(X)
    Eigen::MatrixXcd sqrtX{ Eigen::MatrixXcd::Zero(2, 2) };
    sqrtX << c1, c2, c2, c1;
    
    // GOAT pulse optimization configs:
    xacc::HeterogeneousMap configs {
        std::make_pair("method", "GOAT"),
        std::make_pair("dimension", dimension),
        // Target unitary: passing an Eigen matrix as input parameter
        std::make_pair("target-U", sqrtX),
        // Control parameter (used in the control function)
        std::make_pair("control-params", std::vector<std::string> { "sigma" }),
        std::make_pair("control-funcs", std::vector<std::string> { "0.062831853*exp(-t^2/(2*sigma^2))" }),
        // The list of Hamiltonian terms that are modulated by the control functions
        std::make_pair("control-H", std::vector<std::string> { "X0" }),
        // Initial params
        std::make_pair("initial-parameters", initParams),
        std::make_pair("max-time", tMax)
    };
    
    auto optimizer = xacc::getOptimizer("quantum-control", configs);
    const auto result = optimizer->optimize();
    // Optimal param ~ 10.0 (half that of the full X gate), cost function value -> 0.0
    EXPECT_EQ(result.second.size(), 1);
    EXPECT_NEAR(result.second[0],  10.0, 0.5);
    EXPECT_NEAR(result.first,  0.0, 0.01);
}

TEST(OptimalControlTester, testMultipleParams)
{
    const std::vector<double> initParams { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 };
    const double tMax = 100;
    const int dimension = 1;

    // GOAT pulse optimization configs:
    xacc::HeterogeneousMap configs {
        std::make_pair("method", "GOAT"),
        std::make_pair("optimizer", "ml-pack"),
        std::make_pair("dimension", dimension),
        // Target unitary is an X gate on qubit 0
        std::make_pair("target-U", "X0"),
        // Control parameter (used in the control function)
        std::make_pair("control-params", std::vector<std::string> { "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8" }),
        // Fourier series (first 8 terms only)
        std::make_pair("control-funcs", std::vector<std::string> { "a0 + a1*cos(1*0.1*t) + b1*sin(1*0.1*t) + a2*cos(2*0.1*t) + b2*sin(2*0.1*t) + a3*cos(3*0.1*t) + b3*sin(3*0.1*t) + a4*cos(4*0.1*t) + b4*sin(4*0.1*t) + a5*cos(5*0.1*t) + b5*sin(5*0.1*t) + a6*cos(6*0.1*t) + b6*sin(6*0.1*t) + a7*cos(7*0.1*t) + b7*sin(7*0.1*t) + a8*cos(8*0.1*t) + b8*sin(8*0.1*t)" }),
        // The list of Hamiltonian terms that are modulated by the control functions
        std::make_pair("control-H", std::vector<std::string> { "X0" }),
        // Initial params
        std::make_pair("initial-parameters", initParams),
        std::make_pair("max-time", tMax)
    };
    
    auto optimizer = xacc::getOptimizer("quantum-control", configs);
    const auto result = optimizer->optimize();
    // We have optimized all the params
    EXPECT_EQ(result.second.size(), initParams.size());
    // Should converge to zero cost function (gate fidelity -> 1.0)
    EXPECT_NEAR(result.first,  0.0, 0.01);
}

int main(int argc, char **argv) 
{
    xacc::Initialize();
    ::testing::InitGoogleTest(&argc, argv);
    const auto result = RUN_ALL_TESTS();
    xacc::Finalize();
    return result;
}