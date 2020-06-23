#include <gtest/gtest.h>
#include "xacc.hpp"
#include <string>
#include "GOAT.hpp"
#include "exprtk.hpp"

using namespace xacc;
using symbol_table_t = exprtk::symbol_table<double>;
using expression_t = exprtk::expression<double>;
using parser_t = exprtk::parser<double>;

TEST(GOATTester, testSimple)
{
    // Simple test:
    // Drive term is a gaussian pulse (analytical form) 
    // Optimization param is sigma (1 param only)
    // d(t) = exp(-t^2/2*sigma^2)
    const Matrix iden = Matrix::Identity(2, 2);
    Matrix pauliX{ Matrix::Zero(2, 2) };
    Matrix pauliZ{ Matrix::Zero(2, 2) };
    const double tMax = 100;
    const double omega_a = 2*M_PI/tMax;
    pauliX << 0, 1, 1, 0;
    pauliZ << 1, 0, 0, -1;

    // Hamiltonian: specified as a function of params and time
    const std::string hamiltonianFn = "exp(-t^2/(2*sigma^2))";
    expression_t expression;
    parser_t parser;
    symbol_table_t symbol_table;
    // Placeholders for variable names
    double g_time = 0.0;
    double g_sigma = 0.0;
    symbol_table.add_variable("t", g_time);
    symbol_table.add_variable("sigma", g_sigma);
    expression.register_symbol_table(symbol_table);
    parser.compile(hamiltonianFn, expression);
   
    const Hamiltonian ham = [&](double in_time, OptimParams in_params) -> Matrix {
        assert(in_params.size() == 1);
       
        // We only has 1 parameter which is the gamma.
        double sigma = in_params[0];
        // Set the variables for evaluation. 
        g_time = in_time;
        g_sigma = sigma;
        // Evaluation result by ExprTk
        const auto exprResult = omega_a*expression.value(); 
        // Analytical result:
        const auto pulseVal = omega_a*std::exp(-std::pow(in_time, 2) / (2 * std::pow(sigma, 2)));
        // Verify that the evaluation (string-based expressions)
        EXPECT_NEAR(exprResult, pulseVal, 1e-6);    
        
        return exprResult * pauliX;
    };

    // Differential of H w.r.t. sigma parameter
    const Hamiltonian dHdsigma = [&](double in_time, OptimParams in_params) -> Matrix {
        assert(in_params.size() == 1);
        const double sigma = in_params[0];
        // Derivative (analytical)      
        const double expectedDerivVal = omega_a * (std::pow(in_time, 2)/ std::pow(sigma, 3)) * std::exp(-std::pow(in_time, 2) / (2 * std::pow(sigma, 2)));
        
        // Derivative w.r.t. the sigma parameter
        g_time = in_time;
        g_sigma = sigma;
        // Compute the derivative (exprtk)
        const double exprDerivEval = omega_a * exprtk::derivative(expression, "sigma");
        
        // Verify that the derivative evaluation (string-based expressions)
        EXPECT_NEAR(exprDerivEval, expectedDerivVal, 1e-6);    
        return exprDerivEval * pauliX;
    };

    const dHdalpha dHdparams { dHdsigma };
    // Let's say we want to optimize an X gate: U target = Pauli X
    const std::complex<double> I(0.0, 1.0);
    // Note: this Hamiltonian will result in a global phase of 1i
    // hence, the target U must be set to iX,
    // otherwise the cost functional and gradients will not work.
    const Matrix targetU = -I * pauliX;

    // Expected answer
    const double EXPECTED_OPTIMAL_GAMMA = 20.0; 

    const auto runTest = [&](double in_initialValue){
        OptimParams initialParam { in_initialValue };

        // Construct the GOAT optimizer
        GOAT_PulseOptim myOptimizer(targetU, ham, dHdparams, initialParam, tMax);
        const auto optResult = myOptimizer.optimize();
        
        EXPECT_NEAR(optResult.finalCost, 0.0, 1e-3);
        // We only have 1 parameter (gamma)
        EXPECT_EQ(optResult.optParams.size(), 1);
        // Within 10% variation 
        EXPECT_NEAR(optResult.optParams[0], EXPECTED_OPTIMAL_GAMMA, 0.1*EXPECTED_OPTIMAL_GAMMA);
        // We cap the number of iterations to 50 in GOAT,
        // check that indeed we can converge, not terminate early.
        EXPECT_LT(optResult.nbIters, 50);

        std::cout << "Found OPTIMAL gamma = " << optResult.optParams[0] << " after " << optResult.nbIters << " iterations. \n";
    };

    // Test 1: Initial pulse width = 8ns (below the optimal value ~20)
    std::cout << "##########################################\n";
    runTest(8.0);
    std::cout << "##########################################\n";

    // Test2: Initial pulse width = 30ns (above the optimal value ~20)
    std::cout << "##########################################\n";
    runTest(30.0);
    std::cout << "##########################################\n";
}

// Similar test as before, but use ML-Pack optimizer rather than the default one
TEST(GOATTester, testMlPack)
{
    // Simple test:
    // Drive term is a gaussian pulse (analytical form) 
    // Optimization param is sigma (1 param only)
    // d(t) = exp(-t^2/2*sigma^2)
    const Matrix iden = Matrix::Identity(2, 2);
    Matrix pauliX{ Matrix::Zero(2, 2) };
    Matrix pauliZ{ Matrix::Zero(2, 2) };
    const double tMax = 100;
    const double omega_a = 2*M_PI/tMax;
    pauliX << 0, 1, 1, 0;
    pauliZ << 1, 0, 0, -1;

    // Hamiltonian: specified as a function of params and time
    const std::string hamiltonianFn = "exp(-t^2/(2*sigma^2))";
    expression_t expression;
    parser_t parser;
    symbol_table_t symbol_table;
    // Placeholders for variable names
    double g_time = 0.0;
    double g_sigma = 0.0;
    symbol_table.add_variable("t", g_time);
    symbol_table.add_variable("sigma", g_sigma);
    expression.register_symbol_table(symbol_table);
    parser.compile(hamiltonianFn, expression);
   
    const Hamiltonian ham = [&](double in_time, OptimParams in_params) -> Matrix {
        assert(in_params.size() == 1);
       
        // We only has 1 parameter which is the gamma.
        double sigma = in_params[0];
        // Set the variables for evaluation. 
        g_time = in_time;
        g_sigma = sigma;
        // Evaluation result by ExprTk
        const auto exprResult = omega_a*expression.value(); 
        // Analytical result:
        const auto pulseVal = omega_a*std::exp(-std::pow(in_time, 2) / (2 * std::pow(sigma, 2)));
        // Verify that the evaluation (string-based expressions)
        EXPECT_NEAR(exprResult, pulseVal, 1e-6);    
        
        return exprResult * pauliX;
    };

    // Differential of H w.r.t. sigma parameter
    const Hamiltonian dHdsigma = [&](double in_time, OptimParams in_params) -> Matrix {
        assert(in_params.size() == 1);
        const double sigma = in_params[0];
        // Derivative (analytical)      
        const double expectedDerivVal = omega_a * (std::pow(in_time, 2)/ std::pow(sigma, 3)) * std::exp(-std::pow(in_time, 2) / (2 * std::pow(sigma, 2)));
        
        // Derivative w.r.t. the sigma parameter
        g_time = in_time;
        g_sigma = sigma;
        // Compute the derivative (exprtk)
        const double exprDerivEval = omega_a * exprtk::derivative(expression, "sigma");
        
        // Verify that the derivative evaluation (string-based expressions)
        EXPECT_NEAR(exprDerivEval, expectedDerivVal, 1e-6);    
        return exprDerivEval * pauliX;
    };

    const dHdalpha dHdparams { dHdsigma };
    // Let's say we want to optimize an X gate: U target = Pauli X
    const std::complex<double> I(0.0, 1.0);
    // Note: this Hamiltonian will result in a global phase of 1i
    // hence, the target U must be set to iX,
    // otherwise the cost functional and gradients will not work.
    const Matrix targetU = -I * pauliX;

    // Expected answer
    const double EXPECTED_OPTIMAL_GAMMA = 20.0; 

    const auto runTest = [&](double in_initialValue){
        OptimParams initialParam { in_initialValue };

        auto mlPackOptimizer = std::make_unique<GOAT_PulseOptim::MLPackGradientStepper>();
        // Construct the GOAT optimizer using ML-PACK as the gradient step searcher
        GOAT_PulseOptim myOptimizer(targetU, ham, dHdparams, initialParam, tMax, nullptr, std::move(mlPackOptimizer));
        const auto optResult = myOptimizer.optimize();
        
        EXPECT_NEAR(optResult.finalCost, 0.0, 1e-3);
        // We only have 1 parameter (gamma)
        EXPECT_EQ(optResult.optParams.size(), 1);
        // Within 10% variation 
        EXPECT_NEAR(optResult.optParams[0], EXPECTED_OPTIMAL_GAMMA, 0.1*EXPECTED_OPTIMAL_GAMMA);
        // We cap the number of iterations to 50 in GOAT,
        // check that indeed we can converge, not terminate early.
        EXPECT_LT(optResult.nbIters, 50);

        std::cout << "Found OPTIMAL gamma = " << optResult.optParams[0] << " after " << optResult.nbIters << " iterations. \n";
    };

    // Test 1: Initial pulse width = 8ns (below the optimal value ~20)
    std::cout << "##########################################\n";
    runTest(8.0);
    std::cout << "##########################################\n";

    // Test2: Initial pulse width = 30ns (above the optimal value ~20)
    std::cout << "##########################################\n";
    runTest(30.0);
    std::cout << "##########################################\n";
}

int main(int argc, char **argv) 
{
    xacc::Initialize();
    ::testing::InitGoogleTest(&argc, argv);
    const auto result = RUN_ALL_TESTS();
    xacc::Finalize();
    return result;
}
