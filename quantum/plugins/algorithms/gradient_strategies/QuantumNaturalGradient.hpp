/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
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
#include "AlgorithmGradientStrategy.hpp"
using namespace xacc;

namespace xacc {
namespace algorithm {
class QuantumNaturalGradient : public AlgorithmGradientStrategy
{
public:
virtual bool optionalParameters(const xacc::HeterogeneousMap in_parameters) override;
virtual std::vector<std::shared_ptr<xacc::CompositeInstruction>> getGradientExecutions(std::shared_ptr<xacc::CompositeInstruction> in_circuit, const std::vector<double>& in_x) override;
virtual void compute(std::vector<double>& out_dx, std::vector<std::shared_ptr<xacc::AcceleratorBuffer>> in_results) override;
virtual const std::string name() const override { return "quantum-natural-gradient"; }
virtual const std::string description() const override { return ""; }
};
}
}
