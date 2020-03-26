#pragma once
#include <memory>
#include "Optimizer.hpp"
#include <Eigen/Dense>

namespace xacc {
class ControlOptimizer : public Optimizer 
{
public:
    virtual OptResult optimize(OptFunction& function) override;
    virtual OptResult optimize() override;
    virtual void setOptions(const HeterogeneousMap& in_options) override;
    const std::string name() const override { return "quantum-control"; }
    const std::string description() const override { return ""; }
private:
    std::shared_ptr<Optimizer> m_pulseOptim;
};

// Util to calculate the unitary matrix from string
class UnitaryMatrixUtil : public Identifiable
{
public:
    virtual Eigen::MatrixXcd fromString(const std::string& in_unitaryString, int in_dimension) = 0;
    virtual Eigen::MatrixXcd fromHeterogeneousMap(const HeterogeneousMap& in_options, int in_dimension, const std::string& in_fieldName = "") = 0;
};
} 