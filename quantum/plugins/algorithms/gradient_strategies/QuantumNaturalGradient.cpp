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
#include <optional>
using namespace xacc;

namespace {
std::optional<xacc::quantum::PauliOperator> getGenerator(const InstPtr& in_inst) 
{
    if (in_inst->name() == "Rx")
    {
        return xacc::quantum::PauliOperator({{ in_inst->bits()[0], "X" }}, 0.5);
    }
    if (in_inst->name() == "Ry")
    {
        return xacc::quantum::PauliOperator({{ in_inst->bits()[0], "Y" }}, 0.5);
    }
    if (in_inst->name() == "Rz")
    {
        return xacc::quantum::PauliOperator({{ in_inst->bits()[0], "Z" }}, 0.5);
    }

    return std::nullopt;
}
}
namespace xacc {
namespace algorithm {
bool QuantumNaturalGradient::initialize(const HeterogeneousMap in_parameters)
{
    m_gradientStrategy.reset();
    m_layers.clear();
    m_nbMetricTensorKernels = 0;
    m_metricTermToIdx.clear();
    // User can provide a regular gradient strategy.
    // Note: this natural gradient requires a base gradient strategy.
    if (in_parameters.pointerLikeExists<AlgorithmGradientStrategy>("gradient-strategy"))
    {
        m_gradientStrategy = xacc::as_shared_ptr(in_parameters.getPointerLike<AlgorithmGradientStrategy>("gradient-strategy"));
    }

    // No base gradient strategy provided.
    // Must have observable specified.
    if (!m_gradientStrategy)
    {
        if (in_parameters.pointerLikeExists<Observable>("observable"))
        {
            auto observable = xacc::as_shared_ptr(in_parameters.getPointerLike<Observable>("observable"));
            // Create a default gradient strategy based on the observable.
            m_gradientStrategy = xacc::getService<AlgorithmGradientStrategy>("central");
            m_gradientStrategy->initialize({ std::make_pair("observable", observable)});
        }
        else
        {
            xacc::error("'observable' must be provided if no 'gradient-strategy' specified.");
            return false;
        }
    }

    return true;
}

std::vector<std::shared_ptr<CompositeInstruction>> QuantumNaturalGradient::getGradientExecutions(std::shared_ptr<CompositeInstruction> in_circuit, const std::vector<double>& in_x) 
{
    auto baseGradientKernels = m_gradientStrategy->getGradientExecutions(in_circuit, in_x);
    m_nbMetricTensorKernels = 0;
    // Layering the circuit:
    m_layers = ParametrizedCircuitLayer::toParametrizedLayers(in_circuit);
    m_nbParams = in_x.size();
    std::vector<std::string> paramNames;
    assert(in_circuit->getVariables().size() == in_x.size());
    for (const auto& param : in_circuit->getVariables())
    {
        paramNames.emplace_back(param);
    }

    std::vector<std::shared_ptr<CompositeInstruction>>  metricTensorKernels;
    for (auto& layer : m_layers)
    {
        auto kernels = constructMetricTensorSubCircuit(layer, paramNames, in_x);
        
        // Insert *non-identity* kernels only
        size_t kernelIdx = 0;
        const auto isIdentityTerm = [](xacc::quantum::PauliOperator& in_pauli){
            return in_pauli.getNonIdentitySubTerms().size() == 0;
        };
        
        for (size_t i = 0; i < layer.kiTerms.size(); ++i)
        {
            if (!isIdentityTerm(layer.kiTerms[i]))
            {
                metricTensorKernels.emplace_back(kernels[kernelIdx]);
                m_metricTermToIdx.emplace(layer.kiTerms[i].toString(), metricTensorKernels.size() - 1);
            }
            kernelIdx++;
        }        
        
        for (size_t i = 0; i < layer.kikjTerms.size(); ++i)
        {
            if (!isIdentityTerm(layer.kikjTerms[i]))
            {
                metricTensorKernels.emplace_back(kernels[kernelIdx]);
                m_metricTermToIdx.emplace(layer.kikjTerms[i].toString(), metricTensorKernels.size());
            }
            kernelIdx++;
        }  
    }

    m_nbMetricTensorKernels = metricTensorKernels.size();
    baseGradientKernels.insert(baseGradientKernels.end(), metricTensorKernels.begin(), metricTensorKernels.end());
    return baseGradientKernels;
}

void QuantumNaturalGradient::compute(std::vector<double>& out_dx, std::vector<std::shared_ptr<AcceleratorBuffer>> in_results)
{
    assert(m_nbMetricTensorKernels > 0 && in_results.size() > m_nbMetricTensorKernels);
    const auto iterPos = in_results.begin() + (in_results.size() - m_nbMetricTensorKernels);
    // Split the results: regular gradient results + metric tensor results.
    std::vector<std::shared_ptr<AcceleratorBuffer>> baseResults;
    baseResults.assign(in_results.begin(), iterPos);
    std::vector<std::shared_ptr<AcceleratorBuffer>> metricTensorResults;
    metricTensorResults.assign(iterPos, in_results.end());
    std::vector<double> rawDx = out_dx;
    // Calculate the raw gradients (using a regular gradient strategy)
    m_gradientStrategy->compute(rawDx, baseResults);
    // Solve the natural gradient equation:
    const auto gMat = constructMetricTensorMatrix(metricTensorResults);
    arma::dvec gradients(rawDx); 
    arma::dvec newGrads = arma::solve(gMat, gradients);
    // std::cout << "Regular gradients:\n" << gradients << "\n";
    // std::cout << "Natural gradients:\n" << newGrads << "\n";
    out_dx = arma::conv_to<std::vector<double>>::from(newGrads); 
}

ObservedKernels QuantumNaturalGradient::constructMetricTensorSubCircuit(ParametrizedCircuitLayer& io_layer, 
                                                    const std::vector<std::string>& in_varNames, 
                                                    const std::vector<double>& in_varVals) const
{
    assert(in_varNames.size() == in_varVals.size());
    // We need to observe all the generators of parametrized gates plus products of them.
    std::vector<xacc::quantum::PauliOperator> KiTerms;
    std::vector<xacc::quantum::PauliOperator> KiKjTerms;

    for (const auto& parOp: io_layer.ops)
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
    std::vector<std::shared_ptr<xacc::CompositeInstruction>> obsComp;
    std::vector<double> resolvedParams;
    for (const auto& op : io_layer.preOps)
    {
        if (op->isParameterized() && op->getParameter(0).isVariable())
        {
            const auto varName = op->getParameter(0).as<std::string>();
            circuitToObs->addVariable(varName);
            const auto iter = std::find(in_varNames.begin(), in_varNames.end(), varName); 
            assert(iter != in_varNames.end());
            const size_t idx = std::distance(in_varNames.begin(), iter);
            assert(idx < in_varVals.size());
            resolvedParams.emplace_back(in_varVals[idx]);
        }
    }

    circuitToObs->addInstructions(io_layer.preOps);
    // Resolves the pre-ops now that we have passed that layer.
    auto resolvedCirc = resolvedParams.empty() ? circuitToObs :  circuitToObs->operator()(resolvedParams);

    io_layer.kiTerms = KiTerms;
    io_layer.kikjTerms = KiKjTerms;
    
    for (auto& term : KiTerms)
    {   
        auto obsKernels = term.observe(resolvedCirc);
        assert(obsKernels.size() == 1);
        obsComp.emplace_back(obsKernels[0]);
    }

    for (auto& term : KiKjTerms)
    {
        auto obsKernels = term.observe(resolvedCirc);
        assert(obsKernels.size() == 1);
        obsComp.emplace_back(obsKernels[0]);
    }

    const size_t NUM_KI_TERMS = io_layer.paramInds.size();
    const size_t NUM_KIKJ_TERMS = io_layer.paramInds.size() * io_layer.paramInds.size();
    // Validate the expected count.
    assert(obsComp.size() == NUM_KI_TERMS + NUM_KIKJ_TERMS);
    return obsComp;
}

arma::dmat QuantumNaturalGradient::constructMetricTensorMatrix(const std::vector<std::shared_ptr<xacc::AcceleratorBuffer>>& in_results)
{   
    arma::dmat gMat(m_nbParams, m_nbParams, arma::fill::zeros);
    size_t blockIdx = 0;
    for (auto& layer : m_layers)
    {
        const auto nbParamsInBlock = layer.paramInds.size();
        // Constructs the block diagonal matrices
        arma::dmat blockMat(nbParamsInBlock, nbParamsInBlock, arma::fill::zeros);

        for (size_t i = 0; i < nbParamsInBlock; ++i)
        {
            for(size_t j = 0; j < nbParamsInBlock; ++j)
            {
                // Entry = <KiKj> - <Ki><Kj>
                // second_order_ev[i, j] - first_order_ev[i] * first_order_ev[j]
                auto firstOrderTerm1 = layer.kiTerms[i];
                const double factor1 = firstOrderTerm1.coefficient().real();
                auto firstOrderTerm2 = layer.kiTerms[j];
                const double factor2 = firstOrderTerm2.coefficient().real();
                auto secondOrderTerm = layer.kiTerms[i] * layer.kiTerms[j];
                
                const auto getExpectationForTerm = [&](xacc::quantum::PauliOperator& in_pauli){
                    if (m_metricTermToIdx.find(in_pauli.toString()) == m_metricTermToIdx.end())
                    {
                        return 1.0;
                    }
                    return in_results[m_metricTermToIdx[in_pauli.toString()]]->getExpectationValueZ();
                };

                const double firstOrderTerm1Exp = getExpectationForTerm(firstOrderTerm1);
                const double firstOrderTerm2Exp = getExpectationForTerm(firstOrderTerm2);
                const double secondOrderTermExp = getExpectationForTerm(secondOrderTerm);
                const double value = factor1*factor2*(secondOrderTermExp - firstOrderTerm1Exp * firstOrderTerm2Exp);
                blockMat(i,j) = value;
            }
        }

        for (size_t i = 0; i < nbParamsInBlock; ++i)
        {
            for(size_t j = 0; j < nbParamsInBlock; ++j)
            {
                gMat(blockIdx + i, blockIdx + j) =  blockMat(i,j);
            }
        }

        blockIdx += nbParamsInBlock;
    }

    return gMat;
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