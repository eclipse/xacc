#pragma once
#include <Eigen/Dense>
#include "OptimalControl.hpp"

struct GrapeConfigs
{
    // targetU: Target unitary evolution operator.
    Eigen::MatrixXcd targetU;
    // H0: Static Hamiltonian (that cannot be tuned by the control fields)
    Eigen::MatrixXcd H0;
    // H_ops: A list of operators that can be tuned in the Hamiltonian via the control fields.
    std::vector<Eigen::MatrixXcd> H_ops;
    // dt: duration between pulse samples
    double dt;
    // nbSamples: total number of data samples
    int nbSamples;
    // initial_pulses: vector of initial pulses (as vectors)
    std::vector<std::vector<double>> initial_pulses;
    // Gradient step size multiplier
    double eps;
};

struct GrapeResult
{
    double overlap;
    std::vector<std::vector<double>> pulses;
};

class GrapePulseOptim
{
public:
    GrapePulseOptim(const GrapeConfigs& in_configs);
    GrapeResult optimize(int in_nIters);

private:
    // Single iteration calculation
    std::vector<std::vector<double>> evaluate();
private:
    GrapeConfigs m_configs;
    std::vector<std::vector<std::vector<double>>> m_pulses;
    std::vector<Eigen::MatrixXcd> m_uMats; 
};

namespace xacc {
// Public GRAPE pulse optimization interface
class PulseOptimGRAPE : public PulseOptim
{   
public:
    const std::string name() const override { return "GRAPE"; }
    const std::string description() const override { return ""; }
    bool initialize(const HeterogeneousMap& in_options) override;
    OptResult optimize() override;
private:
    std::unique_ptr<GrapePulseOptim> m_optimizer;
    int m_nbIters;
};
}