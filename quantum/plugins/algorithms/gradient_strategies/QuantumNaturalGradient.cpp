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

#include "QuantumNaturalGradient.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include <cassert>

using namespace xacc;

namespace {
std::optional<xacc::quantum::PauliOperator> getGenerator(const InstPtr& in_inst) 
{
    if (in_inst->name() == "Rx")
    {
        return xacc::quantum::PauliOperator(0.5, "X" + std::to_string(in_inst->bits()[0]));
    }
    if (in_inst->name() == "Ry")
    {
        return xacc::quantum::PauliOperator(0.5, "Y" + std::to_string(in_inst->bits()[0]));

    }
    if (in_inst->name() == "Rz")
    {
        return xacc::quantum::PauliOperator(0.5, "Z" + std::to_string(in_inst->bits()[0]));
    }

    return std::nullopt;
}
}
namespace xacc {
namespace algorithm {
bool QuantumNaturalGradient::initialize(const HeterogeneousMap in_parameters)
{
    // TODO: 
    return true;
}

std::vector<std::shared_ptr<CompositeInstruction>> QuantumNaturalGradient::getGradientExecutions(std::shared_ptr<CompositeInstruction> in_circuit, const std::vector<double>& in_x) 
{
    // TODO:
    return { in_circuit };
}

void QuantumNaturalGradient::compute(std::vector<double>& out_dx, std::vector<std::shared_ptr<AcceleratorBuffer>> in_results)
{
    // TODO
}

std::vector<std::shared_ptr<xacc::CompositeInstruction>> QuantumNaturalGradient::constructMetricTensorSubCircuit(const ParametrizedCircuitLayer& in_layer) const
{
    // We need to observe all the generators of parametrized gates plus products of them.
    std::vector<xacc::quantum::PauliOperator> KiTerms;
    std::vector<xacc::quantum::PauliOperator> KiKjTerms;

    for (const auto& parOp: in_layer.ops)
    {
        auto opGenerator = getGenerator(parOp);
        assert(opGenerator.has_value());
        KiTerms.emplace_back(opGenerator.value());
    }

    for (const auto& genOp1 : KiTerms)
    {
        for (const auto& genOp2 : KiTerms)
        {
            auto kikjOp = genOp1 * genOp2;
            assert(kikjOp.nTerms() == 1);
            KiKjTerms.emplace_back(kikjOp);
        }
    }

    auto gateRegistry = xacc::getService<IRProvider>("quantum");
    auto circuitToObs = gateRegistry->createComposite("__LAYER__COMPOSITE__");
    circuitToObs->addInstructions(in_layer.preOps);

    std::vector<std::shared_ptr<xacc::CompositeInstruction>> obsComp;
    for (auto& term : KiTerms)
    {
        auto obsKernels = term.observe(circuitToObs);
        assert(obsKernels.size() == 1);
        obsComp.emplace_back(obsKernels[0]);
    }

    for (auto& term : KiKjTerms)
    {
        auto obsKernels = term.observe(circuitToObs);
        assert(obsKernels.size() == 1);
        obsComp.emplace_back(obsKernels[0]);
    }

    return obsComp;
}

std::vector<ParametrizedCircuitLayer> ParametrizedCircuitLayer::toParametrizedLayers(const std::shared_ptr<xacc::CompositeInstruction>& in_circuit)
{
    const auto variables = in_circuit->getVariables();
    if (variables.empty())
    {
        xacc::error("Input circuit is not parametrized.");
    }

    // Assemble parametrized circuit layer:
    std::vector<ParametrizedCircuitLayer> layers; 
    ParametrizedCircuitLayer currentLayer;
    std::set<size_t> qubitsInLayer;
    for (size_t instIdx = 0; instIdx < in_circuit->nInstructions(); ++instIdx)
    {
        const auto& inst = in_circuit->getInstruction(instIdx);
        if (!inst->isParameterized())
        {
            if (currentLayer.ops.empty())
            {
                currentLayer.preOps.emplace_back(inst);
            }
            else
            {
                currentLayer.postOps.emplace_back(inst);
            }
        }
        else
        {
            const auto& instParam = inst->getParameter(0);
            if (instParam.isVariable())
            {
                const auto varName = instParam.as<std::string>();
                const auto iter = std::find(variables.begin(), variables.end(), varName); 
                assert(iter != variables.end());
                assert(inst->bits().size() == 1);
                const auto bitIdx = inst->bits()[0];
                // This qubit line was already acted upon in this layer by a parametrized gate.
                // Hence, start a new layer.
                if (!currentLayer.postOps.empty() || xacc::container::contains(qubitsInLayer, bitIdx))
                {
                    assert(!currentLayer.ops.empty() && !currentLayer.paramInds.empty());
                    layers.emplace_back(std::move(currentLayer));
                    qubitsInLayer.clear();
                }
                currentLayer.ops.emplace_back(inst);
                currentLayer.paramInds.emplace_back(std::distance(iter, variables.begin()));
                qubitsInLayer.emplace(bitIdx);
            } 
            else
            {
                if (currentLayer.ops.empty())
                {
                    currentLayer.preOps.emplace_back(inst);
                }
                else
                {
                    currentLayer.postOps.emplace_back(inst);
                }       
            }
        }
    }

    assert(!currentLayer.ops.empty() && !currentLayer.paramInds.empty());
    layers.emplace_back(std::move(currentLayer));
    
    // Need to make a copy to do two rounds:
    auto layersCopied = layers;

    // Add pre-ops and post-ops:
    for (size_t i = 1; i < layers.size(); ++i)
    {
        // Pre-ops:
        auto& thisLayerPreOps = layers[i].preOps;
        const auto& previousLayerOps = layers[i - 1].ops;
        const auto& previousLayerPreOps = layers[i - 1].preOps;
        const auto& previousLayerPostOps = layers[i - 1].postOps;

        // Insert pre-ops, ops, and *raw* post-ops of the previous layer to the begining
        thisLayerPreOps.insert(thisLayerPreOps.begin(), previousLayerPostOps.begin(), previousLayerPostOps.end());
        thisLayerPreOps.insert(thisLayerPreOps.begin(), previousLayerOps.begin(), previousLayerOps.end());
        thisLayerPreOps.insert(thisLayerPreOps.begin(), previousLayerPreOps.begin(), previousLayerPreOps.end());
    }

    for (int i = layers.size() - 2; i >= 0; --i)
    {
        // Post-ops:
        auto& thisLayerPostOps = layers[i].postOps;
        const auto& nextLayerOps = layers[i + 1].ops;
        const auto& nextLayerPostOps = layers[i + 1].postOps;
        // Get the original pre-ops
        const auto& nextLayerPreOps = layersCopied[i + 1].preOps;

        // Insert next layer pre-ops, ops and its post op
        thisLayerPostOps.insert(thisLayerPostOps.end(), nextLayerPreOps.begin(), nextLayerPreOps.end());
        thisLayerPostOps.insert(thisLayerPostOps.end(), nextLayerOps.begin(), nextLayerOps.end());
        thisLayerPostOps.insert(thisLayerPostOps.end(), nextLayerPostOps.begin(), nextLayerPostOps.end());
    }

    // Verify:
    for (const auto& layer: layers)
    {
        assert(layer.preOps.size() + layer.ops.size() + layer.postOps.size() == in_circuit->nInstructions());
        assert(!layer.paramInds.empty() && !layer.ops.empty());
    }

    return layers;
}    
}
}