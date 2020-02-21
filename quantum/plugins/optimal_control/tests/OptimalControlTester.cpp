#include <gtest/gtest.h>
#include "xacc.hpp"
#include "Optimizer.hpp"

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
        std::make_pair("init-params", initParams),
        std::make_pair("max-time", tMax)
    };
    
    auto optimizer = xacc::getOptimizer("optimal", configs);
    const auto result = optimizer->optimize(xacc::Optimizer::NOOP_FUNCTION());
    // Optimal param ~ 19.9, cost function value -> 0.0
    EXPECT_EQ(result.second.size(), 1);
    EXPECT_NEAR(result.second[0],  19.9, 0.1);
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