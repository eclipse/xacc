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
  // TEMP CODE:
  m_nbSteps = 1;
  m_accelerator = xacc::getAccelerator("qpp");

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
  // Helper to generate all permutation of Pauli observables:
  // e.g.
  // 1-qubit: I, X, Y, Z
  // 2-qubit: II, IX, IY, IZ, XI, XX, XY, XZ, YI, YX, YY, YZ, ZI, ZX, ZY, ZZ
  const auto generatePauliPermutation = [](int in_nbQubits) -> std::vector<std::string> {
    assert(in_nbQubits > 0);
    const int nbPermutations = std::pow(4, in_nbQubits);
    std::vector<std::string> opsList;
    opsList.reserve(nbPermutations);
    
    const std::vector<std::string> pauliOps { "X", "Y", "Z" };
    const auto addQubitPauli = [&opsList, &pauliOps](int in_qubitIdx){
      const auto currentOpListSize = opsList.size();
      for (int i = 0; i < currentOpListSize; ++i)
      {
        auto& currentOp = opsList[i];
        for (const auto& pauliOp : pauliOps)
        {
          const auto newOp = currentOp + pauliOp + std::to_string(in_qubitIdx);
          opsList.emplace_back(newOp);
        }
      }
    };
    
    opsList = { "", "X0", "Y0", "Z0" };
    for (int i = 1; i < in_nbQubits; ++i) 
    {
      addQubitPauli(i);
    }

    assert(opsList.size() == nbPermutations);
    std::sort(opsList.begin(), opsList.end());

    return opsList;
  };
  
  const auto pauliObsOps = generatePauliPermutation(buffer->size());
  const std::string pauliObsStr = [&pauliObsOps](){
    std::string result = "1.0 " + pauliObsOps[1];
    for (int i = 2; i < pauliObsOps.size(); ++i)
    {
      result.append(" + 1.0 " + pauliObsOps[i]);
    }

    return result;
  }();
  
  // Calculate approximate A operator observable at each time step.
  const auto calcAOps = [&](std::shared_ptr<CompositeInstruction> in_kernel) -> std::shared_ptr<Observable> {
    // Step 1: Observe the kernels using the various Pauli
    // operators to calculate S and b.
    std::vector<double> sigmaExpectation(pauliObsOps.size());
    sigmaExpectation[0] = 1.0;
    for (int i = 1; i < pauliObsOps.size(); ++i)
    {
      std::shared_ptr<Observable> tomoObservable = std::make_shared<xacc::quantum::PauliOperator>();
      const std::string pauliObsStr = "1.0 " + pauliObsOps[i];
      // std::cout << "Observable string: \n" << pauliObsStr << "\n";
      tomoObservable->fromString(pauliObsStr);
      std::cout << "Observable: \n" << tomoObservable->toString() << "\n";
      assert(tomoObservable->getSubTerms().size() == 1);
      assert(tomoObservable->getNonIdentitySubTerms().size() == 1);
      auto obsKernel = tomoObservable->observe(in_kernel).front();
      // std::cout << "HOWDY:\n" << obsKernel->toString() << "\n";
      auto tmpBuffer = xacc::qalloc(buffer->size());
      m_accelerator->execute(tmpBuffer, obsKernel);
      const auto expval = tmpBuffer->getExpectationValueZ();
      // std::cout << "Exp-Val = " << expval << "\n";
      sigmaExpectation[i] = expval;
    }

    // TODO: this requires some Pauli algebra computation.
    // Step 2: Calculate S matrix and b vector
    // i.e. set up the linear equation Sa = b
    // These are expectations on the current state.
    // i.e. set-up the observables to calculate those entries.
    // TODO: construct circuits and evaluate to get expectation values
    // and populate S matrix and b vector.
    const auto sMatDim = pauliObsOps.size();
    Eigen::MatrixXcf S_Mat = Eigen::MatrixXcf::Zero(sMatDim, sMatDim);
    Eigen::VectorXcf b_Vec = Eigen::VectorXcf::Zero(sMatDim); 
    
    const auto calcSmatEntry = [](const std::vector<double>& in_tomoExp, int in_row, int in_col) -> std::complex<double> {
      // Map the tomography expectation to the S matrix
      // S(i, j) = <psi|sigma_dagger(i)sigma(j)|psi>
      // sigma_dagger(i)sigma(j) will produce another Pauli operator with an additional coefficient.
      // e.g. sigma_x * sigma_y = i*sigma_z
      // TODO: formalize this, for testing, we do 1-qubit here.
      // For multiple qubits, use this one-qubit base map to generate the mapping.
      assert(in_tomoExp.size() == 4);
      int indexMap[4][4] = {
        {0, 1, 2, 3}, 
        {1, 0, 3, 2}, 
        {2, 3, 0, 1},
        {3, 2, 1, 0}
      }; 

      const std::complex<double> I{ 0.0, 1.0};
      std::complex<double> coefficientMap [4][4] = {
        {1, 1, 1, 1},
        {1, 1, I, -I},
        {1, -I, 1, I},
        {1, I, -I, 1}
      };

			return in_tomoExp[indexMap[in_row][in_col]]*coefficientMap[in_row][in_col];
    };

    // S matrix:
    // S(i, j) = <psi|sigma_dagger(i)sigma(j)|psi>
    for (int i = 0; i < sMatDim; ++i)
    {
      for (int j = 0; j < sMatDim; ++j)
      {
        S_Mat(i, j) = calcSmatEntry(sigmaExpectation, i, j);
      }
    }

    // b vector:
    // Project/flatten the target observable into the full list of all
    // possible Pauli operator combinations.
    // e.g. H = a X + b Z (1 qubit)
    // -> { 0.0, a, 0.0, b } (the ordering is I, X, Y, Z)
    std::vector<std::complex<double>> obsProjCoeffs (sMatDim);
    // TODO: assign this vector
    // !! This is complex!!!


    auto lhs = S_Mat + S_Mat.transpose();
    auto rhs = -b_Vec;
    const auto a_Vec = lhs.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(rhs);
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