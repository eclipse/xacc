#include <GOAT.hpp>

GOAT_PulseOptim::GOAT_PulseOptim(const Matrix& in_targetU, const Hamiltonian& in_hamiltonian, const dHdalpha& in_dHda,
    IStopper* io_stopper, IIntegrator* io_integrator, IGradientStepper* io_gradStepper):
    m_targetU(in_targetU),
    m_hamiltonian(in_hamiltonian),
    m_dHda(in_dHda),
    m_stopper(io_stopper),
    m_integrator(io_integrator),
    m_gradStepper(io_gradStepper)
    {}

OptimParams GOAT_PulseOptim::optimize(const OptimParams& in_initialParams, double in_maxTime, double& out_finalCost)
{
    assert(m_dHda.size() == in_initialParams.size());
    // Set-up initial parameters
    OptimParams params = in_initialParams;
    // Hamiltonian matrix @ t = 0; params = init
    const auto initialH = m_hamiltonian(0, in_initialParams);
    // Hamiltonian must be a square matrix
    assert(initialH.rows() == initialH.cols());
    const auto dimH = initialH.rows();
    
    // High level execution
    while (true)
    {
        // Before the integration
        const auto integrateResult = m_integrator->integrate(m_hamiltonian, m_dHda, in_maxTime);
        // We expect the result is a matrix that has (nb_params+1)*dimH rows and dimH columns
        // i.e. we have U(T) and a vector of dUda(T) (one for each param) stacked on each other.
        assert(integrateResult.cols() == dimH);
        assert(integrateResult.rows() == dimH * (1 + params.size()));

        const auto evalCostFn = [&](const Matrix& in_uMat) -> double {
            assert(m_targetU.rows() == in_uMat.rows() && m_targetU.cols() == in_uMat.cols());
            // Const/goal function, i.e. Eq. (4)
            return 1.0 - (1.0/m_targetU.rows())*std::abs((m_targetU.adjoint() * in_uMat).trace());
        };
 
        const auto evalCostFnGradient = [&](const std::vector<Matrix>& in_duMats, const OptimParams& in_params, double in_costVal) -> std::vector<double> {
            assert(in_duMats.size() == in_params.size());
            std::vector<double> results;

            for (int i = 0; i < in_params.size(); ++i)
            {
                assert(in_duMats[i].rows() == m_targetU.rows() && m_targetU.cols() == in_duMats[i].cols());
                // Cost function gradients, i.e., Eq. (6)
                const auto gradResult = - std::real((1/ std::abs(in_costVal))*(1/dimH)*(m_targetU.adjoint() * in_duMats[i]).trace());
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

        const double costFnVal = evalCostFn(uMat);

        // Check if we should stop now (e.g. cost function value has reached the target)
        if (m_stopper->shouldStop(costFnVal, params))
        {
            out_finalCost = costFnVal;
            return params;
        }

        // If not, continue
        
        // Evaluate the gradients
        const std::vector<double> costFnGradVals = evalCostFnGradient(duMats, params, costFnVal);
        // Use the provide gradient-based stepper (e.g. BFGS) to find the step sizes
        const auto paramStepSzs = m_gradStepper->calculate(costFnGradVals);
        assert(paramStepSzs.size() == params.size());

        for (int i = 0; i < params.size(); ++i)
        {
            // Update the params by the step size
            params[i] += paramStepSzs[i];
        }
    } 
}