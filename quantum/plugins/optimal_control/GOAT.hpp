#pragma once
#include <vector>
#include <memory>
#include <Eigen/Dense>
#include "xacc.hpp"

// Proof-of-concept implemenation of the Quantum Optimization of Analytic conTrols (GOAT) algorithm.
// The GOAT algorithm required:
// (1) Analytical expression of the differentiation of the Hamiltonian w.r.t. the control params.
// (2) A generic integrator (e.g. Runge-Kutta) to evolve the U matrix and its differentiation w.r.t. control params.
// This needs the result from (1) to construct the RHS of the differential equation.
// (3) A gradient-based param search mechanism (e.g. BFGS)
// The gradient of the cost/target function w.r.t. control params is calculated by the result of (2)
// All of these ingredients are abstracted away, i.e. provided by user.
// TODO: gradually put this into a more structured framework (perhaps as services)

// Type aliasing:
using Matrix = Eigen::MatrixXcd;
using OptimParams = std::vector<double>;
// Hamiltonian is a function from time (double) and optimization params to a matrix.
using Hamiltonian = std::function<Matrix(double, OptimParams)>;
// Differentiation of the Hamiltonian w.r.t. the params
// (a vector, same size as the list of parameters)
using dHdalpha = std::vector<Hamiltonian>;
// Inputs: target and actual unitary matrices
using costFunctional = std::function<double(Matrix, Matrix)>;
// Gradient of the cost function w.r.t. all params
using costFunctionalGradient = std::function<std::vector<double>(costFunctional, Matrix, std::vector<Matrix>)>;

// Integrator/propagator (i.e. solving Eq. (7))
struct IIntegrator
{
    virtual Matrix integrate(const Hamiltonian& in_hamiltonian, const dHdalpha& in_dHda, const OptimParams& in_params, double in_stopTime) = 0;
};

// Abstract gradient-based step search (e.g. BFGS)
struct IGradientStepper
{
    virtual void optimize(xacc::OptFunction* io_problem, const OptimParams& in_initialParams) = 0;
};

// GOAT optimal control:
class GOAT_PulseOptim: public xacc::OptFunction
{
public:
    struct DefaultIntegrator : public IIntegrator
    {
        DefaultIntegrator(double in_dt);
        virtual Matrix integrate(const Hamiltonian& in_hamiltonian, const dHdalpha& in_dHda, const OptimParams& in_params, double in_stopTime) override;

    private:
        double m_dt;
    };

    struct DefaultGradientStepper : public IGradientStepper
    {
        virtual void optimize(xacc::OptFunction* io_problem, const OptimParams& in_initialParams) override;
    };

    struct MLPackGradientStepper : public IGradientStepper
    {
        virtual void optimize(xacc::OptFunction* io_problem, const OptimParams& in_initialParams) override;
    };
    
    struct OptimizationResult 
    {
        OptimizationResult(): finalCost(1.0), nbIters(0) {}
        double finalCost;
        OptimParams optParams;
        int nbIters;
    };

    // Constructor: optionally provide integrator/propagator and the gradient stepper.
    // Otherwise the default ones will be used.
    GOAT_PulseOptim(const Matrix& in_targetU, const Hamiltonian& in_hamiltonian, const dHdalpha& in_dHda, const OptimParams& in_initialParams, double in_maxTime, 
    std::unique_ptr<IIntegrator>&& io_integrator = nullptr, std::unique_ptr<IGradientStepper>&& io_gradStepper = nullptr);
    OptimizationResult optimize();
    
    // Entry point for the gradient stepper to call
    double eval(const OptimParams& in_params, std::vector<double>& out_grads); 

private:
    const Matrix& m_targetU;
    const Hamiltonian& m_hamiltonian;
    const dHdalpha& m_dHda;
    std::unique_ptr<IIntegrator> m_integrator;
    std::unique_ptr<IGradientStepper> m_gradStepper;
    OptimParams m_initialParams;
    double m_maxTime;
    int m_iterationCounter;
    OptimizationResult m_resultCache;
};
