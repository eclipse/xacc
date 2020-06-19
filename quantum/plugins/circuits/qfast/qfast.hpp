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

#include "Circuit.hpp"
#include "IRProvider.hpp"
#include <Eigen/Dense>

namespace xacc {
namespace circuits {

class QFAST : public xacc::quantum::Circuit 
{
public:
    QFAST();
    bool expand(const xacc::HeterogeneousMap& runtimeOptions) override;
    const std::vector<std::string> requiredKeys() override;
    DEFINE_CLONE(QFAST);
private:
    // We only perform the decomposition to the fixed
    // block size of 2 (2-qubit gates).
    // There is no use for decomposition to larger block sizes
    // since our instruction set doesn't support those.
    static constexpr int NATIVE_BLOCK_SIZE = 2;
    // Represents a generic gate block:
    // i.e. 4x4 matrix for block size = 2.
    using BlockMatrix = Eigen::Matrix<std::complex<double>, 1ULL << NATIVE_BLOCK_SIZE, 1ULL << NATIVE_BLOCK_SIZE>;
    
    // Topology is a list of qubit pairs which we will apply decomposed gates.
    using Topology = std::vector<std::pair<size_t, size_t>>;
    
    struct Block 
    {
        // Qubit indices
        std::pair<size_t, size_t> qubits;
        // U matrix
        BlockMatrix uMat;
    };

    struct PauliReps 
    {
        // F and A lists (see Algo. 2 in https://arxiv.org/pdf/2003.04462.pdf)
        std::vector<double> funcValues;
        std::vector<double> locValues;
    };

    // Returns decomposed Blocks
    std::vector<Block> decompose();
    // Algorithm #3
    std::vector<PauliReps> explore();
    // Algorithm #4
    std::vector<PauliReps> refine(const std::vector<PauliReps>& in_rawResults);
    // Algorithm #5
    void addLayer(std::vector<PauliReps>& io_currentLayers);

    // Gate Instantiation: i.e. KAK
    std::shared_ptr<CompositeInstruction> genericBlockToGates(const Block& in_genericBlock);
    
    // Pauli Helper:
    const std::vector<Eigen::MatrixXcd>& getAllPaulis() const { return m_allPaulis; }
    // Generate the list of all Paulis for a particular order (number of qubits)
    // and topology (i.e. qubit pairs)
    // This is the set of matrices that we can use to decompose the target unitary
    // into two-qubit blocks (constrained by the topology)
    static std::vector<Eigen::MatrixXcd> generateAllPaulis(size_t in_nbQubits, const Topology& in_topology);
private:
    Eigen::MatrixXcd m_targetU;
    // All Pauli tensors
    std::vector<Eigen::MatrixXcd> m_allPaulis;
    size_t m_nbQubits;
    Topology m_topology;
};

} // namespace circuits
} // namespace xacc