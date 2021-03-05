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
#include "QppAccelerator.hpp"
#include <thread>
#include <mutex>
#include "IRUtils.hpp"

namespace {
    inline bool isMeasureGate(const xacc::InstPtr& in_instr)
    {
        return (in_instr->name() == "Measure");
    }

    inline size_t getNumberOfThreads()
    {
        static const size_t NB_THREADS = std::thread::hardware_concurrency();
        return NB_THREADS;
    }

    inline double generateRandomProbability() 
    {
        auto randFunc = std::bind(std::uniform_real_distribution<double>(0, 1), std::mt19937(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
        return randFunc();
    }

    bool applyMeasureOp(KetVectorType& io_psi, size_t in_qubitIndex)
    {
        const auto N = io_psi.size();
        const auto k_range = 1ULL << in_qubitIndex;        
        const double randProbPick = generateRandomProbability();
        double cumulativeProb = 0.0;
        size_t stateSelect = 0;
        //select a state based on cumulative distribution
        while (cumulativeProb < randProbPick && stateSelect < N)
        {
            cumulativeProb += std::norm(io_psi[stateSelect++]);
        }
        stateSelect--;

        //take the value of the measured bit
        bool result = ((stateSelect >> in_qubitIndex) & 1);
        // Collapse the state vector according to the measurement result
        double measProb = 0.0;
        for (size_t g = 0; g < N; g += (k_range * 2))
        {
            for (size_t i = 0; i < k_range; ++i)
            {
                if ((((i + g) >> in_qubitIndex) & 1) == result)
                {
                    measProb += std::norm(io_psi[i + g]);
                    io_psi[i + g + k_range] = 0.0; 
                }
                else
                {
                    measProb += std::norm(io_psi[i + g + k_range]);
                    io_psi[i + g] = 0.0;
                }
            }
        }
        // Renormalize the state
        measProb = std::sqrt(measProb);
        for (size_t g = 0; g < N; g += (k_range * 2))
        {
            for (size_t i = 0; i < k_range; i += 1)
            {
                if ((((i + g) >> in_qubitIndex) & 1) == result)
                {
                    io_psi[i + g] /= measProb;
                }
                else
                {
                    io_psi[i + g + k_range] /= measProb;
                }    
            }
        }

        return result;
    }

    void generateMeasureBitString(std::shared_ptr<xacc::AcceleratorBuffer> in_buffer, const std::vector<size_t>& in_bits, const KetVectorType& in_stateVec, int in_shotCount, bool in_multiThread = false)
    {
        if (!in_multiThread)
        {
            // Sequential execution
            for (int i = 0; i < in_shotCount; ++i)
            {
                std::string bitString;
                auto stateVecCopy = in_stateVec;
                for (const auto& bit : in_bits)    
                {
                    bitString.append(std::to_string(applyMeasureOp(stateVecCopy, bit)));
                }

                in_buffer->appendMeasurement(bitString);
            }
        }
        else
        {
            // Parallel execution: divide the shot counts to threads.
            std::vector<std::string> bitStringArray(in_shotCount);
            std::vector<std::thread> threads(getNumberOfThreads());
            std::mutex critical;
            for(int t = 0; t < getNumberOfThreads(); ++t)
            {
                threads[t] = std::thread(std::bind([&](int beginIdx, int endIdx, int threadIdx) {
                    for(int i = beginIdx; i < endIdx; ++i)
                    {
                        std::string bitString;
                        auto stateVecCopy = in_stateVec;
                        for (const auto& bit : in_bits)    
                        {
                            bitString.append(std::to_string(applyMeasureOp(stateVecCopy, bit)));
                        }
                        bitStringArray[i] = bitString;
                    }
                    {
                        // Add measurement bitstring to the buffer:
                        std::lock_guard<std::mutex> lock(critical);
                        for(int i = beginIdx; i < endIdx; ++i)
                        {
                            in_buffer->appendMeasurement(bitStringArray[i]);
                        }
                    }
                }, 
                t * in_shotCount / getNumberOfThreads(), 
                (t+1) == getNumberOfThreads() ? in_shotCount: (t+1) * in_shotCount/getNumberOfThreads(), 
                t));
            }
            std::for_each(threads.begin(),threads.end(),[](std::thread& x){
                x.join();
            });
        }
    }

    // Helper to determine if shot count distribution can be simulated by 
    // sampling the final state vector.
    // Requirements: no reset and measure at the very end and nothing after measure.
    bool shotCountFromFinalStateVec(const std::shared_ptr<CompositeInstruction>& in_composite)
    {
        InstructionIterator it(in_composite);
        bool measureAtTheEnd = true;
        bool measureEncountered = false;
        bool hasReset = false;
        while (it.hasNext())
        {
            auto nextInst = it.next();
            if (nextInst->isEnabled())
            {
                if (nextInst->name() == "Reset") {
                    hasReset = true;
                }

                if (isMeasureGate(nextInst))
                {
                    // Flag that we have seen a Measure gate.
                    measureEncountered = true;
                }

                // We have seen a Measure gate but this one is not another Measure gate.
                if (measureEncountered && !isMeasureGate(nextInst))
                {
                    measureAtTheEnd = false;
                }
            }
        }

        // If Measure gates are at the very end and no reset,
        // this Composite can be simulated by random sampling from the state vec.
        return !hasReset && measureAtTheEnd;
    }

    Eigen::MatrixXcd convertToEigenMat(const NoiseModelUtils::cMat& in_stdMat)
    {
        Eigen::MatrixXcd result =  Eigen::MatrixXcd::Zero(in_stdMat.size(), in_stdMat.size());
        for (size_t row = 0; row < in_stdMat.size(); ++row)
        {
            for (size_t col = 0; col < in_stdMat.size(); ++col)
            {
                result(row, col) = in_stdMat[row][col];
            }
        }
        return result;
    }

    NoiseModelUtils::cMat convertToStdMat(const Eigen::MatrixXcd& in_eigenMat)
    {
        const size_t dim = in_eigenMat.rows();
        NoiseModelUtils::cMat result;
        for (size_t row = 0; row < dim; ++row)
        {
            std::vector<std::complex<double>> rowVec;
            for (size_t col = 0; col < dim; ++col)
            {
                rowVec.emplace_back(in_eigenMat(row, col)); 
            }
            result.emplace_back(rowVec);
        }
        return result;
    }
}

namespace xacc {
namespace quantum {

    void QppAccelerator::initialize(const HeterogeneousMap& params)
    {
        m_visitor = std::make_shared<QppVisitor>();
        // Default: no shots (unless otherwise specified)
        m_shots = -1;
        if (params.keyExists<int>("shots"))
        {
            m_shots = params.get<int>("shots");
            if (m_shots < 1)
            {
                xacc::error("Invalid 'shots' parameter.");
            }
        }
        // Enable VQE mode by default if not using shots.
        // Note: in VQE mode, only expectation values are computed.
        m_vqeMode = (m_shots < 1);
        if (params.keyExists<bool>("vqe-mode"))
        {
            m_vqeMode = params.get<bool>("vqe-mode");
            if (m_vqeMode)
            {
                xacc::info("Enable VQE Mode.");
            }
        }

        if (params.keyExists<std::vector<std::pair<int,int>>>("connectivity")) {
            m_connectivity = params.get<std::vector<std::pair<int,int>>>("connectivity");
        }
    }

    void QppAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::shared_ptr<CompositeInstruction> compositeInstruction)
    {
        const auto runCircuit = [&](bool shotsMode){
            m_visitor->initialize(buffer, shotsMode);

            // Walk the IR tree, and visit each node
            InstructionIterator it(compositeInstruction);
            while (it.hasNext())
            {
                auto nextInst = it.next();
                if (nextInst->isEnabled())
                {
                    nextInst->accept(m_visitor);
                }
            }

            m_visitor->finalize();
        };

        // Not possible to simulate shot count by direct sampling,
        // e.g. must collapse the state vector.
        if(!shotCountFromFinalStateVec(compositeInstruction))
        {
            if (m_shots < 0)
            {
                runCircuit(false);
            }
            else
            {
                for (int i = 0; i < m_shots; ++i)
                {
                    runCircuit(true);
                }
            }
        }
        else
        {
            // Index of measure bits
            std::vector<size_t> measureBitIdxs;
            m_visitor->initialize(buffer);
            // Walk the IR tree, and visit each node
            InstructionIterator it(compositeInstruction);
            while (it.hasNext())
            {
                auto nextInst = it.next();
                if (nextInst->isEnabled())
                {
                    if (!isMeasureGate(nextInst))
                    {
                        nextInst->accept(m_visitor);
                    }
                    else
                    {
                        // Just collect the indices of measured qubit
                        measureBitIdxs.emplace_back(nextInst->bits()[0]);
                    }
                }
            }
            
            // Run bit-string simulation
            if (!measureBitIdxs.empty())
            {
                const auto& stateVec = m_visitor->getStateVec();
                if (m_shots < 0)
                {
                    const double expectedValueZ = QppVisitor::calcExpectationValueZ(stateVec, measureBitIdxs);
                    buffer->addExtraInfo("exp-val-z", expectedValueZ);
                }
                else
                {
                    // Try multi-threaded execution if there are many shots.
                    const bool multiThreadEnabled = (m_shots > 1024);
                    generateMeasureBitString(buffer, measureBitIdxs, stateVec, m_shots, multiThreadEnabled);
                }
            }
            // Note: must save the state-vector before finalizing the visitor.
            cacheExecutionInfo();
            m_visitor->finalize();
        }
    }
    
    void QppAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<std::shared_ptr<CompositeInstruction>> compositeInstructions)
    {
        if (!m_vqeMode || compositeInstructions.size() <= 1) 
        {
            for (auto& f : compositeInstructions)
            {
                auto tmpBuffer = std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size());
                execute(tmpBuffer, f);
                buffer->appendChild(f->name(), tmpBuffer);
            }
        }
        else 
        {
            auto kernelDecomposed = ObservedAnsatz::fromObservedComposites(compositeInstructions);
            // Always validate kernel decomposition in DEBUG
            assert(kernelDecomposed.validate(compositeInstructions));
            m_visitor->initialize(buffer);            
            // Base kernel:
            auto baseKernel = kernelDecomposed.getBase();
            // Basis-change + measures
            auto obsCircuits = kernelDecomposed.getObservedSubCircuits();
            // Walk the base IR tree, and visit each node
            InstructionIterator it(baseKernel);
            while (it.hasNext()) 
            {
                auto nextInst = it.next();
                if (nextInst->isEnabled() && !nextInst->isComposite()) 
                {
                    nextInst->accept(m_visitor);
                }
            }

            // Now we have a wavefunction that represents execution of the ansatz.
            // Run the observable sub-circuits (change of basis + measurements)
            for (int i = 0; i < obsCircuits.size(); ++i) 
            {
                auto tmpBuffer = std::make_shared<xacc::AcceleratorBuffer>(obsCircuits[i]->name(), buffer->size());
                const double e = m_visitor->getExpectationValueZ(obsCircuits[i]);
                tmpBuffer->addExtraInfo("exp-val-z", e);
                buffer->appendChild(obsCircuits[i]->name(), tmpBuffer);
            }
            m_visitor->finalize();
        }
    }

    void QppAccelerator::apply(std::shared_ptr<AcceleratorBuffer> buffer, std::shared_ptr<Instruction> inst) 
    {
        if (!m_visitor->isInitialized()) {
            m_visitor->initialize(buffer);
            m_currentBuffer = std::make_pair(buffer.get(), buffer->size());
        }

        if (buffer.get() == m_currentBuffer.first &&
            buffer->size() != m_currentBuffer.second) {
            if (buffer->size() > m_currentBuffer.second) {
                const auto nbQubitsToAlloc = buffer->size() - m_currentBuffer.second;
                // std::cout << "Allocate " << nbQubitsToAlloc << " qubits.\n";
                m_visitor->allocateQubits(nbQubitsToAlloc);
                m_currentBuffer.second = buffer->size();
            }
            else {
                xacc::error("Qubits deallocation is not supported.");
            }
        }

        if (inst->isComposite() || inst->isAnalog())
        {
            xacc::error("Only gates are allowed.");
        }
        if (inst->name() == "Measure")
        {
            const auto measRes = m_visitor->measure(inst->bits()[0]);
            buffer->measure(inst->bits()[0], (measRes ? 1 : 0));
        }
        else
        {
            auto gateCast = std::dynamic_pointer_cast<xacc::quantum::Gate>(inst);
            assert(gateCast);
            m_visitor->applyGate(*gateCast);
        }
    }

    void QppAccelerator::cacheExecutionInfo() {
      // Cache the state-vector:
      // Note: qpp stores wavefunction in Eigen vectors,
      // hence, maps to std::vector.
      auto stateVec = m_visitor->getStateVec();
      ExecutionInfo::WaveFuncType waveFunc(stateVec.data(),
                                           stateVec.data() + stateVec.size());
      m_executionInfo = {
          {ExecutionInfo::WaveFuncKey,
           std::make_shared<ExecutionInfo::WaveFuncType>(std::move(waveFunc))}};
    }

    NoiseModelUtils::cMat DefaultNoiseModelUtils::krausToChoi(const std::vector<NoiseModelUtils::cMat>& in_krausMats) const
    {
        std::vector<Eigen::MatrixXcd> krausMats;
        for (const auto& mat : in_krausMats)
        {
            krausMats.emplace_back(convertToEigenMat(mat));
        }
        return convertToStdMat(qpp::kraus2choi(krausMats));
    }

    std::vector<NoiseModelUtils::cMat> DefaultNoiseModelUtils::choiToKraus(const NoiseModelUtils::cMat& in_choiMat) const
    {
        std::vector<NoiseModelUtils::cMat> resultKraus;
        const auto krausMats = qpp::choi2kraus(convertToEigenMat(in_choiMat));
        for (const auto& mat : krausMats)
        {
            resultKraus.emplace_back(convertToStdMat(mat));
        }
        return resultKraus;
    }

    NoiseModelUtils::cMat DefaultNoiseModelUtils::combineChannelOps(const std::vector<NoiseModelUtils::cMat> &in_choiMats) const 
    {
        assert(!in_choiMats.empty());
        auto choiSum = convertToEigenMat(in_choiMats[0]);
        for (size_t i = 1; i < in_choiMats.size(); ++i)
        {
            const auto nextOp = convertToEigenMat(in_choiMats[i]);
            choiSum = choiSum + nextOp;
        }
        const double normalized = std::abs((choiSum(0,0) + choiSum(1,1) + choiSum(2,2) + choiSum(3,3)).real()/2.0);
        choiSum = (1/normalized)*choiSum;
        return convertToStdMat(choiSum);
    }
}}