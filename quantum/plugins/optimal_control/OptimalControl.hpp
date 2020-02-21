#pragma once
#include <memory>
#include "Optimizer.hpp"
#include "GOAT.hpp"

namespace xacc {
class ControlOptimizer : public Optimizer 
{
public:
    virtual OptResult optimize(OptFunction& function) override;
    virtual void setOptions(const HeterogeneousMap& in_options) override;
    const std::string name() const override { return "optimal"; }
    const std::string description() const override { return ""; }   
private:
    // Unlike other optimizer, this optimal control service requires 
    // proper set-up (e.g. method to use, optimal control problem description)
    bool m_initialized = false; 
    std::unique_ptr<GOAT_PulseOptim> m_goatOptimizer;
    std::unique_ptr<GoatHamiltonian> m_hamiltonian;
};
} 