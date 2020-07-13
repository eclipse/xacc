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
#include <armadillo>

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
    // Tensor matrix terms:
    std::vector<xacc::quantum::PauliOperator> kiTerms;
    std::vector<xacc::quantum::PauliOperator> kikjTerms;
    // Partition the circuit into layers.
    static std::vector<ParametrizedCircuitLayer> toParametrizedLayers(const std::shared_ptr<xacc::CompositeInstruction>& in_circuit);
};

using ObservedKernels = std::vector<std::shared_ptr<xacc::CompositeInstruction>>;
class QuantumNaturalGradient : public AlgorithmGradientStrategy
{
public:
    virtual bool isNumerical() const override { return true; }
    virtual bool initialize(const xacc::HeterogeneousMap in_parameters) override;
    virtual ObservedKernels getGradientExecutions(std::shared_ptr<xacc::CompositeInstruction> in_circuit, const std::vector<double>& in_x) override;
    virtual void compute(std::vector<double>& out_dx, std::vector<std::shared_ptr<xacc::AcceleratorBuffer>> in_results) override;
    virtual void setFunctionValue(const double expValue) override { m_gradientStrategy->setFunctionValue(expValue); }
    virtual const std::string name() const override { return "quantum-natural-gradient"; }
    virtual const std::string description() const override { return ""; }

private:
    // Constructs circuits to observe Fubini-Study metric tensor elements.
    ObservedKernels constructMetricTensorSubCircuit(ParametrizedCircuitLayer& io_layer, 
                                                    const std::vector<std::string>& in_varNames, 
                                                    const std::vector<double>& in_varVals) const; 
    arma::dmat constructMetricTensorMatrix(const std::vector<std::shared_ptr<xacc::AcceleratorBuffer>>& in_results);

private:
    // The *regular* gradient strategy service whose gradients will
    // be 'modified' via this QNG metric tensor.
    std::shared_ptr<AlgorithmGradientStrategy> m_gradientStrategy;
    // Cache the circuit layer structure to reconstruct metric tensor.
    std::vector<ParametrizedCircuitLayer> m_layers;
    size_t m_nbMetricTensorKernels;
    // Keeps track of the term and the index in the kernel sequence.
    std::unordered_map<std::string, size_t> m_metricTermToIdx;
    size_t m_nbParams;
};
}
}
