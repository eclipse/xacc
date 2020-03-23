#include "GRAPE.hpp"
#include <unsupported/Eigen/MatrixFunctions>

namespace {
    const std::complex<double> I(0.0, 1.0);
    std::complex<double> overlapCalc(const Eigen::MatrixXcd& in_mat1, const Eigen::MatrixXcd& in_mat2)
    {
        const double dim = in_mat1.rows();
        return ((in_mat1.adjoint() * in_mat2).trace()) / dim;
    };
}

GrapePulseOptim::GrapePulseOptim(const GrapeConfigs& in_configs):
    m_configs(in_configs)
{
    m_pulses.emplace_back(in_configs.initial_pulses);
}

GrapeResult GrapePulseOptim::optimize(int in_nIters)
{
    m_pulses.reserve(in_nIters + 1);
    for (int i = 0; i < in_nIters; ++i)
    {
        const auto newPulses = evaluate();
        m_pulses.emplace_back(std::move(newPulses));
    }
    
    GrapeResult result;
    result.pulses = m_pulses.back();
    result.overlap = overlapCalc(m_configs.targetU, m_uMats.back()).real();
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
            const std::complex<double> du = - overlapCalc(P, Q);
            // Gradient-based update
            newPulses[i][timeIdx] = oldPulses[i][timeIdx] + m_configs.eps * du.real();
        }
        // Last sample is equal the one before
        newPulses[i, m_configs.nbSamples - 1] = newPulses[i, m_configs.nbSamples - 2];
    }
        
    return newPulses;
}