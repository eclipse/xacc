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
#include <unsupported/Eigen/NumericalDiff>
#include <numeric>
#include <random>
#include "json.hpp"
#include "xacc_config.hpp"

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
bool makeDirectory(const std::string& in_path) 
{
    int ret = mkdir(in_path.c_str(), S_IRWXU | S_IRGRP |  S_IXGRP | S_IROTH | S_IXOTH);
    return (ret == 0);
}
// In-place softmax function 
template <typename Iter>
void softmax(Iter iterBegin, Iter iterEnd)
{
    using ValueType = typename std::iterator_traits<Iter>::value_type;
    static_assert(std::is_floating_point<ValueType>::value, "Softmax function only applicable for floating types");
    const auto maxElement { *std::max_element(iterBegin, iterEnd) };
    std::transform(iterBegin, iterEnd, iterBegin, [&](const ValueType x) { 
        // Use a very large factor to make this *one-hot*
        return std::exp(500.0 * (x - maxElement)); 
    });
    const ValueType expTol = std::accumulate(iterBegin, iterEnd, 0.0);
    std::transform(iterBegin, iterEnd, iterBegin, std::bind(std::divides<ValueType>(), std::placeholders::_1, expTol));  
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
    // Default cache file:
    std::string qfastCacheFileName = "qfast.cache";
    const std::string rootPath(XACC_INSTALL_DIR);
    const std::string cacheDir = rootPath + "/tmp";
    if (!directoryExists(cacheDir) && !makeDirectory(cacheDir)) 
    {
        xacc::warning("Failed to initialize QFAST cache directory. No cache look-up.");
    }
    
    // If user specified another file
    if (runtimeOptions.stringExists("cache-file-name"))
    {
        qfastCacheFileName = runtimeOptions.getString("cache-file-name");
    }

    if (!m_cache.initialize(cacheDir + "/" + qfastCacheFileName))
    {
        xacc::warning("Failed to initialize QFAST cache. No cache look-up.");
    }

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

    m_optimizer.reset();
    if (runtimeOptions.pointerLikeExists<Optimizer>("optimizer"))
    {
        m_optimizer = xacc::as_shared_ptr(runtimeOptions.getPointerLike<Optimizer>("optimizer"));
    }

    m_locationModel = std::make_shared<LocationModel>(m_nbQubits);
    
    const auto cacheLookupResult = m_cache.getCache(m_targetU);

    const auto decomposedResult = cacheLookupResult.empty() ? 
                                    // No cache, run decompose:
                                    decompose() :
                                    // cache found, just use it.
                                    cacheLookupResult;
    
    // Cache the decomposed results,
    // if it was not found (i.e. decomposed in the previous step)
    if (cacheLookupResult.empty())
    {
        if (m_cache.addCacheEntry(m_targetU, decomposedResult))
        {
            xacc::info("Caching the QFAST result to file succeeded.");
        }
        else
        {
            xacc::info("Caching the QFAST result to file FAILED.");
        }
    }

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

    RefineDiffFunctor diffFunctor(this, refinedReps, nbParams);
    Eigen::NumericalDiff<RefineDiffFunctor> numDiff(diffFunctor);    
    
    auto optimizer = m_optimizer ? m_optimizer : xacc::getOptimizer("nlopt");

    const bool needGrads = optimizer->isGradientBased();    
    
    // Handle optimizer specific configurations.
    // e.g. they may use different key names.
    optimizer->appendOption("initial-parameters", initialParams);
    if (optimizer->name() == "nlopt") 
    {
        optimizer->appendOption("nlopt-maxeval", maxEval);
        optimizer->appendOption("nlopt-stopval", m_distanceLimit);
        optimizer->appendOption("nlopt-lower-bounds", std::vector<double>(nbParams, -M_PI));
        optimizer->appendOption("nlopt-upper-bounds", std::vector<double>(nbParams, M_PI));
    }

    if (optimizer->name() == "mlpack") 
    {
        optimizer->appendOption("mlpack-max-iter", maxEval);
        // This is a *refine* optimization,
        // hence the step size must be small.
        optimizer->appendOption("mlpack-step-size", 0.001);
        // Set the tolerance so that it will terminate when the
        // trace distance is sufficiently converged.
        optimizer->appendOption("mlpack-tolerance", m_distanceLimit/20.0);
        
        if (optimizer->get_algorithm() == "adam")
        {
            optimizer->appendOption("adam-exact-objective", true);
        }
    }
    
    if (needGrads)
    {
        xacc::info("[Refine] A gradient-based optimizer was selected. Gradients will be computed.");
    }

    OptFunction f(
        [&](const std::vector<double>& x, std::vector<double>& grad) 
        {
            const Eigen::VectorXd inputParams = Eigen::Map<const Eigen::VectorXd>(x.data(), x.size());
            // Just 1 output:
            Eigen::VectorXd outVal(1);
            numDiff(inputParams, outVal);
            const double costValue = outVal(0);
            // If grads are needed, compute them.
            if (needGrads)
            {
                Eigen::MatrixXd gradients(1, x.size());
                numDiff.df(inputParams, gradients);
                assert(gradients.rows() == 1 && gradients.cols() == x.size());
                grad.clear();
                for (int i = 0; i < x.size(); ++i)
                {
                    grad.emplace_back(gradients(0, i));
                }
            }
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

int QFAST::RefineDiffFunctor::operator()(const InputType& in_x, ValueType& out_fvec) const
{
    // Should only have one output value.
    assert(values() == 1);
    const auto nbQubits = m_parent->m_nbQubits;
    const auto& locationModel = m_parent->m_locationModel;

    Eigen::MatrixXcd accumU(Eigen::MatrixXcd::Identity(1ULL << nbQubits, 1ULL << nbQubits));
    std::vector<double> params;
    size_t idx = 0;
    size_t paramIdx = 0;
    bool alternativeLayer = false;
    auto optLayers = m_layerConfigs;
    for (auto& layer : optLayers)
    {
        params.clear();
        params.assign(in_x.begin() + paramIdx, in_x.begin() + paramIdx + layer.funcValues.size());
        assert(params.size() == layer.funcValues.size());
        paramIdx += params.size();
        // Added the fixed location values (not being optimized)
        params.insert(params.end(), layer.locValues.begin(), layer.locValues.end());                
        layer.updateParams(params);
        idx++;
        auto& topology = alternativeLayer ? locationModel->buckets.second : locationModel->buckets.first;
        auto& topologyPauli = alternativeLayer ? locationModel->bucketPaulis.second : locationModel->bucketPaulis.first;
        accumU = accumU * m_parent->layerToUnitaryMatrix(layer, topology, topologyPauli);
        alternativeLayer = !alternativeLayer;
    }

    const double costValue = m_parent->evaluateCostFunc(accumU);    
    out_fvec[0] = costValue;
    return 0;
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

    auto optimizer = m_optimizer ? m_optimizer : xacc::getOptimizer("nlopt");
    const bool needGrads = optimizer->isGradientBased();    
    
    // Handle optimizer specific configurations.
    // e.g. they may use different key names.
    optimizer->appendOption("initial-parameters", initialParams);
    std::vector<double> lowerBounds;
    std::vector<double> upperBounds;
    for (const auto& layer : io_repsToOpt)
    {
        for (int i = 0; i < layer.funcValues.size(); ++i)
        {
            lowerBounds.emplace_back(-M_PI);
            upperBounds.emplace_back(M_PI);
        }
        for (int i = 0; i < layer.locValues.size(); ++i)
        {
            lowerBounds.emplace_back(0.0);
            upperBounds.emplace_back(1.0);
        }
    }
    assert(lowerBounds.size() == nbParams && upperBounds.size() == nbParams);

    if (optimizer->name() == "nlopt") 
    {
        optimizer->appendOption("nlopt-maxeval", maxEval);
        optimizer->appendOption("nlopt-stopval", in_targetDistance);
        optimizer->appendOption("nlopt-lower-bounds", lowerBounds);
        optimizer->appendOption("nlopt-upper-bounds", upperBounds);
    }

    if (optimizer->name() == "mlpack") 
    {
        optimizer->appendOption("mlpack-max-iter", maxEval);
        optimizer->appendOption("mlpack-step-size", 0.01);
        optimizer->appendOption("mlpack-tolerance", in_targetDistance/100.0);
        if (optimizer->get_algorithm() == "adam")
        {
            optimizer->appendOption("adam-exact-objective", true);
        }
    }
    
    if (needGrads)
    {
        xacc::info("[Explore] A gradient-based optimizer was selected. Gradients will be computed.");
    }

    ExploreDiffFunctor diffFunctor(this, io_repsToOpt, nbParams);
    Eigen::NumericalDiff<ExploreDiffFunctor, Eigen::NumericalDiffMode::Central> numDiff(diffFunctor);  
    
    OptFunction f(
        [&](const std::vector<double>& x, std::vector<double>& grad) 
        {
            const Eigen::VectorXd inputParams = Eigen::Map<const Eigen::VectorXd>(x.data(), x.size());
            // Just 1 output:
            Eigen::VectorXd outVal(1);
            numDiff(inputParams, outVal);
            const double costValue = outVal(0);
            
            // If grads are needed, compute them.
            if (needGrads)
            {
                Eigen::MatrixXd gradients(1, x.size());
                numDiff.df(inputParams, gradients);
                assert(gradients.rows() == 1 && gradients.cols() == x.size());
                grad.clear();
                for (int i = 0; i < x.size(); ++i)
                {
                    grad.emplace_back(gradients(0, i));
                }
            }

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

    // Try to fix the location params here
    // and validate the target explore distance has been meet.
    // This is to catch the cases whereby soft-max failed to localize the location parameters,
    // e.g. the optimizer generated two location parameters that are exactly the same.
    m_locationModel->fixLocations(io_repsToOpt);
    bool alternativeLayer = false;
    Eigen::MatrixXcd accumU(Eigen::MatrixXcd::Identity(1ULL << m_nbQubits, 1ULL << m_nbQubits));

    for (const auto& layer : io_repsToOpt)
    {
        const auto& topology = alternativeLayer ? m_locationModel->buckets.second : m_locationModel->buckets.first;
        const auto& topologyPauli = alternativeLayer ? m_locationModel->bucketPaulis.second : m_locationModel->bucketPaulis.first;
        accumU = accumU * layerToUnitaryMatrix(layer, topology, topologyPauli);
        alternativeLayer = !alternativeLayer;
    }

    // Use the actual cost function after fixing the location 
    // as the criteria to terminate the `explore` phase.
    // Note: The optimizer may find a lower cost function when
    // allowing a linear combination of multiple locations.
    // We have used *soft-max* to prevent most of that
    // unless the optimizer created two identical location parameters at a layer.    
    return (evaluateCostFunc(accumU) < in_targetDistance);
}

int QFAST::ExploreDiffFunctor::operator()(const InputType& in_x, ValueType& out_fvec) const
{
    // Should only have one output value.
    assert(values() == 1);
    const auto nbQubits = m_parent->m_nbQubits;
    const auto& locationModel = m_parent->m_locationModel;
    Eigen::MatrixXcd accumU(Eigen::MatrixXcd::Identity(1ULL << nbQubits, 1ULL << nbQubits));
    std::vector<double> params;
    size_t idx = 0;
    bool alternativeLayer = false;
    size_t paramIdx = 0;
    auto repsToOpt = m_layerConfigs;
    for (auto& layer : repsToOpt)
    {
        params.clear();
        params.assign(in_x.begin() + paramIdx, in_x.begin() + paramIdx + layer.nbParams());
        assert(params.size() == layer.nbParams());
        paramIdx += params.size();
        layer.updateParams(params);
        // Softmax location parameters:
        softmax(layer.locValues.begin(), layer.locValues.end());
        idx++;
        const auto& topology = alternativeLayer ? locationModel->buckets.second : locationModel->buckets.first;
        const auto& topologyPauli = alternativeLayer ? locationModel->bucketPaulis.second : locationModel->bucketPaulis.first;
        accumU = accumU * m_parent->layerToUnitaryMatrix(layer, topology, topologyPauli);
        alternativeLayer = !alternativeLayer;
    }

    const double costValue = m_parent->evaluateCostFunc(accumU);
    out_fvec[0] = costValue;
    return 0;
}

double QFAST::computeTraceDistance(const Eigen::MatrixXcd& in_mat1, const Eigen::MatrixXcd& in_mat2)
{
    assert(in_mat1.rows() == in_mat2.rows());
    assert(in_mat1.cols() == in_mat2.cols());
    const auto d = in_mat1.cols();
    const auto product = in_mat1.adjoint() * in_mat2;
    const double traceNorm =  std::norm(product.trace());
    return std::sqrt(1.0 - traceNorm / (d*d));
}

double QFAST::evaluateCostFunc(const Eigen::MatrixXcd& in_U) const
{
    return computeTraceDistance(in_U, m_targetU);
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

bool QFAST::DecomposedResultCache::initialize(const std::string& in_filePath)
{
    fileName = in_filePath;
    if (!fileExists(in_filePath))
    {
        // Creates the file (empty)
        FILE* hFile = fopen(fileName.c_str(), "w");
        if (hFile == NULL) 
        {
           return false;
        }
        fclose(hFile);
        
        // Nothing in the cache.
        return true;
    }

    // Has the file -> read
    std::ifstream fileStream(in_filePath);
    std::string fileString;
    fileStream.seekg(0, std::ios::end);   
    fileString.reserve(fileStream.tellg());
    fileStream.seekg(0, std::ios::beg);
    fileString.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    
    if (!fileString.empty() && !fromJsonString(fileString))
    {
        cache.clear();
        xacc::warning("Failed to parse cache file " + in_filePath);
        return false;
    }

    xacc::info("Successfully load " + std::to_string(cache.size()) + " entries from cache file.");
    return true;
}
        
bool QFAST::DecomposedResultCache::addCacheEntry(const Eigen::MatrixXcd& in_unitary, const std::vector<Block>& in_decomposedBlocks, bool in_shouldSync)
{
    // Must add new entries only
    assert(getCache(in_unitary).empty());
    CacheEntry newEntry;
    newEntry.uMat = in_unitary;
    newEntry.blocks = in_decomposedBlocks;
    cache.emplace_back(std::move(newEntry));
    // Write to file
    if (in_shouldSync && fileExists(fileName))
    {
        const std::string newCacheContent = toJson();
        // Open file to write
        FILE* hFile = fopen(fileName.c_str(), "w");
        // Failed to open file for writing.
        if (hFile == NULL) 
        {
           xacc::warning("Failed to open file to write QFAST cache.");
           return false;
        }
        fputs(newCacheContent.c_str(), hFile);
        fclose(hFile);
    }
    
    return true;
}
        
std::vector<QFAST::Block> QFAST::DecomposedResultCache::getCache(const Eigen::MatrixXcd& in_unitary) const
{
    // Note: for now, we do a linear search here since this is a local cache,
    // hence we don't expect it to have more than a handful of cached results.
    // TODO: if we are to implement a global database, we may need to implement
    // some form of hashing machanism.    
    // Tolerance for matching    
    constexpr double TRACE_DISTANCE_TOLERANCE = 1e-5;
    for (const auto& entry : cache)
    {
        // Dimension matched
        if (entry.uMat.size() == in_unitary.size())
        {
            if (QFAST::computeTraceDistance(entry.uMat, in_unitary) < TRACE_DISTANCE_TOLERANCE)  
            {
                xacc::info("Found cache entry!");
                return entry.blocks;
            }          
        }
    }
    return {};
}

std::string QFAST::Block::toJson() const 
{
    std::vector<double> realElems;
    std::vector<double> imagElems;
    realElems.reserve(16);
    imagElems.reserve(16);

    for (int i = 0; i < uMat.rows(); ++i)
    {
        for (int j = 0; j < uMat.cols(); ++j)
        {
            realElems.emplace_back(uMat(i, j).real());
            imagElems.emplace_back(uMat(i, j).imag()); 
        }
    }
    nlohmann::json j;
    j["u-real"] = realElems;
    j["u-imag"] = imagElems;
    j["bit1"] = qubits.first;
    j["bit2"] = qubits.second;

    return j.dump();
}

bool QFAST::Block::fromJsonString(const std::string& in_jsonString)
{
    auto j = nlohmann::json::parse(in_jsonString);
    const auto realElems = j["u-real"].get<std::vector<double>>();
    const auto imagElems = j["u-imag"].get<std::vector<double>>();
    const auto bit1 = j["bit1"].get<size_t>();
    const auto bit2 = j["bit2"].get<size_t>();   

    if (realElems.size() == 16 && imagElems.size() == 16 && bit1 != bit2)
    {
        uMat = BlockMatrix::Zero();
        for (int i = 0; i < uMat.rows(); ++i)
        {
            for (int j = 0; j < uMat.cols(); ++j)
            {
                uMat(i, j) = std::complex<double> { realElems[i * uMat.rows() + j], imagElems[i * uMat.rows() + j] };
            }
        }
        
        qubits = std::make_pair(bit1, bit2);
        return true;
    }     
    return false;
}

std::string QFAST::DecomposedResultCache::CacheEntry::toJson() const 
{
    std::vector<double> realElems;
    std::vector<double> imagElems;
    realElems.reserve(uMat.size());
    imagElems.reserve(uMat.size());

    for (int i = 0; i < uMat.rows(); ++i)
    {
        for (int j = 0; j < uMat.cols(); ++j)
        {
            realElems.emplace_back(uMat(i, j).real());
            imagElems.emplace_back(uMat(i, j).imag()); 
        }
    }

    std::vector<std::string> blockJson;
    for (const auto& block: blocks)
    {
        blockJson.emplace_back(block.toJson());
    }

    nlohmann::json j;
    j["u-real"] = realElems;
    j["u-imag"] = imagElems;
    j["blocks"] = blockJson;

    return j.dump();
}

bool QFAST::DecomposedResultCache::CacheEntry::fromJsonString(const std::string& in_jsonString)
{
    auto j = nlohmann::json::parse(in_jsonString);
    const auto realElems = j["u-real"].get<std::vector<double>>();
    const auto imagElems = j["u-imag"].get<std::vector<double>>();
    const auto blockJsons = j["blocks"].get<std::vector<std::string>>();

    if (realElems.size() == imagElems.size())
    {
        const auto nbQubits = [](size_t unitaryLength) {
            unsigned int ret = 0;
            while (unitaryLength > 1) 
            {
                unitaryLength >>= 1;
                ret++;
            }
        
            return ret / 2;
        }(realElems.size());
        
        uMat = Eigen::MatrixXcd::Zero(1ULL << nbQubits, 1ULL << nbQubits);
        for (int i = 0; i < uMat.rows(); ++i)
        {
            for (int j = 0; j < uMat.cols(); ++j)
            {
                uMat(i, j) = std::complex<double> { realElems[i * uMat.rows() + j], imagElems[i * uMat.rows() + j] };
            }
        }

        for (const auto& blockJson: blockJsons)
        {
            Block newBlock;
            if (newBlock.fromJsonString(blockJson))
            {
                blocks.emplace_back(newBlock);
            }
            else
            {
                return false;
            }
        }
        
        return true;
    }     

    return false;    
}

std::string QFAST::DecomposedResultCache::toJson() const 
{
    std::vector<std::string> cacheJson;
    for (const auto& entry: cache)
    {
        cacheJson.emplace_back(entry.toJson());
    }

    nlohmann::json j;
    j["cache"] = cacheJson;

    return j.dump();
}

bool QFAST::DecomposedResultCache::fromJsonString(const std::string& in_jsonString)
{
    auto j = nlohmann::json::parse(in_jsonString);
    const auto cacheJsons = j["cache"].get<std::vector<std::string>>();
    for (const auto& entryJson: cacheJsons)
    {
        CacheEntry newEntry;
        if (newEntry.fromJsonString(entryJson))
        {
            cache.emplace_back(newEntry);
        }
        else
        {
            return false;
        }
    }
    
    return true;
}
}
}