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
#include "ControlledGateApplicator.hpp" 

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
    if (buffer->size() < 1)
    {
        std::cout << "Buffer must have more than 1 qubits.\n";
    }
    
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    auto qpeKernel = gateRegistry->createComposite("QuantumPhaseEstKernel");

    // Bit precision: the number of extra qubits that were allocated in the input buffer.
    const auto bitPrecision = buffer->size() - 1;
    std::cout << "Phase estimation precision = " << bitPrecision << " bits.\n";
    // Convention: q[0] is the main qubit
    // q[1]..q[n] are the phase result qubits
    // Hadamard on all ancilla/result qubits
    for (size_t i = 1; i < buffer->size(); ++i)
    {
        qpeKernel->addInstruction(gateRegistry->createInstruction("H", { i }));
    }

    // Prepare q[0] in the eigenstate:
    // Note: user must provide 'state-preparation' composite 
    // to transform |0> state to the eigenstate.
    // Otherwise, assume |0> is the eigenstate.
    if (!m_params.pointerLikeExists<CompositeInstruction>("state-preparation")) 
    {
        auto statePrep = m_params.getPointerLike<CompositeInstruction>("state-preparation");
        if (statePrep->uniqueBits().size() != 1)
        {
            xacc::error("'state-preparation' circuit should only contain one qubit.");
            return;
        }
        // Add state preparation composite
        qpeKernel->addInstructions(statePrep->getInstructions());
    }

    // Controlled-oracle application
    ControlledGateApplicator gateApplicator;
    for (size_t i = 1; i < buffer->size(); ++i)
    {
        // q1: U; q2: U^2; q3: U^4; etc.
        const int nbCalls = 1 << (i - 1);
        
        auto ctrlKernel = gateApplicator.applyControl(std::shared_ptr<CompositeInstruction>(m_oracle, xacc::empty_delete<CompositeInstruction>()), i);
        // Apply C-U^n
        for (int count = 0; count < nbCalls; count++)
        {
            qpeKernel->addInstructions(ctrlKernel->getInstructions());
        }
    }
    
    // IQFT on q[1]-q[n]
    auto iqft = std::dynamic_pointer_cast<CompositeInstruction>(xacc::getService<Instruction>("iqft"));
    iqft->expand( { std::make_pair("nq", bitPrecision) });
    // We need to shift the qubit index up by 1 
    InstructionIterator it(iqft);
    while (it.hasNext())
    {
        auto nextInst = it.next();
        if (nextInst->isEnabled())
        {
            auto currentBits = nextInst->bits();
            for (auto& bit: currentBits)
            {
                bit = bit + 1;
            }
            nextInst->setBits(currentBits);
        }
    }
    qpeKernel->addInstructions(iqft->getInstructions());

    // Measure the ancilla/result qubits
    for (size_t i = 1; i < buffer->size(); ++i)
    {
        qpeKernel->addInstruction(gateRegistry->createInstruction("Measure", { i }));
    }
    
    // DEBUG: 
    std::cout << "QPE kernel:\n" << qpeKernel->toString() << "\n\n";

    // TODO: execute
}

std::vector<double> QuantumPhaseEstimation::execute(const std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<double>& x) 
{
    // TODO
    return { };
}
} // namespace algorithm
} // namespace xacc