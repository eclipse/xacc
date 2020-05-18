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
    if (buffer->size() < 1)
    {
        std::cout << "Buffer must have more than 1 qubits.\n";
    }
    
    // Bit precision: the number of extra qubits that were allocated in the input buffer.
    const auto bitPrecision = buffer->size() - 1;



}

std::vector<double> QuantumPhaseEstimation::execute(const std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<double>& x) 
{
    // TODO
    return { };
}

} // namespace algorithm
} // namespace xacc