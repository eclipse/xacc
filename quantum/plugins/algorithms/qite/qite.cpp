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
#include "qite.hpp"

#include "Observable.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "PauliOperator.hpp"
#include "Circuit.hpp"
#include <memory>
#include <Eigen/Dense>

using namespace xacc;

namespace xacc {
namespace algorithm {
bool QITE::initialize(const HeterogeneousMap &parameters) 
{
  // TODO
  return true;
}

const std::vector<std::string> QITE::requiredParameters() const 
{
  // TODO
  return {};
}

std::shared_ptr<CompositeInstruction> QITE::constructPropagateCircuit() const
{
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  auto propagateKernel = gateRegistry->createComposite("statePropCircuit");

  // Adds ansatz if provided
  if (m_ansatz)
  {
    propagateKernel->addInstructions(m_ansatz->getInstructions());
  } 

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

  // Progagates by Trotter steps
  // Using those A operators that have been 
  // optimized up to this point.
  for (const auto& aObs : m_approxOps)
  {
    // Circuit is: exp(-idt*A),
    // i.e. regular evolution which approximates the imaginary time evolution.
    for (const auto& term : aObs->getNonIdentitySubTerms())
    {
      const auto pauliStr = pauliTermToString(term);
      auto expCirc = std::dynamic_pointer_cast<xacc::quantum::Circuit>(xacc::getService<Instruction>("exp_i_theta"));
      expCirc->addVariable("theta");
      expCirc->expand({ std::make_pair("pauli", term) });
      auto evaled = expCirc->operator()({ m_dBeta });
      propagateKernel->addInstructions(evaled->getInstructions());
    }
  }

  return propagateKernel;
}


void QITE::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const 
{
  // Calculate approximate A operator observable at each time step.
  const auto calcAOps = [&](std::shared_ptr<CompositeInstruction>) -> std::shared_ptr<Observable> {
    // Step 1: Observe the kernels using the various Pauli
    // operators to calculate S and b.
    // TODO: this requires some Pauli algebra computation.
    // Step 2: Calculate S matrix and b vector
    // i.e. set up the linear equation Sa = b
    // These are expectations on the current state.
    // i.e. set-up the observables to calculate those entries.
    // TODO: construct circuits and evaluate to get expectation values
    // and populate S matrix and b vector.
    Eigen::MatrixXf S_Mat;
    Eigen::VectorXf b_Vec; 
    
    // const auto a_Vec = S_Mat.bdcSvd(ComputeThinU | ComputeThinV).solve(b_Vec);
    // TODO: construct A observable from the coefficients in a_Vec
    return nullptr; 
  };
  
  // Time stepping:
  for (int i = 0; i < m_nbSteps; ++i)
  {
    // Propagates the state via Trotter steps:
    auto kernel = constructPropagateCircuit();
    // Optimizes/calculates next A ops
    auto nextAOps = calcAOps(kernel);
    m_approxOps.emplace_back(nextAOps);
  }
}

std::vector<double> QITE::execute(const std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<double>& x) 
{
  // TODO
  return {};
}
} // namespace algorithm
} // namespace xacc