#pragma once
#include <vector>
#include <memory>
#include <Eigen/Dense>
#include "xacc.hpp"
#include "exprtk.hpp"
#include "OptimalControl.hpp"

namespace xacc {
using symbol_table_t = exprtk::symbol_table<double>;
using expression_t = exprtk::expression<double>;
using parser_t = exprtk::parser<double>;

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

struct GoatHamiltonian
{
    Hamiltonian hamiltonian;
    dHdalpha dHda;
    int dimension;
    std::vector<std::pair<std::string, Matrix>> hamOps;
    std::vector<std::string> params;
    void construct(int in_dimension, const std::string& in_H0, const std::vector<std::string>& in_Hi, const std::vector<std::string>& in_fi, const std::vector<std::string>& in_params);
private:
    // Cache of the symbol table that are used by the expressions.
    // These symbols are linked to m_paramVals and m_time.
    // Hence, must update those values before evaluating the expressions.
    // Note: compiling these expressions takes a long time, hence, we don't want to repeat it.
    symbol_table_t m_symbolTable;
    parser_t m_parser;
    std::vector<double> m_paramVals;
    double m_time;
    // Compiled expressions
    std::vector<expression_t> m_exprs;
    Matrix m_h0;
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
    
    static Matrix constructMatrixFromPauliString(const std::string& in_pauliString, int in_dimension);
    
    // Entry point for the gradient stepper to call
    double eval(const OptimParams& in_params, std::vector<double>& out_grads); 

private:
    Matrix m_targetU;
    const Hamiltonian& m_hamiltonian;
    const dHdalpha& m_dHda;
    std::unique_ptr<IIntegrator> m_integrator;
    std::unique_ptr<IGradientStepper> m_gradStepper;
    OptimParams m_initialParams;
    double m_maxTime;
    int m_iterationCounter;
    OptimizationResult m_resultCache;
};

// Public GOAT pulse optimization interface
class PulseOptimGOAT : public Optimizer
{   
public:
    const std::string name() const override { return "GOAT"; }
    const std::string description() const override { return ""; }
    void setOptions(const HeterogeneousMap& in_options) override;
    OptResult optimize() override;
    OptResult optimize(OptFunction& function) override { return optimize(); }

private:
    std::unique_ptr<GOAT_PulseOptim> m_goatOptimizer;
    std::unique_ptr<GoatHamiltonian> m_hamiltonian;
};

class PauliUnitaryMatrixUtil : public UnitaryMatrixUtil
{
public:
    const std::string name() const override { return "Pauli"; }
    const std::string description() const override { return ""; }
    Eigen::MatrixXcd fromString(const std::string& in_unitaryString, int in_dimension) override {
        return GOAT_PulseOptim::constructMatrixFromPauliString(in_unitaryString, in_dimension);
    }
    
    Eigen::MatrixXcd fromHeterogeneousMap(const HeterogeneousMap& in_options, int in_dimension, const std::string& in_fieldName) override;
};
}
