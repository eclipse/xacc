#pragma once

#include "Algorithm.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include "CompositeInstruction.hpp"
#include "IRProvider.hpp"

namespace xacc {
namespace algorithm {
class maxcut_qaoa : public Algorithm
{
public:
    bool initialize(const HeterogeneousMap& parameters) override;
    const std::vector<std::string> requiredParameters() const override;
    void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
    std::vector<double> execute(const std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<double> &x) override;
    const std::string name() const override { return "maxcut_qaoa"; }
    const std::string description() const override { return ""; }
    DEFINE_ALGORITHM_CLONE(maxcut_qaoa)
private:
    Observable* constructMaxCutHam(xacc::Graph* in_graph) const;
private:
    Observable* m_costHamObs;
    Observable* m_refHamObs;
    Accelerator* m_qpu;
    Optimizer* m_optimizer;
    std::shared_ptr<AlgorithmGradientStrategy> gradientStrategy;
    std::shared_ptr<CompositeInstruction> externalAnsatz;
    std::shared_ptr<CompositeInstruction> m_single_exec_kernel;
    int m_nbSteps;
    std::string m_parameterizedMode;
    std::string m_initializationMode = "default";
    xacc::Graph* m_graph;
    bool m_maximize = false;
    CompositeInstruction* m_initial_state;
};
} // namespace algorithm
} // namespace xacc
