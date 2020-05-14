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
#include "xacc_observable.hpp"
#include "Circuit.hpp"
#include <cassert>
#include <iomanip>

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
    
    // (4) Cost Hamiltonian
    if (!parameters.pointerLikeExists<Observable>("observable")) 
    {
        std::cout << "'observable' is required.\n";
        initializeOk = false;
    }

    // (5) Reference Hamiltonian: optional.
    // Default is X0 + X1 + ...
    // i.e. the X-basis where |+>|+>... is the ground state. 
    m_refHam.clear();
    if (parameters.keyExists<std::vector<std::string>>("ref-ham")) 
    {
        m_refHam = parameters.get<std::vector<std::string>>("ref-ham");
    }

    if (initializeOk)
    {
        m_costHamObs = parameters.getPointerLike<Observable>("observable");
        // Add cost Hamiltonian terms to the list of terms for gamma exp
        m_costHam.clear();
        for (const auto& term : m_costHamObs->getNonIdentitySubTerms())
        {
            std::string pauliTermStr = term->toString();
            // HACK: the Pauli parser doesn't like '-0', i.e. extra minus sign on integer (no decimal point)
            // hence, just reformat it.
            std::stringstream s;
            s.precision(12);
            s << std::fixed << term->coefficient();
            // Find the parenthesis
            const auto startPosition = pauliTermStr.find("(");
            const auto endPosition = pauliTermStr.find(")");

            if (startPosition != std::string::npos && endPosition != std::string::npos)
            {
                const auto length = endPosition - startPosition + 1;
                pauliTermStr.replace(startPosition, length, s.str());
            }
            
            m_costHam.emplace_back(pauliTermStr);
        }
        
        m_qpu = parameters.getPointerLike<Accelerator>("accelerator");
        m_optimizer = parameters.getPointerLike<Optimizer>("optimizer");
    }

    return initializeOk;
}

const std::vector<std::string> QAOA::requiredParameters() const 
{
    return { "accelerator", "optimizer", "observable" };
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
    int betaParamCounter = 0;
    int gammaParamCounter = 0;

    for (size_t i = 0; i < m_nbSteps; ++i)
    {
        for (const auto& term : m_costHam)
        {
            auto expCirc = std::dynamic_pointer_cast<xacc::quantum::Circuit>(xacc::getService<Instruction>("exp_i_theta"));
            const std::string paramName = "gamma" + std::to_string(gammaParamCounter++);
            expCirc->addVariable(paramName);
            expCirc->expand({ std::make_pair("pauli", term) });
            qaoaKernel->addVariable(paramName);
            qaoaKernel->addInstructions(expCirc->getInstructions());
        }

        // Beta params:
        // If no drive/reference Hamiltonian is given,
        // then assume the default X0 + X1 + ...
        std::vector<std::string> refHamTerms(m_refHam);
        if (refHamTerms.empty())
        {
            for (size_t qId = 0; qId < nbQubits; ++qId)
            {
                refHamTerms.emplace_back("X" + std::to_string(qId));
            }
        }

        for (const auto& term : refHamTerms)
        {
            auto expCirc = std::dynamic_pointer_cast<xacc::quantum::Circuit>(xacc::getService<Instruction>("exp_i_theta"));
            const std::string paramName = "beta" + std::to_string(betaParamCounter++);
            expCirc->addVariable(paramName);
            expCirc->expand({ std::make_pair("pauli", term) });
            qaoaKernel->addVariable(paramName);
            qaoaKernel->addInstructions(expCirc->getInstructions());
        }
    }
   
    return qaoaKernel;
}

void QAOA::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const 
{
    const int nbQubits = buffer->size();
    auto kernel = constructParameterizedKernel(buffer);
    // Observe the cost Hamiltonian:
    auto kernels = m_costHamObs->observe(kernel);

    int iterCount = 0;
    // Construct the optimizer/minimizer:
    OptFunction f(
        [&, this](const std::vector<double>& x, std::vector<double>& dx) {
            std::vector<double> coefficients;
            std::vector<std::string> kernelNames;
            std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

            double identityCoeff = 0.0;
            for (auto& f : kernels) 
            {
                kernelNames.push_back(f->name());
                std::complex<double> coeff = f->getCoefficient();

                int nFunctionInstructions = 0;
                if (f->getInstruction(0)->isComposite()) 
                {
                    nFunctionInstructions = kernel->nInstructions() + f->nInstructions() - 1;
                } 
                else 
                {
                    nFunctionInstructions = f->nInstructions();
                }

                if (nFunctionInstructions > kernel->nInstructions()) 
                {
                    auto evaled = f->operator()(x);
                    fsToExec.push_back(evaled);
                    coefficients.push_back(std::real(coeff));
                } 
                else 
                {
                    identityCoeff += std::real(coeff);
                }
            }

            auto tmpBuffer = xacc::qalloc(buffer->size());
            m_qpu->execute(tmpBuffer, fsToExec);
            auto buffers = tmpBuffer->getChildren();

            double energy = identityCoeff;
            auto idBuffer = xacc::qalloc(buffer->size());
            idBuffer->addExtraInfo("coefficient", identityCoeff);
            idBuffer->setName("I");
            idBuffer->addExtraInfo("kernel", "I");
            idBuffer->addExtraInfo("parameters", x);
            idBuffer->addExtraInfo("exp-val-z", 1.0);
            buffer->appendChild("I", idBuffer);

            for (int i = 0; i < buffers.size(); i++) 
            {
                auto expval = buffers[i]->getExpectationValueZ();
                energy += expval * coefficients[i];
                buffers[i]->addExtraInfo("coefficient", coefficients[i]);
                buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
                buffers[i]->addExtraInfo("exp-val-z", expval);
                buffers[i]->addExtraInfo("parameters", x);
                buffer->appendChild(fsToExec[i]->name(), buffers[i]);
            }
            
            std::stringstream ss;
            iterCount++;
            ss << "Iter " << iterCount << ": E(" << ( !x.empty() ? std::to_string(x[0]) : "");
            for (int i = 1; i < x.size(); i++)
            {
                ss << "," << std::setprecision(3) << x[i];
                if (i > 4)
                {
                    // Don't print too many params
                    ss << ", ...";
                    break;
                }
            }
            ss << ") = " << std::setprecision(12) << energy;
            std::cout << ss.str() << '\n';
            return energy;
        },
        kernel->nVariables());

    auto result = m_optimizer->optimize(f);
    buffer->addExtraInfo("opt-val", ExtraInfo(result.first));
    buffer->addExtraInfo("opt-params", ExtraInfo(result.second));
}

std::vector<double> QAOA::execute(const std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<double>& x) 
{
    const int nbQubits = buffer->size();
    auto kernel = constructParameterizedKernel(buffer);

    // Observe the cost Hamiltonian:
    auto kernels = m_costHamObs->observe(kernel);
    std::vector<double> coefficients;
    std::vector<std::string> kernelNames;
    std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

    double identityCoeff = 0.0;
    for (auto& f : kernels) 
    {
        kernelNames.push_back(f->name());
        std::complex<double> coeff = f->getCoefficient();

        int nFunctionInstructions = 0;
        if (f->getInstruction(0)->isComposite()) 
        {
            nFunctionInstructions = kernel->nInstructions() + f->nInstructions() - 1;
        } 
        else 
        {
            nFunctionInstructions = f->nInstructions();
        }

        if (nFunctionInstructions > kernel->nInstructions()) 
        {
            auto evaled = f->operator()(x);
            fsToExec.push_back(evaled);
            coefficients.push_back(std::real(coeff));
        } 
        else 
        {
            identityCoeff += std::real(coeff);
        }
    }

    auto tmpBuffer = xacc::qalloc(buffer->size());
    m_qpu->execute(tmpBuffer, fsToExec);
    auto buffers = tmpBuffer->getChildren();

    double energy = identityCoeff;
    auto idBuffer = xacc::qalloc(buffer->size());
    idBuffer->addExtraInfo("coefficient", identityCoeff);
    idBuffer->setName("I");
    idBuffer->addExtraInfo("kernel", "I");
    idBuffer->addExtraInfo("parameters", x);
    idBuffer->addExtraInfo("exp-val-z", 1.0);
    buffer->appendChild("I", idBuffer);

    for (int i = 0; i < buffers.size(); i++) 
    {
        auto expval = buffers[i]->getExpectationValueZ();
        energy += expval * coefficients[i];
        buffers[i]->addExtraInfo("coefficient", coefficients[i]);
        buffers[i]->addExtraInfo("kernel", fsToExec[i]->name());
        buffers[i]->addExtraInfo("exp-val-z", expval);
        buffers[i]->addExtraInfo("parameters", x);
        buffer->appendChild(fsToExec[i]->name(), buffers[i]);
    }
    
    return { energy };
}

} // namespace algorithm
} // namespace xacc