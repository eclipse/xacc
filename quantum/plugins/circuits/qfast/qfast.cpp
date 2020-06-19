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

#include "qfast.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include <unsupported/Eigen/KroneckerProduct>

namespace {
Eigen::MatrixXcd X { Eigen::MatrixXcd::Zero(2, 2)};      
Eigen::MatrixXcd Y { Eigen::MatrixXcd::Zero(2, 2)};       
Eigen::MatrixXcd Z { Eigen::MatrixXcd::Zero(2, 2)};  
Eigen::MatrixXcd Id { Eigen::MatrixXcd::Identity(2, 2)};  

constexpr std::complex<double> I { 0.0, 1.0 };
const std::vector<Eigen::MatrixXcd>& getPaulisMap(int n)
{
    static std::vector<std::vector<Eigen::MatrixXcd>> cachedMap;
    static bool init = false;
    if (!init)
    {
        X << 0, 1, 1, 0;
        Y << 0, -I, I, 0;
        Z << 1, 0, 0, -1;
        cachedMap.emplace_back(std::vector<Eigen::MatrixXcd> { Id });
        cachedMap.emplace_back(std::vector<Eigen::MatrixXcd> { Id, X, Y, Z });
        init = true;
    }

    if (n < cachedMap.size())
    {
        return cachedMap[n];
    }

    std::vector<Eigen::MatrixXcd> norderPaulis;
    
    for (const auto& paulisNminus1 : getPaulisMap(n - 1))
    {
        for (const auto& paulis1 : getPaulisMap(1))
        {
            norderPaulis.emplace_back(Eigen::kroneckerProduct(paulisNminus1, paulis1));
        }
    }
    
    cachedMap.emplace_back(std::move(norderPaulis));
    return cachedMap[n];
}

enum class PauliOp : size_t { I = 0, X = 1, Y = 2, Z = 3 };
const PauliOp SinglePauliOps[] = { PauliOp::I, PauliOp::X, PauliOp::Y, PauliOp::Z };

// Index to the all Paulis (at a specific number of qubits):
// Note: we limit ourselves to the case of two operators only.
// Indexing scheme: Pauli string as base-4 bitstring:
// with I = 0, X = 1, Y = 2, Z = 3
// e.g. XXY = 1 * 4^2 + 1 & 4^1 + 2 * 4^0 = 22 (in base 10)
size_t computePauliMapIndex(size_t in_nbQubits, const std::pair<PauliOp, size_t>& in_op1, const std::pair<PauliOp, size_t>& in_op2)
{
    assert(in_op1.second != in_op2.second);
    assert(in_op1.second < in_nbQubits);
    assert(in_op2.second < in_nbQubits);
    const auto opToIndx = [&in_nbQubits](const std::pair<PauliOp, size_t>& in_op) -> size_t {
        return static_cast<size_t>(in_op.first) * std::pow(4, in_nbQubits - in_op.second - 1);
    };
    return opToIndx(in_op1) + opToIndx(in_op2);
}

}

namespace xacc {
namespace circuits {
QFAST::QFAST():
    xacc::quantum::Circuit("QFAST")
{}

bool QFAST::expand(const xacc::HeterogeneousMap& runtimeOptions) 
{
    // !! TEMP CODE !!
    m_nbQubits = 3;
    m_topology = { { 0, 1}, { 1, 2} };
    // CCNOT gate:
    m_targetU = Eigen::MatrixXcd::Identity(8, 8);
    m_targetU.block(6, 6, 2, 2) = X;
    m_distanceLimit = 0.001;
    // !! TEMP CODE !!

    // Generate and cache the Pauli decompose basis:
    m_allPaulis = generateAllPaulis(m_nbQubits, m_topology);
    
    const auto decomposedResult = decompose();
    for (const auto& block : decomposedResult)
    {
        auto compInst = genericBlockToGates(block);
        addInstructions(compInst->getInstructions());
    }

    // TODO
    return true;
}

const std::vector<std::string> QFAST::requiredKeys()
{
   // TODO
   return {};
}

std::vector<QFAST::Block> QFAST::decompose()
{
    const auto exloreResults = explore();
    const auto refinedResult = refine(exloreResults);
    
    return pauliRepsToBlocks(refinedResult);
}

std::vector<QFAST::Block> QFAST::pauliRepsToBlocks(const std::vector<PauliReps>& in_pauliRep) const 
{
    // TODO
    return {};
}

std::vector<QFAST::PauliReps> QFAST::explore()
{
    std::vector<QFAST::PauliReps> layers;
    addLayer(layers);
    // Coarse limit during exploration:
    const double EXPLORE_PHASE_TARGET_DISTANCE = 100 * m_distanceLimit;
    // Infinite loop
    for (;;)
    {
        const bool optResult = optimizeAtDepth(layers, EXPLORE_PHASE_TARGET_DISTANCE);
        if (optResult)
        {
            break;
        }
        else
        {
            // Add one more layer
            addLayer(layers);
        }
    }
    
    return layers;
}

std::vector<QFAST::PauliReps> QFAST::refine(const std::vector<QFAST::PauliReps>& in_rawResults)
{
    // TODO:
    return in_rawResults;
}

void QFAST::addLayer(std::vector<QFAST::PauliReps>& io_currentLayers)
{
    // TODO:
}
    
std::shared_ptr<CompositeInstruction> QFAST::genericBlockToGates(const QFAST::Block& in_genericBlock)
{
    // TODO
    return nullptr;
}
 
std::vector<Eigen::MatrixXcd> QFAST::generateAllPaulis(size_t in_nbQubits, const Topology& in_topology)
{
    // Minimum: 3 qubits
    assert(in_nbQubits > 2);
    // Must have at least 2 pairs of connected qubits.
    assert(in_topology.size() > 1);
    // Get all Paulis
    const auto& paulis = getPaulisMap(in_nbQubits);

    std::vector<Eigen::MatrixXcd> result;
    
    for (const auto& pair : in_topology)
    {
        const auto bit1 = pair.first;
        const auto bit2 = pair.second;
        for (const auto& op1 : SinglePauliOps)
        {
            for (const auto& op2 : SinglePauliOps)
            {
                result.emplace_back(paulis[computePauliMapIndex(in_nbQubits, { op1, bit1 }, { op2, bit2 })]);
            }
        }
    }

    return result;
}

bool QFAST::optimizeAtDepth(std::vector<PauliReps>& io_repsToOpt, double in_targetDistance) const 
{
    // TODO:
    return false;
}


double QFAST::evaluateCostFunc(const Eigen::MatrixXcd in_U) const
{
    assert(in_U.rows() == m_targetU.rows());
    assert(in_U.cols() == m_targetU.cols());
    const auto d = in_U.cols();
    const auto product = in_U.adjoint() * m_targetU;
    const double traceNorm =  std::norm(product.trace());
    return std::sqrt(1.0 - traceNorm / (d*d));
}
}
}