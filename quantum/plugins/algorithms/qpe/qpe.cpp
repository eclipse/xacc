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
#include "qpe.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "Circuit.hpp"
#include <cassert>
#include <iomanip>

namespace xacc {
namespace algorithm {
bool QuantumPhaseEstimation::initialize(const HeterogeneousMap& parameters) 
{
    bool initializeOk = true;
    if (!parameters.pointerLikeExists<Accelerator>("accelerator")) 
    {
        std::cout << "'accelerator' is required.\n";
        // We check all required params; hence don't early return on failure.
        initializeOk = false;
    }

    if (!parameters.pointerLikeExists<CompositeInstruction>("oracle")) 
    {
        std::cout << "'oracle' is required.\n";
        initializeOk = false;
    }

    if (initializeOk)
    {
        m_qpu = parameters.getPointerLike<Accelerator>("accelerator");
        m_oracle = parameters.getPointerLike<CompositeInstruction>("oracle");
    }

    m_params = parameters;
    return initializeOk;
}

const std::vector<std::string> QuantumPhaseEstimation::requiredParameters() const 
{
    return { "accelerator", "oracle" };
}

void QuantumPhaseEstimation::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const 
{
    // Calculate the number of qubits that are used by the oracle:
    const size_t nbOracleBits = m_oracle->uniqueBits().size();

    if (nbOracleBits < 1)
    {
        std::cout << "Invalid oracle Composite.\n";
    }
    
    // Buffer must contain additional qubits for phase est. result
    if (buffer->size() <= nbOracleBits)
    {
        std::cout << "Buffer must have more than " << nbOracleBits << " qubits.\n";
    }
    
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    auto qpeKernel = gateRegistry->createComposite("QuantumPhaseEstKernel");

    // Bit precision: the number of extra qubits that were allocated in the input buffer.
    const auto bitPrecision = buffer->size() - nbOracleBits;
    std::cout << "Phase estimation precision = " << bitPrecision << " bits.\n";
    
    // Map the oracle/state-prep circuit to the qubit range after the phase estimation register:
    // q[0] -> q[bitPrecision]
    // q[1] -> q[bitPrecision + 1], etc...
    const auto mapPrimaryQubits = [&bitPrecision](const std::shared_ptr<xacc::CompositeInstruction>& in_composite){
        InstructionIterator it(in_composite);
        while (it.hasNext())
        {
            auto nextInst = it.next();
            if (nextInst->isEnabled())
            {
                auto currentBits = nextInst->bits();
                for (auto& bit: currentBits)
                {
                    bit = bit + bitPrecision;
                }
                nextInst->setBits(currentBits);
            }
        }
    };
    
    // Convention: q[0] - q[precision - 1] is phase-estimation result register.
    // Hadamard on all ancilla/result qubits
    for (size_t i = 0; i < bitPrecision; ++i)
    {
        qpeKernel->addInstruction(gateRegistry->createInstruction("H", { i }));
    }

    // Prepare the eigenstate:
    // Note: user must provide 'state-preparation' composite 
    // to transform |0> state to the eigenstate.
    // Otherwise, assume |0> is the eigenstate.
    if (m_params.pointerLikeExists<CompositeInstruction>("state-preparation")) 
    {
        auto statePrep = m_params.getPointerLike<CompositeInstruction>("state-preparation");        
        mapPrimaryQubits(std::shared_ptr<CompositeInstruction>(statePrep, xacc::empty_delete<CompositeInstruction>()));

        if (statePrep->uniqueBits().size() > nbOracleBits)
        {
            xacc::error("'state-preparation' circuit cannot operate on more qubits than the oracle.");
            return;
        }

        // Add state preparation composite
        qpeKernel->addInstructions(statePrep->getInstructions());
    }

    auto oracleSharedPtr = std::shared_ptr<CompositeInstruction>(m_oracle, xacc::empty_delete<CompositeInstruction>());
    mapPrimaryQubits(oracleSharedPtr);
    
    // Controlled-oracle application
    for (size_t i = 0; i < bitPrecision; ++i)
    {
        // q0: U; q1: U^2; q2: U^4; etc.
        const int nbCalls = 1 << i;
        auto ctrlKernel = std::dynamic_pointer_cast<CompositeInstruction>(xacc::getService<Instruction>("C-U"));
        ctrlKernel->expand( { 
            std::make_pair("U",  m_oracle),
            std::make_pair("control-idx",  static_cast<int>(i)),
        });            
        // Apply C-U^n
        for (int count = 0; count < nbCalls; count++)
        {
            for (int instId = 0; instId < ctrlKernel->nInstructions(); ++instId)
            {
                // We need to clone the instruction since it'll be repeated.
                qpeKernel->addInstruction(ctrlKernel->getInstruction(instId)->clone());
            }
        }
    }
    
    // IQFT on the phase estimation register.
    auto iqft = std::dynamic_pointer_cast<CompositeInstruction>(xacc::getService<Instruction>("iqft"));
    iqft->expand( { std::make_pair("nq", static_cast<int>(bitPrecision)) });    
    qpeKernel->addInstructions(iqft->getInstructions());
    
    // Measure the ancilla/result qubits
    for (size_t i = 0; i < bitPrecision; ++i)
    {
        qpeKernel->addInstruction(gateRegistry->createInstruction("Measure", { i }));
    }
    
    buffer->addExtraInfo("qpe-kernel", qpeKernel->toString());
    m_qpu->execute(buffer, qpeKernel);
}

std::vector<double> QuantumPhaseEstimation::execute(const std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<double>& x) 
{
    // We don't have this for QPE.
    xacc::error("This method is unsupported!");
    return { };
}
} // namespace algorithm
} // namespace xacc