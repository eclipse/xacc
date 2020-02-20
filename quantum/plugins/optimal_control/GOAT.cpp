#include "GOAT.hpp"
#include <iostream>

namespace {
constexpr int DEFAULT_NUMBER_STEPS = 10000;
class RungeKutta
{
public:
    RungeKutta(std::function<Matrix(float, const Matrix&)> in_func): 
        m_func(in_func) 
    {}

    // Simple RK stepper
    Matrix step(double in_time, const Matrix& in_mat, double in_dt) const
    {
        Matrix k1, k2, k3, k4;
        k1 = m_func(in_time, in_mat);
        k2 = m_func(in_time + in_dt / 2, in_mat + (in_dt / 2) * k1);
        k3 = m_func(in_time + in_dt / 2, in_mat + (in_dt / 2) * k2);
        k4 = m_func(in_time + in_dt, in_mat + in_dt * k3);

        return in_mat + (k1 + 2 * k2 + 2 * k3 + k4) * in_dt / 6;
    }

private:
    std::function<Matrix(float, const Matrix&)> m_func;
};

Matrix vstack(const Matrix& in_mat1, const Matrix& in_mat2)
{
    if(in_mat1.rows() == 0)
    {
        return in_mat2;
    }
    else if(in_mat2.rows() == 0)
    {
        return in_mat1;
    }

    auto nbCols = in_mat1.cols();
    
    if(nbCols == 0)
    {
        nbCols = in_mat2.cols();
    }

    Matrix result(in_mat1.rows()+in_mat2.rows(), nbCols);
    result << in_mat1, in_mat2;
  
    return result;
}
}


GOAT_PulseOptim::GOAT_PulseOptim(const Matrix& in_targetU, const Hamiltonian& in_hamiltonian, const dHdalpha& in_dHda, 
    const OptimParams& in_initialParams, double in_maxTime, 
    std::unique_ptr<IIntegrator>&& io_integrator, std::unique_ptr<IGradientStepper>&& io_gradStepper):
    m_targetU(in_targetU),
    m_hamiltonian(in_hamiltonian),
    m_dHda(in_dHda),
    m_integrator(std::move(io_integrator)),
    m_gradStepper(std::move(io_gradStepper)),
    m_initialParams(in_initialParams),
    m_maxTime(in_maxTime),
    m_iterationCounter(0)
{
    // If not provided, use the default propagator (Runge-Kutta) and gradient-based stepper (L-BFGS)
    if (!m_integrator)
    {
        m_integrator = std::make_unique<DefaultIntegrator>(in_maxTime/DEFAULT_NUMBER_STEPS);
    }

    if (!m_gradStepper)
    {
        m_gradStepper =  std::make_unique<DefaultGradientStepper>();
    }
}


GOAT_PulseOptim::OptimizationResult GOAT_PulseOptim::optimize()
{
    m_iterationCounter = 0;
    double* ptr = &m_initialParams[0];
    Eigen::Map<Eigen::VectorXd> initParams(ptr, m_initialParams.size());
    // Invoke the Gradient stepper to drive optimization procedure
    m_gradStepper->optimize(this, initParams);
    
    // Return the result cache (update during optimization run)
    return m_resultCache;
}

double GOAT_PulseOptim::operator()(const Eigen::VectorXd& in_params, Eigen::VectorXd& out_grads) 
{
    double fx = 0.0;
    assert(m_dHda.size() == in_params.size());
    // Set-up initial parameters
    OptimParams params;
    params.resize(in_params.size());
    Eigen::VectorXd::Map(&params[0], params.size()) = in_params;

    // Hamiltonian matrix @ t = 0; params = init
    const auto initialH = m_hamiltonian(0, params);
    // Hamiltonian must be a square matrix
    assert(initialH.rows() == initialH.cols());
    const auto dimH = initialH.rows();
    
    const auto integrateResult = m_integrator->integrate(m_hamiltonian, m_dHda, params, m_maxTime);
    // We expect the result is a matrix that has (nb_params+1)*dimH rows and dimH columns
    // i.e. we have U(T) and a vector of dUda(T) (one for each param) stacked on each other.
    assert(integrateResult.cols() == dimH);
    assert(integrateResult.rows() == dimH * (1 + params.size()));

    const auto evalCostFn = [&](const Matrix& in_uMat) -> double {
        assert(m_targetU.rows() == in_uMat.rows() && m_targetU.cols() == in_uMat.cols());
        // Cost/goal function, i.e. Eq. (4)
        return 1.0 - (1.0/m_targetU.rows())*std::abs((m_targetU.adjoint() * in_uMat).trace());
    };

    const auto evalCostFnGradient = [&](const std::vector<Matrix>& in_duMats, const OptimParams& in_params, double in_costVal) -> std::vector<double> {
        assert(in_duMats.size() == in_params.size());
        std::vector<double> results;

        for (int i = 0; i < in_params.size(); ++i)
        {
            assert(in_duMats[i].rows() == m_targetU.rows() && m_targetU.cols() == in_duMats[i].cols());  
            // Cost function gradients, i.e., Eq. (6)
            const auto gradResult = -std::real((1.0/dimH)*(m_targetU.adjoint() * in_duMats[i]).trace());
            results.emplace_back(gradResult);
        }

        return results;
    }; 

    const auto uMat = integrateResult(Eigen::seqN(0, dimH), Eigen::all);
    std::vector<Matrix> duMats;
    for (int i = 0; i < params.size(); ++i)
    {
        // Vector of dUda (one for each param)
        duMats.emplace_back(integrateResult(Eigen::seqN(dimH + i*dimH, dimH), Eigen::all));
    }

    fx = evalCostFn(uMat);
    // Evaluate the gradients
    const std::vector<double> costFnGradVals = evalCostFnGradient(duMats, params, fx);

    for (int i = 0; i < costFnGradVals.size(); ++i)
    {
        out_grads[i] = costFnGradVals[i];
    }

    // SUMMARY:
    {
        std::cout << "        Iteration: " << m_iterationCounter << "\n";
        std::cout << "Params = " << in_params << "\n";
        std::cout << "Gradients = " << out_grads << "\n";
        std::cout << "Cost Function Val = " << fx << "\n";
        std::cout << "===========================================\n";
    }
  
    {
        m_resultCache.finalCost = fx;
        m_resultCache.optParams = params;
        m_resultCache.nbIters = m_iterationCounter;
    }

    m_iterationCounter++;
    return fx;
}

GOAT_PulseOptim::DefaultIntegrator::DefaultIntegrator(double in_dt):
    m_dt(in_dt)
{}

Matrix GOAT_PulseOptim::DefaultIntegrator::integrate(const Hamiltonian& in_hamiltonian, const dHdalpha& in_dHda, const OptimParams& in_params, double in_stopTime)
{
    const auto nbParams = in_dHda.size();
    const auto initialH = in_hamiltonian(0, in_params);
    const auto dimH = initialH.rows();
    
    // initial condition: U(0) = I, dUdalpha(0) = 0
    Matrix U = Matrix::Identity(dimH, dimH);
    std::vector<Matrix> dUs;
    for (int i = 0; i < nbParams; ++i)
    {
        dUs.emplace_back(Matrix::Zero(dimH, dimH));
    }
   
    for (int i = 0; i * m_dt < in_stopTime; ++i)
    {
        const double currentTime = m_dt*i;
        const auto currentU = U;
        
        std::function<Matrix(float, const Matrix&)> uIntegrateFunc = [&in_hamiltonian, &in_params](float in_time, const Matrix& in_u) -> Matrix {
            const std::complex<double> I(0.0, 1.0);
            return -I*in_hamiltonian(in_time, in_params)*in_u;
        };
        const RungeKutta uProgagator(uIntegrateFunc);

        std::vector<RungeKutta> dUPropagators;
        for (const auto& dHda :  in_dHda)
        {
            std::function<Matrix(float, const Matrix&)> duIntegrateFunc = [&in_hamiltonian, &in_params, &dHda, &currentU](float in_time, const Matrix& in_u) -> Matrix {
                const std::complex<double> I(0.0, 1.0);
                return -I*(dHda(in_time, in_params)*currentU + in_hamiltonian(in_time, in_params)*in_u);
            };
            const RungeKutta dUProgagator(duIntegrateFunc);

            dUPropagators.emplace_back(dUProgagator);
        }
   
        // Propagate U 
        U = uProgagator.step(currentTime, U, m_dt);
        // Propagate dUdalpha (params)
        for (int j = 0; j < nbParams; ++j)
        {
           dUs[j] = dUPropagators[j].step(currentTime, dUs[j], m_dt);
        }
    }
   
    Matrix result = U;     
    for (const auto& dUMat : dUs)
    {
        result = vstack(result, dUMat);
    }

    return result;
}

void GOAT_PulseOptim::DefaultGradientStepper::optimize(IOptimizationFunction* io_problem, const Eigen::VectorXd& in_initialParams) 
{
    // Set up parameters
    // TODO: enable customization via Heterogenous map
    // We are using a very simple L-BFGS implementation (~100 lines of code)
    // Interestingly enough, both ML-PACK and NL-OPT were *UNABLE* to converge
    // but this simple impl can converge just fine.
    // Looks like ML-PACK and NL-OPT were stuck at local minima and don't have any mechanisms to escape.
    // For now, just use this simple implementation.
    LBFGSpp::LBFGSParam<double> param;
    param.epsilon = 1e-6;
    param.max_iterations = 50;
    LBFGSpp::LBFGSSolver<double> solver(param);
    
    double fx = 0.0;
    Eigen::VectorXd x = in_initialParams;
    solver.minimize(*io_problem, x, fx);
}
    