#pragma once 
#include "xacc.hpp"
#include "DmsimCpuVisitor.hpp"

#include "util_cpu.h"
#include "dmsim_cpu_omp.hpp"

namespace xacc {

class DmsimCpuAccelerator : public Accelerator {
public:
    void initialize(const HeterogeneousMap &params = {}) override;
    void updateConfiguration(const HeterogeneousMap &config) override;
    HeterogeneousMap getProperties() override;
    const std::vector<std::string> configurationKeys() override;
    void execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::shared_ptr<CompositeInstruction> CompositeInstruction) override;
    void execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<std::shared_ptr<CompositeInstruction>> CompositeInstruction) override;

    const std::string name() const override { return "dmsimcpu"; }
    const std::string description() const override { return "DMSIM Accelerator for Gate Model QC on CPU"; }
private:
    std::shared_ptr<DmsimCpuVisitor> m_visitor;
    int m_shots;
};
}