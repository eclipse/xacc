/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Daniel Claudino - initial API and implementation
 *******************************************************************************/

#include "adapt.hpp"

#include "AcceleratorBuffer.hpp"
#include "FermionOperator.hpp"
#include "ObservableTransform.hpp"
#include "PauliOperator.hpp"
#include "Utils.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "Circuit.hpp"
#include "AlgorithmGradientStrategy.hpp"

#include <complex>
#include <cstddef>
#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace algorithm {

bool ADAPT::initialize(const HeterogeneousMap &parameters) {

  if (!parameters.pointerLikeExists<Observable>("observable")) {
    xacc::info("Obs was false\n");
    return false;
  }

  if (!parameters.pointerLikeExists<Accelerator>("accelerator")) {
    xacc::info("Acc was false\n");
    return false;
  }

  if (!parameters.stringExists("pool")) {
    xacc::info("Pool was false\n");
    return false;
  }

  if (!parameters.stringExists("sub-algorithm")) {
    xacc::info("Sub algorithm was false\n");
    return false;
  }

  optimizer = parameters.get<std::shared_ptr<Optimizer>>("optimizer");
  observable = parameters.get<std::shared_ptr<Observable>>("observable");
  accelerator = parameters.get<std::shared_ptr<Accelerator>>("accelerator");
  pool = xacc::getService<OperatorPool>(parameters.getString("pool"));
  subAlgorithm = parameters.getString("sub-algorithm");

  if (parameters.keyExists<int>("maxiter")) {
    _maxIter = parameters.get<int>("maxiter");
  }

  if (parameters.keyExists<double>("adapt-threshold")) {
    _adaptThreshold = parameters.get<double>("adapt-threshold");
  }

  if (parameters.keyExists<double>("print-threshold")) {
    _printThreshold = parameters.get<double>("print-threshold");
  }

  if (parameters.keyExists<bool>("print-operators")) {
    _printOps = parameters.get<bool>("print-operators");
  }

  if (parameters.stringExists("gradient_strategy")) {
    gradStrategyName = parameters.getString("gradient_strategy");
  }

  if (parameters.pointerLikeExists<CompositeInstruction>("initial-state")) {
    initialState =
        parameters.get<std::shared_ptr<CompositeInstruction>>("initial-state");
  }

  // adding the number of particles for future model Hamiltonians
  if (parameters.keyExists<int>("n-electrons") ||
      parameters.keyExists<int>("n-particles")) {
    _nParticles = parameters.get<int>("n-electrons");
  }

  if (subAlgorithm != "QAOA" && !(parameters.keyExists<int>("n-electrons") ||
                                  parameters.keyExists<int>("n-particles"))) {
    xacc::error("This simulation needs the number of particles/electrons.");
    return false;
  }

  if (pool->isNumberOfParticlesRequired()) {
    if (parameters.keyExists<int>("n-electrons") ||
        parameters.keyExists<int>("n-particles")) {
      pool->optionalParameters({{"n-electrons", _nParticles}});
    } else {
      xacc::error(
          "The chosen pool requires the number of particles/electrons.");
      return false;
    }
  }

  if (observable->toString().find("^") != std::string::npos) {

    auto jw = xacc::getService<ObservableTransform>("jw");
    if (std::dynamic_pointer_cast<FermionOperator>(observable)) {
      observable = jw->transform(observable);
    } else {
      auto fermionObservable =
          xacc::quantum::getObservable("fermion", observable->toString());
      observable = jw->transform(
          std::dynamic_pointer_cast<Observable>(fermionObservable));
    }

    // observable is PauliOperator, but does not cast down to it
    // Not sure about the likelihood of this happening, but want to cover all
    // bases
  } else if (observable->toString().find("X") != std::string::npos ||
             observable->toString().find("Y") != std::string::npos ||
             observable->toString().find("Z") != std::string::npos &&
                 !std::dynamic_pointer_cast<PauliOperator>(observable)) {

    auto pauliObservable =
        xacc::quantum::getObservable("pauli", observable->toString());
    observable = std::dynamic_pointer_cast<Observable>(pauliObservable);
  }

  return true;
}

const std::vector<std::string> ADAPT::requiredParameters() const {
  return {"observable", "optimizer", "accelerator", "pool", "sub-algorithm"};
}

void ADAPT::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  std::stringstream ss;
  ss << std::setprecision(12);
  // Instantiate adaptive circuit and add instructions from initial state
  // initial state for chemistry VQE is usually HF
  // initial state for QAOA is all |+>
  auto provider = xacc::getIRProvider("quantum");
  auto ansatz = provider->createComposite("ansatzCircuit");

  if (initialState) {

    for (auto &inst : initialState->getInstructions()) {
      ansatz->addInstruction(inst);
    }

  } else {

    if (xacc::container::contains(physicalSubAlgorithms, subAlgorithm)) {
      for (auto &inst :
           getHartreeFockState(buffer->size())->getInstructions()) {
        ansatz->addInstruction(inst);
      }
    }

    if (subAlgorithm == "QAOA") {
      for (auto &inst :
           getQAOAInitialState(buffer->size())->getInstructions()) {
        ansatz->addInstruction(inst);
      }
    }
  }

  auto vqe = xacc::getAlgorithm("vqe", {{"observable", observable},
                                        {"accelerator", accelerator},
                                        {"ansatz", ansatz}});

  // Generate operators in the pool
  auto operators = pool->generate(buffer->size());
  std::vector<int> ansatzOps;

  // Vector of commutators, need to compute them only once
  std::vector<std::shared_ptr<Observable>> commutators;
  if (subAlgorithm == "QAOA") {

    std::complex<double> i(0, 1);
    for (auto &op : operators) {

      auto comm = observable->commutator(op);
      auto &tmp = *std::dynamic_pointer_cast<PauliOperator>(comm);
      tmp = tmp * i;
      commutators.push_back(std::make_shared<PauliOperator>(tmp));
    }

  } else {

    for (auto &op : operators) {
      commutators.push_back(observable->commutator(op));
    }
  }

  // Gather all unique commutator terms
  std::map<std::string, std::shared_ptr<Observable>> uniqueCommutatorTerms;
  for (auto &commutator : commutators) {

    for (auto &term : commutator->getSubTerms()) {

      auto termName = std::dynamic_pointer_cast<PauliOperator>(term)
                          ->getTerms()
                          .begin()
                          ->first;

      if (uniqueCommutatorTerms.find(termName) == uniqueCommutatorTerms.end()) {
        uniqueCommutatorTerms[termName] = term;
      }
    }
  }

  xacc::info("Operator pool: " + pool->name());
  xacc::info("Number of operators in the pool: " +
             std::to_string(operators.size()));
  double oldEnergy = 0.0;
  std::vector<double> x; // these are the variational parameters

  // start ADAPT loop
  for (int iter = 0; iter < _maxIter; iter++) {

    xacc::info("Iteration: " + std::to_string(iter + 1));
    xacc::info("Computing [H, A]");
    xacc::info("Printing commutators with absolute value above " +
               std::to_string(_printThreshold));

    if (subAlgorithm == "QAOA") {

      auto costHamiltonianGates = std::dynamic_pointer_cast<quantum::Circuit>(
          xacc::getService<Instruction>("exp_i_theta"));

      // Create instruction for new operator
      costHamiltonianGates->expand(
          {{"pauli", observable->toString()},
           {"param_id", "x" + std::to_string(ansatz->nVariables())}});

      ansatz->addVariable("x" + std::to_string(ansatz->nVariables()));
      for (auto &inst : costHamiltonianGates->getInstructions()) {
        ansatz->addInstruction(inst);
      }
      x.insert(x.begin(), 0.01);
    }

    // observe each term with the current ansatz
    std::vector<std::shared_ptr<CompositeInstruction>> observedKernels;
    for (auto &term : uniqueCommutatorTerms) {

      std::vector<std::shared_ptr<CompositeInstruction>> termObservedKernel;
      if (x.empty()) {
        termObservedKernel = term.second->observe(ansatz);
      } else {
        auto tmp_x = x;
        std::reverse(tmp_x.begin(), tmp_x.end());
        auto evaled = ansatz->operator()(tmp_x);
        termObservedKernel = term.second->observe(evaled);
      }
      observedKernels.insert(observedKernels.end(), termObservedKernel.begin(),
                             termObservedKernel.end());
    }

    // execute all unique terms
    auto tmpBuffer = xacc::qalloc(buffer->size());
    accelerator->execute(tmpBuffer, observedKernels);

    std::map<std::string, std::shared_ptr<AcceleratorBuffer>> termToBuffer;
    for (auto &buffer : tmpBuffer->getChildren()) {
      termToBuffer[buffer->name()] = buffer;
    }

    int maxCommutatorIdx = 0;
    double maxCommutator = 0.0, gradientNorm = 0.0;

    // compute the commutators
    for (int operatorIdx = 0; operatorIdx < commutators.size(); operatorIdx++) {

      double commutatorValue = 0.0;
      for (auto &term : commutators[operatorIdx]->getNonIdentitySubTerms()) {
        auto termName = std::dynamic_pointer_cast<PauliOperator>(term)
                            ->getTerms()
                            .begin()
                            ->first;
        commutatorValue += std::real(term->coefficient()) *
                           termToBuffer[termName]->getExpectationValueZ();
      }

      // print commutator above threshold
      if (abs(commutatorValue) > _printThreshold) {
        ss << "[H," << operatorIdx << "] = " << commutatorValue;
        xacc::info(ss.str());
        ss.str(std::string());
      }

      // update maxCommutator
      if (abs(commutatorValue) > abs(maxCommutator)) {
        maxCommutatorIdx = operatorIdx;
        maxCommutator = commutatorValue;
      }

      gradientNorm += commutatorValue * commutatorValue;
    }

    ss << "Max gradient component: [H, " << maxCommutatorIdx
       << "] = " << maxCommutator << " a.u.";
    xacc::info(ss.str());
    ss.str(std::string());

    gradientNorm = std::sqrt(gradientNorm);
    ss << "Norm of gradient vector: " << gradientNorm << " a.u.";
    xacc::info(ss.str());
    ss.str(std::string());

    if (gradientNorm < _adaptThreshold) { // ADAPT converged

      xacc::info("ADAPT-" + subAlgorithm + " converged in " +
                 std::to_string(iter) + " iterations.");
      ss << "ADAPT-" << subAlgorithm << " energy: " << oldEnergy << " a.u.";
      xacc::info(ss.str());
      ss.str(std::string());

      ss << "Optimal parameters: ";
      for (auto param : x) {
        ss << param << " ";
      }
      xacc::info(ss.str());
      ss.str(std::string());

      xacc::info("Final ADAPT-" + subAlgorithm + " circuit\n" +
                 ansatz->toString());

      // Add the ansatz to the compilation database for later retrieval
      xacc::appendCompiled(ansatz, true);
      buffer->addExtraInfo("final-ansatz", ExtraInfo(ansatz->name()));
      return;

    } else if (iter < _maxIter) { // Add operator and reoptimize

      xacc::info(subAlgorithm + " optimization of current ansatz.");

      // keep track of growing ansatz
      ansatzOps.push_back(maxCommutatorIdx);

      // Instruction service for the operator to be added to the ansatz
      auto maxCommutatorGate =
          pool->getOperatorInstructions(maxCommutatorIdx, ansatz->nVariables());

      // Label for new variable and add it to the circuit
      ansatz->addVariable("x" + std::to_string(ansatz->nVariables()));

      // Append new instructions to current circuit
      for (auto &inst : maxCommutatorGate->getInstructions()) {
        ansatz->addInstruction(inst);
      }

      // Convergence is improved if passing initial parameters to optimizer
      if (subAlgorithm == "QAOA") {
        x.insert(x.begin(), xacc::constants::pi / 2.0);
      } else {
        // this should work for QAOA too, but I have not tested it
        auto normConstant = pool->getNormalizationConstant(maxCommutatorIdx);
        auto param = newParameter(ansatz, x, oldEnergy, normConstant);
        x.insert(x.begin(), param);
      }
      optimizer->appendOption("initial-parameters", x);

      // Instantiate gradient class
      std::shared_ptr<AlgorithmGradientStrategy> gradientStrategy;
      if (!gradStrategyName.empty() && optimizer->isGradientBased()) {
        gradientStrategy =
            xacc::getService<AlgorithmGradientStrategy>(gradStrategyName);
        gradientStrategy->initialize({{"observable", observable}});
      }

      // Start subAlgorithm optimization
      auto sub_opt = xacc::getAlgorithm(
          subAlgorithm, {{"observable", observable},
                         {"optimizer", optimizer},
                         {"accelerator", accelerator},
                         {"gradient_strategy", gradientStrategy},
                         {"ansatz", ansatz}});

      double newEnergy;
      if (subAlgorithm == "vqe" && iter == 0) {
        newEnergy = sub_opt->execute(buffer, x)[0];
      } else {
        sub_opt->execute(buffer);
        newEnergy = (*buffer)["opt-val"].as<double>();
        x = (*buffer)["opt-params"].as<std::vector<double>>();
      }

      oldEnergy = newEnergy;
      ss << "Energy at ADAPT iteration " << iter + 1 << ": " << newEnergy;
      xacc::info(ss.str());
      ss.str(std::string());

      ss << "Parameters at ADAPT iteration " << iter + 1 << ": ";
      for (auto param : x) {
        ss << param << " ";
      }
      xacc::info(ss.str());
      ss.str(std::string());

      ss << "Ansatz at ADAPT iteration " << std::to_string(iter + 1) << ": ";
      for (auto op : ansatzOps) {
        ss << op << " ";
      }
      xacc::info(ss.str());
      ss.str(std::string());

      if (_printOps) {
        xacc::info("Printing operators at ADAPT iteration " +
                   std::to_string(iter + 1));
        for (auto op : ansatzOps) {
          xacc::info("Operator index: " + std::to_string(op));
          xacc::info(pool->operatorString(op));
        }
      }

      buffer->addExtraInfo("opt-val", ExtraInfo(oldEnergy));
      buffer->addExtraInfo("opt-params", ExtraInfo(x));
      buffer->addExtraInfo("opt-ansatz", ExtraInfo(ansatzOps));

    } else {
      xacc::info("ADAPT-" + subAlgorithm + " did not converge in " +
                 std::to_string(_maxIter) + " iterations.");
      return;
    }
  }
}

std::shared_ptr<CompositeInstruction>
ADAPT::getHartreeFockState(const std::size_t nBits) const {

  auto provider = xacc::getIRProvider("quantum");
  auto state = provider->createComposite("initial_state");
  for (std::size_t i = 0; i < _nParticles / 2; i++) {
    state->addInstruction(provider->createInstruction("X", {i}));
    auto j = (i + nBits / 2);
    state->addInstruction(provider->createInstruction("X", {j}));
  }
  return state;
}

std::shared_ptr<CompositeInstruction>
ADAPT::getQAOAInitialState(const std::size_t nBits) const {

  auto provider = xacc::getIRProvider("quantum");
  auto state = provider->createComposite("initial_state");
  for (std::size_t i = 0; i < nBits; i++) {
    state->addInstruction(provider->createInstruction("H", {i}));
  }
  return state;
}

// initializes new parameter from partial tomography ideas
// where all previous parameters are frozen
double ADAPT::newParameter(const std::shared_ptr<CompositeInstruction> kernel,
                           const std::vector<double> &x, double zeroExpValue,
                           const double normConstant) const {

  auto vqe = xacc::getAlgorithm("vqe", {{"observable", observable},
                                        {"accelerator", accelerator},
                                        {"ansatz", kernel}});

  std::vector<double> tmp_x;
  auto nBits = observable->nBits();
  // if this is the first iteration, we don't have the
  // the energy/expectation value E(0.0, ...)
  // for the other iterations, we use oldEnergy
  if (zeroExpValue == 0.0) {
    tmp_x = x;
    tmp_x.insert(tmp_x.begin(), 0.0);
    zeroExpValue = vqe->execute(xacc::qalloc(nBits), tmp_x)[0];
  }

  tmp_x = x;
  tmp_x.insert(tmp_x.begin(), normConstant * xacc::constants::pi / 2.0);
  auto plusExpValue = vqe->execute(xacc::qalloc(nBits), tmp_x)[0];

  tmp_x = x;
  tmp_x.insert(tmp_x.begin(), -normConstant * xacc::constants::pi / 2.0);
  auto minusExpValue = vqe->execute(xacc::qalloc(nBits), tmp_x)[0];

  auto B = zeroExpValue - (plusExpValue + minusExpValue) / 2.0;
  auto C = (plusExpValue - minusExpValue) / 2.0;
  auto newParam = std::atan2(-C, -B) * normConstant;

  return newParam;
}

} // namespace algorithm
} // namespace xacc