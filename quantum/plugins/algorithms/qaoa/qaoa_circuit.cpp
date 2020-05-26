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

namespace xacc {
namespace circuits {
bool QAOA::expand(const xacc::HeterogeneousMap& runtimeOptions)
{
  return false;
}

const std::vector<std::string> QAOA::requiredKeys() 
{
  return {};
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


void QAOA::applyRuntimeArguments() 
{
  // Apply runtime arguments: i.e. resolve all parameters to concrete values
  m_nbQubits = arguments[0]->runtimeValue.get<int>(INTERNAL_ARGUMENT_VALUE_KEY);
  const std::vector<double> betaVec = arguments[1]->runtimeValue.get<std::vector<double>>(INTERNAL_ARGUMENT_VALUE_KEY);
  const std::vector<double> gammaVec = arguments[2]->runtimeValue.get<std::vector<double>>(INTERNAL_ARGUMENT_VALUE_KEY);
  auto costHam = arguments[3]->runtimeValue.getPointerLike<xacc::Observable>(INTERNAL_ARGUMENT_VALUE_KEY);
  auto refHam = arguments[4]->runtimeValue.getPointerLike<xacc::Observable>(INTERNAL_ARGUMENT_VALUE_KEY);
  // std::cout << "Number of qubits: " << m_nbQubits << "\n";
  // std::cout << "Beta length: " << betaVec.size() << "\n";
  // std::cout << "Gamma length: " << gammaVec.size() << "\n";
  // std::cout << "Cost Ham: \n " << costHam->toString() << "\n";
  // std::cout << "Ref Ham: \n " << refHam->toString() << "\n";

  // Number of QAOA steps
  m_nbSteps = betaVec.size()/m_nbQubits;
  // std::cout << "Number steps (p): " << m_nbSteps << "\n";

  m_costHam.clear();
  for (const auto& term : costHam->getNonIdentitySubTerms())
  {
      std::string pauliTermStr = term->toString();
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

  m_refHam.clear();
  for (const auto& term : refHam->getNonIdentitySubTerms())
  {
      std::string pauliTermStr = term->toString();
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
      m_refHam.emplace_back(pauliTermStr);
  }

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
