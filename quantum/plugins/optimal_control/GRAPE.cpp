#include "GRAPE.hpp"
#include <unsupported/Eigen/MatrixFunctions>
#include "xacc.hpp"
#include "xacc_service.hpp"

namespace {
    const std::complex<double> I(0.0, 1.0);
    std::complex<double> overlapCalc(const Eigen::MatrixXcd& in_mat1, const Eigen::MatrixXcd& in_mat2)
    {
        const double dim = in_mat1.rows();
        return ((in_mat1.adjoint() * in_mat2).trace()) / dim;
    };

    std::vector<double> randPulse(int in_nbSamples)
    {
        std::vector<double> result;
        result.reserve(in_nbSamples);

        for (int i = 0; i < in_nbSamples; ++i)
        {
            constexpr double LO = -1;
            constexpr double HI = 1;

            const double val = LO + static_cast<double>(rand())/(static_cast<double>(RAND_MAX/(HI-LO)));
            result.emplace_back(val);
        }
        
        return result;
    }
}

namespace xacc {
GrapePulseOptim::GrapePulseOptim(const GrapeConfigs& in_configs):
    m_configs(in_configs)
{
    m_pulses.emplace_back(in_configs.initial_pulses);
}

GrapeResult GrapePulseOptim::optimize(int in_nIters, double in_tol)
{
    m_pulses.reserve(in_nIters + 1);
    for (int i = 0; i < in_nIters; ++i)
    {
        const auto newPulses = evaluate();
        m_pulses.emplace_back(std::move(newPulses));
        assert(m_uMats.size() == (i + 1));
        const double fidelityError = 1.0 - std::abs(overlapCalc(m_configs.targetU, m_uMats.back()));
        // Logging: 
        std::cout << "Iteration " << i << ": Fidelity error = " << fidelityError << "\n";

        if (fidelityError < in_tol)
        {
            // Debug
            std::cout << "Achieved fidelity target. Terminating the optimization loop.\n";
            break;
        }
    }
    
    GrapeResult result;
    result.pulses = m_pulses.back();
    result.overlap = std::abs(overlapCalc(m_configs.targetU, m_uMats.back()));
    return result;
}

std::vector<std::vector<double>> GrapePulseOptim::evaluate()
{
    const auto evalHmatAtStep = [](int in_timeStepIdx,  
                                    const GrapeConfigs& in_configs, 
                                    const std::vector<std::vector<double>>& in_ctrlFields) {
        const auto& H0 = in_configs.H0;
        const auto& H_ops = in_configs.H_ops;
        auto resultMat = H0;
        for (int i = 0; i < H_ops.size(); ++i)
        {
            resultMat += (in_ctrlFields[i][in_timeStepIdx] * H_ops[i]);
        }

        return resultMat;  
    };

    std::vector<Eigen::MatrixXcd> U_list;
    U_list.reserve(m_configs.nbSamples);
    // Compute incremental U aling the time series
    for (int i = 0; i < m_configs.nbSamples; ++i)
    {
        const Eigen::MatrixXcd hMat = evalHmatAtStep(i, m_configs, m_pulses.back());
        U_list.emplace_back((-I * hMat * m_configs.dt).exp());
    }

    // Forward and backward unitary matrices
    std::vector<Eigen::MatrixXcd> U_f_list;
    std::vector<Eigen::MatrixXcd> U_b_list;
    U_f_list.reserve(m_configs.nbSamples);
    U_b_list.reserve(m_configs.nbSamples);
    Eigen::MatrixXcd  U_f = Eigen::MatrixXcd::Identity(m_configs.H0.rows(), m_configs.H0.cols());
    Eigen::MatrixXcd  U_b = Eigen::MatrixXcd::Identity(m_configs.H0.rows(), m_configs.H0.cols());
    
    // Calculate the forward and backward propagators
    for (int i = 0; i < m_configs.nbSamples - 1; ++i)
    {
        U_f = U_list[i] * U_f;
        U_f_list.emplace_back(U_f);

        U_b_list.insert(U_b_list.begin(), U_b);
        U_b = U_list[m_configs.nbSamples - 2 - i].adjoint() * U_b;
    }

    // Save the final unitary at this iteration
    m_uMats.emplace_back(U_f);

    // New pulse vector
    const std::vector<std::vector<double>>& oldPulses = m_pulses.back();
    std::vector<std::vector<double>> newPulses(oldPulses);
    for (int i = 0; i < m_configs.H_ops.size(); ++i)
    {
        for (int timeIdx = 0; timeIdx < m_configs.nbSamples - 1; ++timeIdx)
        {
            const auto& U = m_configs.targetU;
            const auto& H_ops = m_configs.H_ops;
            const auto P = U_b_list[timeIdx] * U;
            const auto Q = I * m_configs.dt * H_ops[i] * U_f_list[timeIdx];
            // Note: we will be using a global-phase insensitive optimization.
            // This simplifies the implementation and makes it consistent across optimization methods.
            const std::complex<double> du = -2.0*overlapCalc(P, Q)*overlapCalc(U_f_list[timeIdx], P);
            // Gradient-based update
            newPulses[i][timeIdx] = oldPulses[i][timeIdx] + m_configs.eps * du.real();
        }
        // Last sample is equal the one before
        newPulses[i][m_configs.nbSamples - 1] = newPulses[i][m_configs.nbSamples - 2];
    }
        
    return newPulses;
}

// ============= GRAPE ==========================
// - Required: { "dimension" : int } : system dimension (number of qubits)
// - Optional: { "grape-maxeval": int}: max number of iteration (default = 1000)
// - Optional: { "grape-ftol": double}: fidelity error tolerance, stop if achieved (default = 1e-10)
// - Required: { "target-U" : string or EigenMatrix } : 
//   If string, represents a Pauli observable type string
//  e.g. "X", "X1Z2", etc.
//   If matrix, expecting a unitary matrix whose size matches the system dimension.
//
// - Optional: { "static-H" : string} : Pauli observable observable type string
//  describing time-indedependent (static) Hamiltonian
//  e.g. 5.1234 Z1Z2
//
// - Required: { "control-H" : vector<string> } : list of Hamiltonian terms corresponding to the control pulses.
//
// - Optional: { "initial-pulses" : vector<vector<double>> }: initial pulses
//
// - Required: { "dt": double}: pulse sample duration
//
// - Optional: { "max-time" : double }: max control time horizon
// Note: initial-pulses and max-time are both optional *BUT* at least one of them
// must be provided.
// - Optional: { "eps": double}: gradient step size multiplier (default = 0.1 * (2 * pi) / max-time)
void PulseOptimGRAPE::setOptions(const HeterogeneousMap& in_options) 
{
    int dimension = 0;
    if (in_options.keyExists<int>("dimension")) 
    {
        dimension = in_options.get<int>("dimension");
    }

    if (dimension < 1)
    {
        xacc::error("Invalid 'dimension' parameter.");
        return;
    }
    
    m_nbIters = 1000;
    if (in_options.keyExists<int>("grape-maxeval")) 
    {
        m_nbIters = in_options.get<int>("grape-maxeval");
    }

    if (m_nbIters < 1)
    {
        xacc::error("Invalid 'max-iterations' parameter.");
        return;
    }
    
    m_tol = 1e-10;
    if (in_options.keyExists<double>("grape-ftol")) 
    {
        m_tol = in_options.get<double>("grape-ftol");
    }

    auto pauliMatrixUtil = xacc::getService<xacc::UnitaryMatrixUtil>("Pauli");
    const auto targetUmat = pauliMatrixUtil->fromHeterogeneousMap(in_options, dimension);
    
    std::string H0str;
    Eigen::MatrixXcd H0mat = Eigen::MatrixXcd::Zero(1 << dimension, 1 << dimension);
    if (in_options.stringExists("static-H")) 
    {
        H0str = in_options.getString("static-H");
    }

    if (!H0str.empty())
    {
        H0mat = pauliMatrixUtil->fromString(H0str, dimension);
    }

    std::vector<std::string> controlOps;    
    if (in_options.keyExists<std::vector<std::string>>("control-H")) 
    {
        controlOps = in_options.get<std::vector<std::string>>("control-H");
    }
    else
    {
        xacc::error("Missing 'control-H' parameter.");
        return;
    }
    std::vector<Eigen::MatrixXcd> H_ops;
    for (const auto& opStr : controlOps)
    {
        const auto hMat = pauliMatrixUtil->fromString(opStr, dimension);
        H_ops.emplace_back(hMat);
    }

    double dt = 0.0;
    if (in_options.keyExists<double>("dt")) 
    {
        dt = in_options.get<double>("dt");
    }
    else
    {
        xacc::error("Missing 'dt' parameter.");
        return;
    }

    int nbSamples = 0;
    if (in_options.keyExists<double>("max-time"))
    {
        const double tMax = in_options.get<double>("max-time");
        nbSamples = static_cast<int>(tMax/dt);
    }

    std::vector<std::vector<double>> initialPulses;
    if (in_options.keyExists<std::vector<std::vector<double>>>("initial-pulses"))
    {
        initialPulses = in_options.get<std::vector<std::vector<double>>>("initial-pulses");
        if (initialPulses.size() != H_ops.size() || initialPulses.empty())
        {
            xacc::error("If provided, 'initial-pulses' parameter must contain all control pulses.");
            return;
        }

        nbSamples = initialPulses[0].size(); 
        for (const auto& initPulse : initialPulses)
        {
            if (initPulse.empty() || initPulse.size() != nbSamples)
            {
                xacc::error("Invalid data array is provided in 'initial-pulses' parameter.");
                return;
            }
        }
    }
    else
    {
        // Initial pulses are not provided, constructed here
        for (int i = 0; i < H_ops.size(); ++i)
        {
            const std::vector<double> pulse = randPulse(nbSamples);
            initialPulses.emplace_back(std::move(pulse));
        }
    }

    const double endTime = initialPulses[0].size() * dt;
    double eps = 0.1 * (2 * M_PI) / endTime;
    if (in_options.keyExists<double>("eps")) 
    {
        eps = in_options.get<double>("eps");
    }

    struct GrapeConfigs configs;
    {
        configs.targetU = targetUmat;
        configs.H0 = H0mat;
        configs.H_ops = H_ops;
        configs.dt = dt;
        configs.nbSamples = nbSamples;
        configs.initial_pulses = initialPulses;
        configs.eps = eps;
    }

    m_optimizer = std::make_unique<GrapePulseOptim>(configs);
}

OptResult PulseOptimGRAPE::optimize()
{
    const auto optimRes = m_optimizer->optimize(m_nbIters, m_tol);
    std::vector<double> flattenPulses;
    for (const auto& pulse: optimRes.pulses)
    {
        flattenPulses.insert(flattenPulses.end(), pulse.begin(), pulse.end());
    }
    
    return std::make_pair(1.0 - optimRes.overlap, flattenPulses);
}
};