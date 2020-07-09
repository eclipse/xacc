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
bool QuantumNaturalGradient::optionalParameters(const HeterogeneousMap in_parameters)
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

std::vector<ParametrizedCircuitLayer> ParametrizedCircuitLayer::toParametrizedLayers(const std::shared_ptr<xacc::CompositeInstruction>& in_circuit)
{
    const auto variables = in_circuit->getVariables();
    if (variables.empty())
    {
        xacc::error("Input circuit is not parametrized.");
    }

    // Assemble parametrized circuit layer:
    std::vector<ParametrizedCircuitLayer> layers; 
    auto& currentLayer = layers.emplace_back();
    std::set<size_t> qubitsInLayer;
    for (size_t instIdx = 0; instIdx < in_circuit->nInstructions(); ++instIdx)
    {
        const auto& inst = in_circuit->getInstruction(instIdx);
        if (!inst->isParameterized())
        {
            currentLayer.ops.emplace_back(inst);
        }
        else
        {
            const auto& instParam = inst->getParameters()[0];
            if (instParam.isVariable())
            {
                const auto varName = instParam.as<std::string>();
                const auto iter = std::find(variables.begin(), variables.end(), varName); 
                assert(iter != variables.end());
                assert(inst->bits().size() == 1);
                const auto bitIdx = inst->bits()[0];
                // This qubit line was already acted upon in this layer by a parametrized gate.
                // Hence, start a new layer.
                if (xacc::container::contains(qubitsInLayer, bitIdx))
                {
                    assert(!currentLayer.ops.empty() && !currentLayer.paramInds.empty());
                    auto& currentLayer = layers.emplace_back();
                    qubitsInLayer.clear();
                }

                currentLayer.ops.emplace_back(inst);
                currentLayer.paramInds.emplace_back(std::distance(iter, variables.begin()));
                qubitsInLayer.emplace(bitIdx);
            } 
            else
            {
                currentLayer.ops.emplace_back(inst);
            }
        }
    }

    return layers;
}    
}
}