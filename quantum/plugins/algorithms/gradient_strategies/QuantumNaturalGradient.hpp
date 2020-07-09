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
#include "CompositeInstruction.hpp"
#include "PauliOperator.hpp"

using namespace xacc;

namespace xacc {
namespace algorithm {
// Represents a parametrized circuit layer:
// we push the layer boundary as far as possible (greedy)
struct ParametrizedCircuitLayer
{
    // Gates that precede the layer
    std::vector<InstPtr> preOps; 
    // Parametrized operators in the layer
    std::vector<InstPtr> ops;
    // Corresponding optimization parameter indices
    std::vector<size_t> paramInds;
    // Gates that succeed the layer
    std::vector<InstPtr> postOps;
    // Partition the circuit into layers.
    static std::vector<ParametrizedCircuitLayer> toParametrizedLayers(const std::shared_ptr<xacc::CompositeInstruction>& in_circuit);
};

class QuantumNaturalGradient : public AlgorithmGradientStrategy
{
public:
    virtual bool isNumerical() const override { return true; }
    virtual bool initialize(const xacc::HeterogeneousMap in_parameters) override;
    virtual std::vector<std::shared_ptr<xacc::CompositeInstruction>> getGradientExecutions(std::shared_ptr<xacc::CompositeInstruction> in_circuit, const std::vector<double>& in_x) override;
    virtual void compute(std::vector<double>& out_dx, std::vector<std::shared_ptr<xacc::AcceleratorBuffer>> in_results) override;
    virtual const std::string name() const override { return "quantum-natural-gradient"; }
    virtual const std::string description() const override { return ""; }

private:
    // Constructs circuits to observe Fubini-Study metric tensor elements.
    std::vector<std::shared_ptr<xacc::CompositeInstruction>> constructMetricTensorSubCircuit(const ParametrizedCircuitLayer& in_layer) const; 

private:
    // The *regular* gradient strategy service whose gradients will
    // be 'modified' via this QNG metric tensor.
    std::shared_ptr<AlgorithmGradientStrategy> m_gradientStrategy;
};
}
}
