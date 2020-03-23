#pragma once
#include <memory>
#include "Optimizer.hpp"
#include <Eigen/Dense>

namespace xacc {
// Pulse Optimization service interface
class PulseOptim: public Identifiable
{
public:
    virtual bool initialize(const HeterogeneousMap& in_options) = 0;
    virtual OptResult optimize() = 0;
};

class ControlOptimizer : public Optimizer 
{
public:
    virtual OptResult optimize(OptFunction& function) override;
    virtual OptResult optimize() override;
    virtual void setOptions(const HeterogeneousMap& in_options) override;
    const std::string name() const override { return "quantum-control"; }
    const std::string description() const override { return ""; }
private:
    // Unlike other optimizer, this optimal control service requires 
    // proper set-up (e.g. method to use, optimal control problem description)
    bool m_initialized = false; 
    std::shared_ptr<PulseOptim> m_pulseOptim;
};

// Util to calculate the unitary matrix from string
class UnitaryMatrixUtil : public Identifiable
{
public:
    virtual Eigen::MatrixXcd fromString(const std::string& in_unitaryString, int in_dimension) = 0;
    virtual Eigen::MatrixXcd fromHeterogeneousMap(const HeterogeneousMap& in_options, int in_dimension, const std::string& in_fieldName = "") = 0;
};
} 