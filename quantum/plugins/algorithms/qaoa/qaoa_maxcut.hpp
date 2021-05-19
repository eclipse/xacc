/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   A.M. Santana - initial API and implementation
 *******************************************************************************/

#pragma once

#include "Algorithm.hpp"
#include "IRProvider.hpp"
#include "CompositeInstruction.hpp"
#include "AlgorithmGradientStrategy.hpp"

namespace xacc {
namespace algorithm {
class maxcut_qaoa : public Algorithm
{
public:
    bool initialize(const HeterogeneousMap& parameters) override;
    const std::vector<std::string> requiredParameters() const override;
    void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
    std::vector<double> execute(const std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<double> &x) override;
    const std::string name() const override { return "maxcut-qaoa"; }
    const std::string description() const override { return ""; }
    DEFINE_ALGORITHM_CLONE(maxcut_qaoa)
private:
    std::shared_ptr<Observable> constructMaxCutHam(xacc::Graph* in_graph) const;
private:
    std::shared_ptr<Observable> m_costHamObs;
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
    CompositeInstruction* m_initial_state = nullptr;
    bool m_shuffleTerms = false;
};
} // namespace algorithm
} // namespace xacc
