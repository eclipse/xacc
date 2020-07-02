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
#include <unsupported/Eigen/MatrixFunctions>
#include <numeric>
#include <random>

namespace {
Eigen::MatrixXcd X { Eigen::MatrixXcd::Zero(2, 2)};      
Eigen::MatrixXcd Y { Eigen::MatrixXcd::Zero(2, 2)};       
Eigen::MatrixXcd Z { Eigen::MatrixXcd::Zero(2, 2)};  
Eigen::MatrixXcd Id { Eigen::MatrixXcd::Identity(2, 2)};  

constexpr std::complex<double> I { 0.0, 1.0 };

int getTempId()
{
  static int tempIdCounter = 0;
  tempIdCounter++;
  return tempIdCounter;
}

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

std::vector<std::pair<size_t, size_t>> qubitPairCombinations(size_t N)
{
    std::string bitmask(2, 1);
    bitmask.resize(N, 0); 
    std::vector<std::pair<size_t, size_t>> result;
    do 
    {
        std::vector<size_t> bits;
        for (size_t i = 0; i < N; ++i) 
        {
            if (bitmask[i]) 
            {
                bits.emplace_back(i); 
            }
        }
        assert(bits.size() == 2);
        result.emplace_back(std::make_pair(bits[0], bits[1]));
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
    
    return result;
}

bool allClose(const Eigen::MatrixXcd& in_mat1, const Eigen::MatrixXcd& in_mat2, double in_tol = 1e-12)
{
  if (in_mat1.rows() == in_mat2.rows() && in_mat1.cols() == in_mat2.cols())
  {
    for (int i = 0; i < in_mat1.rows(); ++i)
    {
      for (int j = 0; j < in_mat1.cols(); ++j)
      {
        if (std::abs(in_mat1(i,j) - in_mat2(i, j)) > in_tol)
        {
          return false;
        }
      }
    }

    return true;
  }
  return false;
}
}

namespace xacc {
namespace circuits {
QFAST::QFAST():
    xacc::quantum::Circuit("QFAST")
{}

bool QFAST::expand(const xacc::HeterogeneousMap& runtimeOptions) 
{
    Eigen::MatrixXcd unitary;
    if (runtimeOptions.keyExists<Eigen::MatrixXcd>("unitary"))
    {
        unitary = runtimeOptions.get<Eigen::MatrixXcd>("unitary");
    }
    const auto nbQubits = [](size_t unitaryDim) {
        unsigned int ret = 0;
        while (unitaryDim > 1) 
        {
            unitaryDim >>= 1;
            ret++;
        }
    
        return ret;
    };
    m_nbQubits = nbQubits(unitary.rows());
    m_targetU = unitary;
    std::stringstream ss;
    ss << "Target U:\n" << m_targetU;
    xacc::info(ss.str());
    // Default trace distance limit = 0.01 (99% fidelity).
    m_distanceLimit = 0.01;
    if (runtimeOptions.keyExists<double>("trace-distance"))
    {
        m_distanceLimit = runtimeOptions.get<double>("trace-distance");
        if (m_distanceLimit < 1e-9 || m_distanceLimit >= 1.0)
        {
            xacc::error("Invalid trace distance setting.");
            return false;
        }
    }

    m_initialDepth = 1;
    if (runtimeOptions.keyExists<int>("initial-depth"))
    {
        m_initialDepth = runtimeOptions.get<int>("initial-depth");
        if (m_initialDepth < 1)
        {
            xacc::error("Invalid initial-depth setting.");
            return false;
        }
    }   

    // Default explore phase trace distance limit:
    m_exploreTraceDistanceLimit = std::min(0.1, 100 * m_distanceLimit);
    if (runtimeOptions.keyExists<double>("explore-trace-distance"))
    {
        m_exploreTraceDistanceLimit = runtimeOptions.get<double>("explore-trace-distance");
        if (m_exploreTraceDistanceLimit < 1e-9 || m_exploreTraceDistanceLimit >= 1.0)
        {
            xacc::error("Invalid 'explore-trace-distance' setting.");
            return false;
        }

        if (m_distanceLimit > m_exploreTraceDistanceLimit)
        {
            m_distanceLimit = m_exploreTraceDistanceLimit;
        }
    }

    m_locationModel = std::make_shared<LocationModel>(m_nbQubits);
    const auto decomposedResult = decompose();
    
    for (auto iter = decomposedResult.rbegin(); iter != decomposedResult.rend(); ++iter) 
    { 
        auto compInst = genericBlockToGates(*iter);
        addInstructions(compInst->getInstructions());
    } 

    return true;
}

const std::vector<std::string> QFAST::requiredKeys()
{
   return { "unitary" };
}

std::vector<QFAST::Block> QFAST::decompose()
{
    const auto exloreResults = explore();
    const auto refinedResult = refine(exloreResults);
    
    return refinedResult;
}

std::vector<QFAST::PauliReps> QFAST::explore()
{
    std::vector<QFAST::PauliReps> layers;
    bool alternativeLayer = false;
    
    for (int i = 0; i < m_initialDepth; ++i)
    {
        const auto& topology = alternativeLayer ? m_locationModel->buckets.second : m_locationModel->buckets.first;
        const auto& topologyPauli = alternativeLayer ? m_locationModel->bucketPaulis.second : m_locationModel->bucketPaulis.first;
        addLayer(layers, topology, topologyPauli);
        alternativeLayer = !alternativeLayer;
    }
   
    // Infinite loop
    for (;;)
    {
        const bool optResult = optimizeAtDepth(layers, m_exploreTraceDistanceLimit);
        if (optResult)
        {
            break;
        }
        else
        {
            const auto& topology = alternativeLayer ? m_locationModel->buckets.second : m_locationModel->buckets.first;
            const auto& topologyPauli = alternativeLayer ? m_locationModel->bucketPaulis.second : m_locationModel->bucketPaulis.first;
            // Add one more layer
            addLayer(layers, topology, topologyPauli);
            alternativeLayer = !alternativeLayer;
        }
    }
    
    return layers;
}

std::vector<QFAST::Block> QFAST::refine(const std::vector<QFAST::PauliReps>& in_rawResults)
{
    xacc::info("[Refine] Number of layers = " + std::to_string(in_rawResults.size()));
    auto refinedReps = in_rawResults;
    const auto fixedLoc = m_locationModel->fixLocations(refinedReps);
    assert(refinedReps.size() == fixedLoc.size());
    std::vector<double> initialParams; 
    // Only function params since the location has been fixed
    for (const auto& layer : refinedReps)
    {
        initialParams.insert(initialParams.end(), layer.funcValues.begin(), layer.funcValues.end());
    }
    
    const int nbParams = initialParams.size();
    const int maxEval = nbParams * 1000;

    // TODO: use gradient-based optimizer (e.g. Adam)
    // This is currently not possible since we don't know how to calculate the gradients.
    auto optimizer = xacc::getOptimizer("nlopt", 
        xacc::HeterogeneousMap { 
            std::make_pair("initial-parameters", initialParams),
            std::make_pair("nlopt-maxeval", maxEval),
            std::make_pair("nlopt-stopval", m_distanceLimit) 
    });

    OptFunction f(
        [&](const std::vector<double>& x, std::vector<double>& grad) 
        {
            Eigen::MatrixXcd accumU(Eigen::MatrixXcd::Identity(1ULL << m_nbQubits, 1ULL << m_nbQubits));
            std::vector<double> params;
            size_t idx = 0;
            size_t paramIdx = 0;
            bool alternativeLayer = false;
            for (auto& layer : refinedReps)
            {
                params.clear();
                params.assign(x.begin() + paramIdx, x.begin() + paramIdx + layer.funcValues.size());
                assert(params.size() == layer.funcValues.size());
                paramIdx += params.size();
                // Added the fixed location values (not being optimized)
                params.insert(params.end(), layer.locValues.begin(), layer.locValues.end());                
                layer.updateParams(params);
                idx++;
                const auto& topology = alternativeLayer ? m_locationModel->buckets.second : m_locationModel->buckets.first;
                const auto& topologyPauli = alternativeLayer ? m_locationModel->bucketPaulis.second : m_locationModel->bucketPaulis.first;
                accumU = accumU * layerToUnitaryMatrix(layer, topology, topologyPauli);
                alternativeLayer = !alternativeLayer;
            }

            const double costValue = evaluateCostFunc(accumU);
            return costValue;
        },
        nbParams);
    
    const auto result = optimizer->optimize(f);
    xacc::info("[Refine] Final trace distance = " + std::to_string(result.first));
    std::vector<double> optParams;
    size_t paramIdx = 0;
    for (auto& layer : refinedReps)
    {
        optParams.clear();
        optParams.assign(result.second.begin() + paramIdx, result.second.begin() + paramIdx + layer.funcValues.size());
        assert(optParams.size() == layer.funcValues.size());
        paramIdx += layer.funcValues.size();
        layer.funcValues = optParams;
    }

    // Double check trace distance:
    std::vector<Eigen::MatrixXcd> layerMat;
    {
        Eigen::MatrixXcd accumU(Eigen::MatrixXcd::Identity(1ULL << m_nbQubits, 1ULL << m_nbQubits));
        bool alternativeLayer = false;
        for (auto& layer : refinedReps)
        {
            const auto& topology = alternativeLayer ? m_locationModel->buckets.second : m_locationModel->buckets.first;
            const auto& topologyPauli = alternativeLayer ? m_locationModel->bucketPaulis.second : m_locationModel->bucketPaulis.first;
            layerMat.emplace_back(layerToUnitaryMatrix(layer, topology, topologyPauli));
            accumU = accumU * layerMat.back();
            alternativeLayer = !alternativeLayer;
        }

        assert(evaluateCostFunc(accumU) <= m_distanceLimit);
    }
   
    std::vector<QFAST::Block> resultBlocks;
    size_t blockIdx = 0;
    for (const auto& layer : refinedReps)
    {
        const std::pair<size_t, size_t> mapBits = (fixedLoc[blockIdx].first < fixedLoc[blockIdx].second) ? 
            std::make_pair(0, 1) : 
            std::make_pair(1, 0);
        const auto subPaulis = generateAllPaulis(2, { mapBits })[0];        
        assert(subPaulis.size() == layer.funcValues.size());
        Block newBlock;
        newBlock.qubits = fixedLoc[blockIdx];
        Eigen::MatrixXcd gateMat = Eigen::MatrixXcd::Zero(4, 4);
        for (size_t j = 0; j < subPaulis.size(); ++j)
        {
            gateMat += (layer.funcValues[j] * subPaulis[j]);
        }
        gateMat = I * gateMat;
        // Exponential: exp(i*H)
        newBlock.uMat = gateMat.exp();
        resultBlocks.emplace_back(newBlock);
        assert(allClose(layerMat[blockIdx], newBlock.toFullMat(m_nbQubits)));
        blockIdx++;
    }
    
    return resultBlocks;
}

void QFAST::addLayer(std::vector<QFAST::PauliReps>& io_currentLayers, const Topology& in_layerTopology, const TopologyPaulis& in_topologyPaulis) const
{
    assert(in_layerTopology.size() == in_topologyPaulis.size());
    assert(!in_layerTopology.empty());
    
    xacc::info("[Explore] Adding a layer. Number of layers = " + std::to_string(io_currentLayers.size() + 1)); 
    // Add a layer:
    QFAST::PauliReps newLayer;
    const double initialParam = 1.0 / in_topologyPaulis[0].size();
    newLayer.funcValues.assign(in_topologyPaulis[0].size(), initialParam);
    // Random:
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int i = 0; i < in_layerTopology.size(); ++i)
    {
        newLayer.locValues.emplace_back(dist(mt));
    }
    io_currentLayers.emplace_back(std::move(newLayer));
}
    
std::shared_ptr<CompositeInstruction> QFAST::genericBlockToGates(const QFAST::Block& in_genericBlock)
{
    // Use KAK to decompose block matrix (4x4) to gates:
    auto gateRegistry = xacc::getService<IRProvider>("quantum");
    auto kak = std::dynamic_pointer_cast<quantum::Circuit>(xacc::getService<Instruction>("kak"));
    const bool expandOk = kak->expand({ 
        std::make_pair("unitary", in_genericBlock.uMat),
        std::make_pair("qubits", std::vector<int> { 
            static_cast<int>(in_genericBlock.qubits.first), 
            static_cast<int>(in_genericBlock.qubits.second) })
    });
    
    std::stringstream ssInfo;
    ssInfo << "[Decompose] Bit1: " << in_genericBlock.qubits.first << "; Bit2: " << in_genericBlock.qubits.second << "\n";
    ssInfo << "[Decompose] Matrix: \n" << in_genericBlock.uMat << "\n";
    xacc::info(ssInfo.str());

    if (!expandOk)
    {
        std::stringstream ss;
        ss << "Failed to decompose unitary matrix: \n"  << in_genericBlock.uMat;
        xacc::error(ss.str());
    } 

    return kak;
}
 
std::vector<std::vector<Eigen::MatrixXcd>> QFAST::generateAllPaulis(size_t in_nbQubits, const Topology& in_topology)
{
    assert(in_nbQubits >= 2);
    
    // Get all Paulis
    const auto& paulis = getPaulisMap(in_nbQubits);

    std::vector<std::vector<Eigen::MatrixXcd>> result;
    
    for (const auto& pair : in_topology)
    {
        const auto bit1 = pair.first;
        const auto bit2 = pair.second;
        auto& paulisForPair = result.emplace_back(std::vector<Eigen::MatrixXcd>{});
        for (const auto& op1 : SinglePauliOps)
        {
            for (const auto& op2 : SinglePauliOps)
            {
                paulisForPair.emplace_back(paulis[computePauliMapIndex(in_nbQubits, { op1, bit1 }, { op2, bit2 })]);
            }
        }
        assert(result.back().size() == 16);
    }

    return result;
}

bool QFAST::optimizeAtDepth(std::vector<PauliReps>& io_repsToOpt, double in_targetDistance) const 
{
    assert(!io_repsToOpt.empty());
   
    int nbParams = 0;
    std::vector<double> initialParams; 
    for (const auto& layer : io_repsToOpt)
    {
        initialParams.insert(initialParams.end(), layer.funcValues.begin(), layer.funcValues.end());
        initialParams.insert(initialParams.end(), layer.locValues.begin(), layer.locValues.end());
        nbParams += layer.nbParams();
    }
    
    const int maxEval = nbParams * 100;

    // TODO: use gradient-based optimizer (e.g. Adam)
    // This is currently not possible since we don't know how to calculate the gradients.
    auto optimizer = xacc::getOptimizer("nlopt", 
        xacc::HeterogeneousMap { 
            std::make_pair("initial-parameters", initialParams),
            std::make_pair("nlopt-maxeval", maxEval),
            std::make_pair("nlopt-stopval", in_targetDistance)  
    });

    OptFunction f(
        [&](const std::vector<double>& x, std::vector<double>& grad) 
        {
            Eigen::MatrixXcd accumU(Eigen::MatrixXcd::Identity(1ULL << m_nbQubits, 1ULL << m_nbQubits));
            std::vector<double> params;
            size_t idx = 0;
            bool alternativeLayer = false;
            size_t paramIdx = 0;
            for (auto& layer : io_repsToOpt)
            {
                params.clear();
                params.assign(x.begin() + paramIdx, x.begin() + paramIdx + layer.nbParams());
                assert(params.size() == layer.nbParams());
                paramIdx += params.size();
                layer.updateParams(params);
                
                const auto maxLocIdx = std::distance(layer.locValues.begin(), std::max_element(layer.locValues.begin(), layer.locValues.end()));
                // This is a non-differentiable implementation.
                // This can be converted to a pseudo-differentable using exp functions.
                for(int i = 0; i < layer.locValues.size(); ++i)
                {
                    layer.locValues[i] = (i == maxLocIdx) ? 1.0 : 0.0;
                }
                
                idx++;
                const auto& topology = alternativeLayer ? m_locationModel->buckets.second : m_locationModel->buckets.first;
                const auto& topologyPauli = alternativeLayer ? m_locationModel->bucketPaulis.second : m_locationModel->bucketPaulis.first;
                accumU = accumU * layerToUnitaryMatrix(layer, topology, topologyPauli);
                alternativeLayer = !alternativeLayer;
            }

            const double costValue = evaluateCostFunc(accumU);
            return costValue;
        },
        nbParams);
    
    const auto result = optimizer->optimize(f);
    xacc::info("[Explore] Final trace distance = " + std::to_string(result.first));
    
    // Save the optimal params for these layers.
    // Hence, this will be the starting point 
    // when adding a new layer.
    std::vector<double> optParams;
    int idx = 0;
    size_t paramIdx = 0;

    for (auto& layer : io_repsToOpt)
    {
        optParams.clear();
        optParams.assign(result.second.begin() + paramIdx, result.second.begin() + paramIdx + layer.nbParams());
        assert(optParams.size() == layer.nbParams());
        paramIdx += layer.nbParams();
        layer.updateParams(optParams);
        idx++;
    }

    return (result.first < in_targetDistance);
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

Eigen::MatrixXcd QFAST::layerToUnitaryMatrix(const PauliReps& in_layer, const Topology& in_layerTopology, const TopologyPaulis& in_topologyPaulis) const
{
    std::vector<Eigen::MatrixXcd> gateMatOfPair;
    for (size_t i = 0; i < in_layerTopology.size(); ++i)
    {
        auto& gateMat = gateMatOfPair.emplace_back(Eigen::MatrixXcd::Zero(1ULL << m_nbQubits, 1ULL << m_nbQubits));
        const auto& paulis = in_topologyPaulis[i];
        assert(paulis.size() == in_layer.funcValues.size());
        for (size_t j = 0; j < paulis.size(); ++j)
        {
            gateMat += (in_layer.funcValues[j] * paulis[j]);
        }
    }
        

    Eigen::MatrixXcd hermMat(Eigen::MatrixXcd::Zero(1ULL << m_nbQubits, 1ULL << m_nbQubits));

    assert(in_layer.locValues.size() == in_layerTopology.size());

    for (size_t i = 0; i < in_layerTopology.size(); ++i)
    {
        hermMat += in_layer.locValues[i] * gateMatOfPair[i];
    }

    hermMat = I * hermMat;
    // Exponential: exp(i*H)
    Eigen::MatrixXcd result = hermMat.exp();
    return result;
}

QFAST::LocationModel::LocationModel(size_t in_nbQubits):
    nbQubits(in_nbQubits),
    locations(qubitPairCombinations(in_nbQubits))
{
    std::size_t const halfSize = locations.size() / 2;
    Topology firstHalf(locations.begin(), locations.begin() + halfSize);
    Topology secondHalf(locations.begin() + halfSize, locations.end());
    buckets = std::make_pair(firstHalf, secondHalf);
    bucketPaulis = std::make_pair(generateAllPaulis(nbQubits, buckets.first), generateAllPaulis(nbQubits, buckets.second));
}

std::vector<std::pair<size_t, size_t>> QFAST::LocationModel::fixLocations(std::vector<PauliReps>& io_repsToFix) const
{
    std::vector<std::pair<size_t, size_t>>  result;
    bool alternativeLayer = false;

    for (auto& layer : io_repsToFix)
    {
        const auto& topology = alternativeLayer ? buckets.second : buckets.first;
        const auto& topologyPauli = alternativeLayer ? bucketPaulis.second : bucketPaulis.first;
        assert(topology.size() == layer.locValues.size());
        assert(topologyPauli[0].size() == layer.funcValues.size());

        const auto& locVals = layer.locValues;
        const auto locIdx = std::distance(locVals.begin(), std::max_element(locVals.begin(), locVals.end()));
        alternativeLayer = !alternativeLayer;
        result.emplace_back(topology[locIdx]);

        for (size_t i = 0; i < layer.locValues.size(); ++i)
        {
            if (i == locIdx)
            {
                layer.locValues[i] = 1.0;
            }
            else
            {
                layer.locValues[i] = 0.0;
            }
        }
    }

    return result;
}

Eigen::MatrixXcd  QFAST::Block::toFullMat(size_t in_totalDim) const
{
    assert(in_totalDim > qubits.first && in_totalDim > qubits.second);
    const auto matSize = 1ULL << in_totalDim;
    Eigen::MatrixXcd resultMat = Eigen::MatrixXcd::Identity(matSize, matSize);
    // Qubit index list (0->(dim-1))
    std::vector<size_t> index_list(in_totalDim);
    std::iota(index_list.begin(), index_list.end(), 0);
    const std::vector<size_t> idx { in_totalDim - qubits.second - 1, in_totalDim - qubits.first - 1 };    			      
    for (size_t k = 0; k < matSize; ++k)
    { 
        std::vector<std::complex<double>> oldColumn(matSize);
        for (size_t i = 0; i < matSize; ++i)
        {
            oldColumn[i] = resultMat(i, k);
        }

        for (size_t i = 0; i < matSize; ++i)
        {
            size_t local_i = 0;
            for (size_t l = 0; l < idx.size(); ++l)
            {
                local_i |= ((i >> idx[l]) & 1) << l;
            }
                
            std::complex<double> res = 0.;
            for (size_t j = 0; j < (1ULL<<idx.size()); ++j)
            {
                size_t locIdx = i;
                for (size_t l = 0; l < idx.size(); ++l)
                {
                    if (((j >> l)&1) != ((i >> idx[l])&1))
                    {
                        locIdx ^= (1ULL << idx[l]);
                    }
                }
                    
                res += oldColumn[locIdx] * uMat(local_i, j);
            }

            resultMat(i, k) = res;
        }
    }

    return resultMat;
}
}
}