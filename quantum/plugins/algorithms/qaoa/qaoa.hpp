/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#pragma once

#include "Algorithm.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include "CompositeInstruction.hpp"

namespace xacc {
namespace algorithm {
class QAOA : public Algorithm 
{
public:
    bool initialize(const HeterogeneousMap& parameters) override;
    const std::vector<std::string> requiredParameters() const override;
    void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
    std::vector<double> execute(const std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<double> &parameters) override;
    const std::string name() const override { return "QAOA"; }
    const std::string description() const override { return ""; }
    DEFINE_ALGORITHM_CLONE(QAOA)
private:
    Observable* constructMaxCutHam(xacc::Graph* in_graph) const;
private:
    Observable* m_costHamObs;
    Observable* m_refHamObs;
    Accelerator* m_qpu;
    Optimizer* m_optimizer;
    std::shared_ptr<AlgorithmGradientStrategy> gradientStrategy;
    std::shared_ptr<CompositeInstruction> externalAnsatz;
    int m_nbSteps;
    std::string m_parameterizedMode;
};
} // namespace algorithm
} // namespace xacc
