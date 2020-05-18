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

namespace xacc {
namespace algorithm {
class QuantumPhaseEstimation : public Algorithm 
{
public:
    bool initialize(const HeterogeneousMap& parameters) override;
    const std::vector<std::string> requiredParameters() const override;
    void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
    std::vector<double> execute(const std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<double> &parameters) override;
    const std::string name() const override { return "QPE"; }
    const std::string description() const override { return ""; }
    DEFINE_ALGORITHM_CLONE(QuantumPhaseEstimation)

private:
    CompositeInstruction* m_eigenStatePrep;
    CompositeInstruction* m_oracle;
    Accelerator* m_qpu;
    HeterogeneousMap m_params;
};
} // namespace algorithm
} // namespace xacc
