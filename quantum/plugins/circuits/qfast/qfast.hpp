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
    using TopologyPaulis = std::vector<std::vector<Eigen::MatrixXcd>>;
    struct Block 
    {
        // Qubit indices
        std::pair<size_t, size_t> qubits;
        // U matrix
        BlockMatrix uMat;
        // Convert this local block (acting on two qubits)
        // to the full matrix representation (total system).
        Eigen::MatrixXcd toFullMat(size_t in_totalDim) const;
    };

    struct PauliReps 
    {
        // F and A lists (see Algo. 2 in https://arxiv.org/pdf/2003.04462.pdf)
        std::vector<double> funcValues;
        std::vector<double> locValues;
        size_t nbParams() const { return funcValues.size() + locValues.size(); }
        void updateParams(const std::vector<double>& in_params) {
            assert(in_params.size() == nbParams());
            size_t idx = 0;
            for (auto& funcParam : funcValues)
            {
                funcParam = in_params[idx];
                idx++;
            }

            for (auto& locParam : locValues)
            {
                locParam = in_params[idx];
                idx++;
            }
        }
    };

    // Location model: to convert generic PauliReps into
    // fixed location (a pair of qubits)
    struct LocationModel
    {
        LocationModel(size_t in_nbQubits);
        // Returns *fixed* locations
        std::vector<std::pair<size_t, size_t>> fixLocations(std::vector<PauliReps>& io_repsToFix) const;
        size_t nbQubits;
        Topology locations;
        std::pair<Topology, Topology> buckets;
        std::pair<TopologyPaulis, TopologyPaulis> bucketPaulis;
    };

    // Returns decomposed Blocks
    std::vector<Block> decompose();
    // Algorithm #3
    std::vector<PauliReps> explore();
    // Algorithm #4
    std::vector<Block> refine(const std::vector<PauliReps>& in_rawResults);
    // Algorithm #5
    void addLayer(std::vector<PauliReps>& io_currentLayers, const Topology& in_layerTopology, const TopologyPaulis& in_topologyPaulis) const;
    // Gate Instantiation: i.e. KAK
    std::shared_ptr<CompositeInstruction> genericBlockToGates(const Block& in_genericBlock);
    
    // Generate the list of all Paulis for a particular order (number of qubits)
    // and topology (i.e. qubit pairs)
    // This is the set of matrices that we can use to decompose the target unitary
    // into two-qubit blocks (constrained by the topology)
    static std::vector<std::vector<Eigen::MatrixXcd>>  generateAllPaulis(size_t in_nbQubits, const Topology& in_topology);
    // Evaluate cost function for the input unitary against the target unitary
    double evaluateCostFunc(const Eigen::MatrixXcd in_U) const;
    
    // Returns true if the target trace distance can be met.
    // The depth is determined by the io_repsToOpt vector,
    // optimization results are updated in-place.
    bool optimizeAtDepth(std::vector<PauliReps>& io_repsToOpt, double in_targetDistance) const;

    // Compute the Pauli Rep. of a layer to unitary matrix
    Eigen::MatrixXcd layerToUnitaryMatrix(const PauliReps& in_layer, const Topology& in_layerTopology, const TopologyPaulis& in_topologyPaulis) const;
private:
    Eigen::MatrixXcd m_targetU;
    size_t m_nbQubits;
    size_t m_initialDepth;
    // Trace/Fidelity distance limit:
    double m_distanceLimit;
    // Explore phase distance limit:
    double m_exploreTraceDistanceLimit;
    std::shared_ptr<LocationModel> m_locationModel;
};

} // namespace circuits
} // namespace xacc