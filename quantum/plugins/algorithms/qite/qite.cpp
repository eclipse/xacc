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
#include <armadillo>
#include <cassert>

namespace {
const std::complex<double> I{ 0.0, 1.0};
}

using namespace xacc;

namespace xacc {
namespace algorithm {
bool QITE::initialize(const HeterogeneousMap &parameters) 
{
  bool initializeOk = true;
  if (!parameters.pointerLikeExists<Accelerator>("accelerator")) 
  {
    std::cout << "'accelerator' is required.\n";
    initializeOk = false;
  }

  if (!parameters.keyExists<int>("steps")) 
  {
    std::cout << "'steps' is required.\n";
    initializeOk = false;
  }

  if (!parameters.keyExists<double>("step-size")) 
  {
    std::cout << "'step-size' is required.\n";
    initializeOk = false;
  }

  if (!parameters.pointerLikeExists<Observable>("observable")) 
  {
    std::cout << "'observable' is required.\n";
    initializeOk = false;
  }
  
  if (initializeOk)
  {
    m_accelerator = xacc::as_shared_ptr(parameters.getPointerLike<Accelerator>("accelerator"));
    m_nbSteps = parameters.get<int>("steps");
    m_dBeta = parameters.get<double>("step-size");
    m_observable = xacc::as_shared_ptr(parameters.getPointerLike<Observable>("observable"));
  }

  return initializeOk;
}

const std::vector<std::string> QITE::requiredParameters() const 
{
  return { "accelerator", "steps", "step-size", "observable" };
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
  // Calculate approximate A operator observable at each time step.
  const auto calcAOps = [&](std::shared_ptr<CompositeInstruction> in_kernel, std::shared_ptr<Observable> in_hmTerm) -> std::shared_ptr<Observable> {
    assert(in_hmTerm->getSubTerms().size() == 1);
    assert(in_hmTerm->getNonIdentitySubTerms().size() == 1);

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
    arma::cx_mat S_Mat(sMatDim, sMatDim, arma::fill::zeros);
    arma::cx_vec b_Vec(sMatDim, arma::fill::zeros); 
    
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
    const auto observableToVec = [](std::shared_ptr<Observable> in_observable, const std::vector<std::string>& in_pauliObsList) {
      // Return true if the *operators* of the two terms are identical
      // e.g. a Z0X1 and b Z0X1 -> true 
      const auto comparePauliString = [](const std::string& in_a, const std::string& in_b) -> bool {
        // Strip the coefficient part
        auto opA = in_a.substr(in_a.find_last_of(")") + 1);
        auto opB = in_b.substr(in_b.find_last_of(")") + 1);
        opA.erase(std::remove(opA.begin(), opA.end(), ' '), opA.end()); 
        opB.erase(std::remove(opB.begin(), opB.end(), ' '), opB.end()); 
        return opA == opB;
      };

      std::vector<double> obsProjCoeffs(in_pauliObsList.size(), 0.0);

      const auto findMatchingIndex = [&](const std::string& in_obsStr){
        for (int i = 0; i < in_pauliObsList.size(); ++i)
        {
          std::shared_ptr<Observable> obs = std::make_shared<xacc::quantum::PauliOperator>();
          const std::string pauliObsStr = "1.0 " + in_pauliObsList[i];
          obs->fromString(pauliObsStr);
          
          if (comparePauliString(obs->toString(), in_obsStr))
          {
            return i;
          }
        }
        // Failed!
        return -1;
      };

      for (const auto& term: in_observable->getNonIdentitySubTerms())
      {
        const auto index = findMatchingIndex(term->toString());
        assert(index >= 0);
        obsProjCoeffs[index] = term->coefficient().real();
      }
      return obsProjCoeffs;
    };

    const auto obsProjCoeffs = observableToVec(in_hmTerm, pauliObsOps);
    std::cout << "Observable Pauli Vec: [";
    for (const auto& elem: obsProjCoeffs)
    {
      std::cout << elem << ", ";
    }
    std::cout << "]\n";

    // Calculate c: Eq. 3 in https://arxiv.org/pdf/1901.07653.pdf
    double c = 1.0;
    for (int i = 0; i < obsProjCoeffs.size(); ++i)
    {
      c -= 2.0 * m_dBeta * obsProjCoeffs[i] * sigmaExpectation[i];
    }
    
    std::cout << "c = " << c << "\n";
    for (int i = 0; i < sMatDim; ++i)
    {
      std::complex<double> b = (sigmaExpectation[i]/ std::sqrt(c) - sigmaExpectation[i])/ m_dBeta;
      for (int j = 0; j < obsProjCoeffs.size(); ++j)
      {
        // The expectation of the pauli product of the Hamiltonian term
        // and the sweeping pauli term.
        const auto expectVal = calcSmatEntry(sigmaExpectation, i, j);
        b -= obsProjCoeffs[j] * expectVal / std::sqrt(c);
      }
      b = I*b - I*std::conj(b);
      // Set b_Vec
      b_Vec(i) = b;
    }

    std::cout << "S Matrix: \n" << S_Mat << "\n"; 
    std::cout << "B Vector: \n" << b_Vec << "\n"; 
    // Add regularizer
	  arma::cx_mat dalpha(sMatDim, sMatDim, arma::fill::eye); 
    dalpha = 0.1 * dalpha;

    auto lhs = S_Mat + S_Mat.st() + dalpha;
    auto rhs = -b_Vec;

    std::cout << "LHS Matrix: \n" << lhs << "\n"; 
    std::cout << "RHS Vector: \n" << rhs << "\n"; 
    arma::cx_vec a_Vec = arma::solve(lhs, rhs);
    std::cout << "Result A Vector: \n" << a_Vec << "\n"; 

    // Now, we have the decomposition of A observable in the basis of
    // all possible Pauli combinations.
    assert(a_Vec.n_elem == pauliObsOps.size());
    const std::string aObsStr = [&](){
      std::stringstream s;
      s.precision(12);
      s << std::fixed << a_Vec(0);
      
      for (int i = 1; i < pauliObsOps.size(); ++i)
      {
        s << " + " << a_Vec(i) << " " << pauliObsOps[i];
      }

      return s.str();
    }(); 

    std::cout << "A string: " <<  aObsStr << "\n";

    return nullptr; 
  };
  
  // Time stepping:
  for (int i = 0; i < m_nbSteps; ++i)
  {
    for (auto& hamTerm : m_observable->getNonIdentitySubTerms())
    {
      // Propagates the state via Trotter steps:
      auto kernel = constructPropagateCircuit();
      // Optimizes/calculates next A ops
      auto nextAOps = calcAOps(kernel, hamTerm);
      m_approxOps.emplace_back(nextAOps);
    }
   
  }
}

std::vector<double> QITE::execute(const std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<double>& x) 
{
  // TODO
  return {};
}
} // namespace algorithm
} // namespace xacc