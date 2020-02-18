#pragma once
#include <vector>
#include <Eigen/Dense>

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

// Abstract stopping condition: we'll check it after every iteration,
// e.g. can implement iteration count and/or convergent stopping conditions.
struct IStopper
{
    virtual bool shouldStop(double in_costFnValue, const OptimParams& in_currentParams) = 0;
};

// Integrator/propagator (i.e. solving Eq. (7))
struct IIntegrator
{
    virtual Matrix integrate(const Hamiltonian& in_hamiltonian, const dHdalpha& in_dHda, double in_stopTime) = 0;
};

// Abstract gradient-based step search (e.g. BFGS)
struct IGradientStepper
{
    // Calculate the param steps given the current cost function differential 
    // (gradients) w.r.t. the params.
    virtual OptimParams calculate(const std::vector<double>& in_costFnDiffs) = 0;
};

class GOAT_PulseOptim
{
public:
    GOAT_PulseOptim(const Matrix& in_targetU, const Hamiltonian& in_hamiltonian, const dHdalpha& in_dHda,  IStopper* io_stopper, IIntegrator* io_integrator, IGradientStepper* io_gradStepper);
    OptimParams optimize(const OptimParams& in_initialParams, double in_maxTime, double& out_finalCost);
private:
    const Matrix& m_targetU;
    const Hamiltonian& m_hamiltonian;
    const dHdalpha& m_dHda;
    IStopper* m_stopper;
    IIntegrator* m_integrator;
    IGradientStepper* m_gradStepper;
};
