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
#include "qaoa_circuit.hpp"
#include "PauliOperator.hpp"

namespace {
  // Null if not an Observable-like type 
  std::shared_ptr<xacc::Observable> getObservableRuntimeArg(const xacc::HeterogeneousMap& in_runtimeArg) 
  {
    // Try base Observable pointer:
    if (in_runtimeArg.pointerLikeExists<xacc::Observable>(xacc::INTERNAL_ARGUMENT_VALUE_KEY))
    {
      return std::shared_ptr<xacc::Observable>(
        in_runtimeArg.getPointerLike<xacc::quantum::PauliOperator>(xacc::INTERNAL_ARGUMENT_VALUE_KEY),
        xacc::empty_delete<xacc::Observable>());
    }
    
    // Try concrete Pauli Observable
    // Reference type
    if (in_runtimeArg.keyExists<xacc::quantum::PauliOperator>(xacc::INTERNAL_ARGUMENT_VALUE_KEY))
    {
      // Just make a copy
      return std::make_shared<xacc::quantum::PauliOperator>(
        in_runtimeArg.get<xacc::quantum::PauliOperator>(xacc::INTERNAL_ARGUMENT_VALUE_KEY));
    }

    // Pointer type
    if (in_runtimeArg.pointerLikeExists<xacc::quantum::PauliOperator>(xacc::INTERNAL_ARGUMENT_VALUE_KEY))
    {
      // Make a copy
      return std::make_shared<xacc::quantum::PauliOperator>(
        *(in_runtimeArg.getPointerLike<xacc::quantum::PauliOperator>(xacc::INTERNAL_ARGUMENT_VALUE_KEY)));
    }

    return nullptr;
  }
}

namespace xacc {
namespace circuits {
bool QAOA::expand(const xacc::HeterogeneousMap& runtimeOptions)
{
  if (runtimeOptions.size() == 0)
  {
    return false;
  }
  
  if (!runtimeOptions.keyExists<int>("nbQubits")) 
  {
    std::cout << "'nbQubits' is required.\n";
    return false;
  }

  if (!runtimeOptions.keyExists<int>("nbSteps")) 
  {
    std::cout << "'nbSteps' is required.\n";
    return false;
  }

  if (!runtimeOptions.pointerLikeExists<xacc::Observable>("cost-ham")) 
  {
    std::cout << "'cost-ham' is required.\n";
    return false;
  }

  m_nbQubits = runtimeOptions.get<int>("nbQubits");
  m_nbSteps = runtimeOptions.get<int>("nbSteps");
  
  auto costHam = runtimeOptions.getPointerLike<xacc::Observable>("cost-ham");
  xacc::Observable* refHam = nullptr;
  if (runtimeOptions.pointerLikeExists<xacc::Observable>("ref-ham")) 
  {
    refHam = runtimeOptions.getPointerLike<xacc::Observable>("ref-ham");
  }

  parseObservables(costHam, refHam);
  // Expand to a parametric kernel
  auto kernel = constructParameterizedKernel();
  clear();
  variables = kernel->getVariables();
  for (int instId = 0; instId < kernel->nInstructions(); ++instId)
  {
    addInstruction(kernel->getInstruction(instId)->clone());
  }

  return true;
}

const std::vector<std::string> QAOA::requiredKeys() 
{
  // Keys for conventional circuit expand
  // Mixer Hamiltonian is optional. 
  return { "nbQubits", "nbSteps", "cost-ham" };
}

void QAOA::parseObservables(Observable* costHam, Observable* refHam)
{
  m_costHam.clear();
  m_refHam.clear();

  const auto pauliTermToString = [](const std::shared_ptr<xacc::Observable>& in_term){
    std::string pauliTermStr = in_term->toString();
    std::stringstream s;
    s.precision(12);
    s << std::fixed << in_term->coefficient();
    // Find the parenthesis
    const auto startPosition = pauliTermStr.find("(");
    const auto endPosition = pauliTermStr.find(")");

    if (startPosition != std::string::npos && endPosition != std::string::npos)
    {
        const auto length = endPosition - startPosition + 1;
        pauliTermStr.replace(startPosition, length, s.str());
    }
    return pauliTermStr;
  };

  for (const auto& term : costHam->getNonIdentitySubTerms())
  {
    m_costHam.emplace_back(pauliTermToString(term));
  }
  // Default X mixer
  if (!refHam)
  {
    for (size_t qId = 0; qId < m_nbQubits; ++qId)
    {
      m_refHam.emplace_back("X" + std::to_string(qId));
    }
  }
  else
  {
    // Ref-Ham was provided:
    for (const auto& term : refHam->getNonIdentitySubTerms())
    {
      m_refHam.emplace_back(pauliTermToString(term));
    }
  }
}

std::shared_ptr<CompositeInstruction> QAOA::constructParameterizedKernel() const
{   
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  auto qaoaKernel = gateRegistry->createComposite("qaoaKernel");

  // Hadamard layer
  for (size_t i = 0; i < m_nbQubits; ++i)
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
          std::cout << expCirc->toString() << "\n";
          qaoaKernel->addVariable(paramName);
          qaoaKernel->addInstructions(expCirc->getInstructions());
      }

      // Beta params:
      // If no drive/reference Hamiltonian is given,
      // then assume the default X0 + X1 + ...
      std::vector<std::string> refHamTerms(m_refHam);
      if (refHamTerms.empty())
      {
          for (size_t qId = 0; qId < m_nbQubits; ++qId)
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
  // std::cout << "Kernel: \n" << qaoaKernel->toString() << "\n";
  return qaoaKernel;
}

// Runtime arguments (e.g. QCOR)
void QAOA::applyRuntimeArguments() 
{
  // Apply runtime arguments: i.e. resolve all parameters to concrete values
  m_nbQubits = arguments[1]->runtimeValue.get<int>(INTERNAL_ARGUMENT_VALUE_KEY);
  const std::vector<double> betaVec = arguments[2]->runtimeValue.get<std::vector<double>>(INTERNAL_ARGUMENT_VALUE_KEY);
  const std::vector<double> gammaVec = arguments[3]->runtimeValue.get<std::vector<double>>(INTERNAL_ARGUMENT_VALUE_KEY);
  auto costHam = getObservableRuntimeArg(arguments[4]->runtimeValue);
  auto refHam = getObservableRuntimeArg(arguments[5]->runtimeValue);
  // Cost Hamiltonian is required.
  if (!costHam)
  {
    std::cout << "No cost Hamiltonian observable was provided!\n";
    return;
  }

  // Number of QAOA steps
  m_nbSteps = betaVec.size()/m_nbQubits;
  parseObservables(costHam.get(), refHam.get());  
  const int nbGammasPerStep = m_costHam.size();
  const int nbBetasPerStep = m_refHam.size();
  // Parametric kernel
  auto kernel = constructParameterizedKernel();
  std::vector<double> params;
  int gammaCounter = 0;
  int betaCounter = 0;

  // Combine gammas and betas into one vector to resolve/evaluate the circuit. 
  for (int i = 0; i < m_nbSteps; ++i)
  {
    // Gamma first
    for (int j = 0; j < nbGammasPerStep; ++j)
    {
      params.emplace_back(gammaVec[gammaCounter]);
      gammaCounter++;
    }
    // then betas
    for (int j = 0; j < nbBetasPerStep; ++j)
    {
      params.emplace_back(betaVec[betaCounter]);
      betaCounter++;
    }
  }
  // Evaluate the circuit:
  auto evaled = kernel->operator()(params);
  // std::cout << "Eval: \n" << evaled->toString() << "\n";
  
  // Construct circuit (now all gates have been resolved).
  clear();
  for (int instId = 0; instId < evaled->nInstructions(); ++instId)
  {
    addInstruction(evaled->getInstruction(instId)->clone());
  }
}
} // namespace circuits
} // namespace xacc
