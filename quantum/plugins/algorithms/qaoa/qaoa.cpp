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
#include "qaoa.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Circuit.hpp"
#include <cassert>

namespace xacc {
namespace algorithm {
bool QAOA::initialize(const HeterogeneousMap& parameters) 
{
    bool initializeOk = true;
    // Hyper-parameters for QAOA:
    // (1) Accelerator (QPU)
    if (!parameters.pointerLikeExists<Accelerator>("accelerator")) 
    {
        std::cout << "'accelerator' is required.\n";
        // We check all required params; hence don't early return on failure.
        initializeOk = false;
    }
    
    // (2) Classical optimizer
    if (!parameters.pointerLikeExists<Optimizer>("optimizer"))
    {
        std::cout << "'optimizer' is required.\n";
        initializeOk = false;
    }
    
    // (3) Number of mixing and cost function steps to use (default = 1)
    m_nbSteps = 1;
    if (parameters.keyExists<int>("steps"))
    {
        m_nbSteps =  parameters.get<int>("steps");
    }

    // (4) Initial values for the beta parameters
    m_betas.clear();
    if (parameters.keyExists<std::vector<double>>("init-betas"))
    {
        m_betas =  parameters.get<std::vector<double>>("init-betas");
    }

    // (5) Initial values for the gamma parameters
    m_gammas.clear();
    if (parameters.keyExists<std::vector<double>>("init-gammas"))
    {
        m_gammas =  parameters.get<std::vector<double>>("init-gammas");
    }
    
    // (6) Cost Hamiltonian
    if (!parameters.keyExists<std::vector<std::string>>("cost-ham")) 
    {
        std::cout << "'cost-ham' is required.\n";
        initializeOk = false;
    }

    // (7) Reference Hamiltonian 
    if (!parameters.keyExists<std::vector<std::string>>("ref-ham")) 
    {
        std::cout << "'ref-ham' is required.\n";
        initializeOk = false;
    }

    if (initializeOk)
    {
        m_refHam = parameters.get<std::vector<std::string>>("ref-ham");
        m_costHam = parameters.get<std::vector<std::string>>("cost-ham");
        m_qpu = parameters.getPointerLike<Accelerator>("accelerator");
        m_optimizer = parameters.getPointerLike<Optimizer>("optimizer");
    }

    return initializeOk;
}

const std::vector<std::string> QAOA::requiredParameters() const 
{
    return { "accelerator", "optimizer", "ref-ham", "cost-ham" };
}

std::shared_ptr<CompositeInstruction> QAOA::constructParameterizedKernel(const std::shared_ptr<AcceleratorBuffer>& in_buffer) const
{   
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    const auto nbQubits = in_buffer->size();
    auto qaoaKernel = gateRegistry->createComposite("qaoaKernel");

    // Hadamard layer
    for (size_t i = 0; i < nbQubits; ++i)
    {
        qaoaKernel->addInstruction(gateRegistry->createInstruction("H", { i }));
    }

    // Trotter layers (parameterized): mixing b/w cost and drive (reference) Hamiltonian
    auto expCirc = std::dynamic_pointer_cast<xacc::quantum::Circuit>(xacc::getService<Instruction>("exp_i_theta"));

    for (size_t i = 0; i < m_nbSteps; ++i)
    {
        for (const auto& term : m_costHam)
        {
            expCirc->expand({ std::make_pair("pauli", term) });
            std::cout << "Term: '" << term << "': \n" << expCirc->toString() << "\n";
        }

        for (const auto& term : m_refHam)
        {
            expCirc->expand({ std::make_pair("pauli", term) });
            std::cout << "Term: '" << term << "': \n" << expCirc->toString() << "\n";
        }

        // TODO: add the instructions from exponential circuits to the kernel:
        // need to re-map the variables to make sure we capture all parameters.
    }
   
    return qaoaKernel;
}

void QAOA::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const 
{
    const int nbQubits = buffer->size();
    auto kernel = constructParameterizedKernel(buffer);
    std::cout << "Kernel: \n" << kernel->toString() << "\n\n";
}

std::vector<double> QAOA::execute(const std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<double>& x) 
{
    // TODO
    return {};
}

} // namespace algorithm
} // namespace xacc