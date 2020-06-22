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
    // Requirements: measure at the very end and nothing after measure.
    bool shotCountFromFinalStateVec(const std::shared_ptr<CompositeInstruction>& in_composite)
    {
        InstructionIterator it(in_composite);
        bool measureAtTheEnd = true;
        bool measureEncountered = false;

        while (it.hasNext())
        {
            auto nextInst = it.next();
            if (nextInst->isEnabled())
            {
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

        // If Measure gates are at the very end,
        // this Composite can be simulated by random sampling from the state vec.
        return measureAtTheEnd;
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
            m_visitor->finalize();
        }
    }
    
    void QppAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<std::shared_ptr<CompositeInstruction>> compositeInstructions)
    {
        for (auto& f : compositeInstructions)
        {
            auto tmpBuffer = std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size());
            execute(tmpBuffer, f);
            buffer->appendChild(f->name(), tmpBuffer);
        }
    }
}}