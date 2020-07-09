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
#include "CompositeInstruction.hpp"
#include "FermionOperator.hpp"
#include "Observable.hpp"
#include "ObservableTransform.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_observable.hpp"
#include "Circuit.hpp"
#include "AlgorithmGradientStrategy.hpp"

#include <complex>
#include <memory>
#include <iomanip>
#include <sstream>
#include <string>

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
  subAlgo = parameters.getString("sub-algorithm");

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

  if (parameters.keyExists<int>("n-electrons")) {
    _nElectrons = parameters.get<int>("n-electrons");
  }

  if ((subAlgo == "vqe" && !initialState) &&
      (subAlgo == "vqe" && !parameters.keyExists<int>("n-electrons"))) {

    xacc::info("VQE requires number of electrons or initial state.");
  }

  if (parameters.getString("pool") == "singlet-adapted-uccsd" &&
      parameters.keyExists<int>("n-electrons")) {

    pool->optionalParameters({std::make_pair("n-electrons", _nElectrons)});
  }

  // Check if Observable is Fermion or Pauli and manipulate accordingly
  //
  // if string has ^, it's FermionOperator
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

  // Instantiate adaptive circuit and add instructions from initial state
  // initial state for chemistry VQE is usually HF
  // initial state for QAOA is all |+>
  auto ansatzRegistry = xacc::getIRProvider("quantum");
  auto ansatzInstructions = ansatzRegistry->createComposite("ansatzCircuit");

  if (initialState) {

    for (auto &inst : initialState->getInstructions()) {
      ansatzInstructions->addInstruction(inst);
    }

  } else {

    if (subAlgo == "vqe") {
      // Define the initial state, usually HF for chemistry problems
      std::size_t j;
      for (int i = 0; i < _nElectrons / 2; i++) {
        j = (std::size_t)i;
        auto alphaXGate =
            ansatzRegistry->createInstruction("X", std::vector<std::size_t>{j});
        ansatzInstructions->addInstruction(alphaXGate);
        j = (std::size_t)(i + buffer->size() / 2);
        auto betaXGate =
            ansatzRegistry->createInstruction("X", std::vector<std::size_t>{j});
        ansatzInstructions->addInstruction(betaXGate);
      }
    }

    if (subAlgo == "QAOA") {
      std::size_t j;
      for (int i = 0; i < buffer->size(); i++) {
        j = (std::size_t)i;
        auto H =
            ansatzRegistry->createInstruction("H", std::vector<std::size_t>{j});
        ansatzInstructions->addInstruction(H);
      }
    }
  }

  // Generate operators in the pool
  auto operators = pool->generate(buffer->size());
  std::vector<int> ansatzOps;

  // Vector of commutators, need to compute them only once
  std::vector<std::shared_ptr<Observable>> commutators;
  if (subAlgo == "QAOA") {

    std::complex<double> i(0, 1);
    for (auto op : operators) {
      auto comm = observable->commutator(op);
      auto &tmp = *std::dynamic_pointer_cast<PauliOperator>(comm);
      tmp = tmp * i;
      auto ptr = std::dynamic_pointer_cast<Observable>(
          std::make_shared<PauliOperator>(tmp));
      commutators.push_back(ptr);
    }

  } else {

    for (auto op : operators) {
      commutators.push_back(observable->commutator(op));
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

    if (subAlgo == "QAOA") {

      auto costHamiltonianGates = std::dynamic_pointer_cast<quantum::Circuit>(
          xacc::getService<Instruction>("exp_i_theta"));

      // Create instruction for new operator
      costHamiltonianGates->expand(
          {std::make_pair("pauli", observable->toString()),
           std::make_pair(
               "param_id",
               "x" + std::to_string(ansatzInstructions->nVariables()))});

      ansatzInstructions->addVariable(
          "x" + std::to_string(ansatzInstructions->nVariables()));
      for (auto &inst : costHamiltonianGates->getInstructions()) {
        ansatzInstructions->addInstruction(inst);
      }
      x.insert(x.begin(), 0.01);
    }

    int maxCommutatorIdx = 0;
    double maxCommutator = 0.0;
    double gradientNorm = 0.0;

    // Loop over non-vanishing commutators and select the one with largest
    // magnitude
    for (int operatorIdx = 0; operatorIdx < commutators.size(); operatorIdx++) {

      // only compute commutators if they aren't zero
      int nTermsCommutator =
          std::dynamic_pointer_cast<PauliOperator>(commutators[operatorIdx])
              ->getTerms()
              .size();
      if (nTermsCommutator != 0) {

        // Print number of instructions for computing <observable>
        xacc::info("Number of instructions for commutator calculation: " +
                   std::to_string(nTermsCommutator));
        // observe the commutators with the updated circuit ansatz
        auto grad_algo = xacc::getAlgorithm(
            subAlgo, {std::make_pair("observable", commutators[operatorIdx]),
                      std::make_pair("optimizer", optimizer),
                      std::make_pair("accelerator", accelerator),
                      std::make_pair("ansatz", ansatzInstructions)});
        auto tmp_buffer = xacc::qalloc(buffer->size());
        auto commutatorValue = std::real(grad_algo->execute(tmp_buffer, x)[0]);

        if (abs(commutatorValue) > _printThreshold) {
          ss << std::setprecision(12) << "[H," << operatorIdx
             << "] = " << commutatorValue;
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
    }

    ss << std::setprecision(12) << "Max gradient component: [H, "
       << maxCommutatorIdx << "] = " << maxCommutator << " a.u.";
    xacc::info(ss.str());
    ss.str(std::string());

    gradientNorm = std::sqrt(gradientNorm);
    ss << std::setprecision(12) << "Norm of gradient vector: " << gradientNorm
       << " a.u.";
    xacc::info(ss.str());
    ss.str(std::string());

    if (gradientNorm < _adaptThreshold) { // ADAPT converged

      xacc::info("ADAPT-" + subAlgo + " converged in " + std::to_string(iter) +
                 " iterations.");
      ss << std::setprecision(12) << "ADAPT-" << subAlgo
         << " energy: " << oldEnergy << " a.u.";
      xacc::info(ss.str());
      ss.str(std::string());

      ss << "Optimal parameters: ";
      for (auto param : x) {
        ss << std::setprecision(12) << param << " ";
      }
      xacc::info(ss.str());
      ss.str(std::string());

      xacc::info("Final ADAPT-" + subAlgo + " circuit\n" +
                 ansatzInstructions->toString());

      return;

    } else if (iter < _maxIter) { // Add operator and reoptimize

      xacc::info(subAlgo + " optimization of current ansatz.");

      // keep track of growing ansatz
      ansatzOps.push_back(maxCommutatorIdx);

      // Instruction service for the operator to be added to the ansatz
      auto maxCommutatorGate = pool->getOperatorInstructions(
          maxCommutatorIdx, ansatzInstructions->nVariables());

      // Label for new variable and add it to the circuit
      ansatzInstructions->addVariable(
          "x" + std::to_string(ansatzInstructions->nVariables()));

      // Append new instructions to current circuit
      for (auto &inst : maxCommutatorGate->getInstructions()) {
        ansatzInstructions->addInstruction(inst);
      }

      // Convergence is improved if passing initial parameters to optimizer
      // so we create a new instance of Optimizer with them
      // insert 0.0 for VQE and pi/2 for QAOA

      // Instantiate gradient class
      std::shared_ptr<AlgorithmGradientStrategy> gradientStrategy;
      std::shared_ptr<Optimizer> newOptimizer;
      if (!gradStrategyName.empty() && subAlgo == "vqe" &&
          optimizer->isGradientBased()) {

        gradientStrategy =
            xacc::getService<AlgorithmGradientStrategy>(gradStrategyName);
        gradientStrategy->initialize(
            {std::make_pair("observable", observable),
             std::make_pair("commutator", commutators[maxCommutatorIdx])});
        x.insert(x.begin(), 0.0);
        newOptimizer = xacc::getOptimizer(
            optimizer->name(), {std::make_pair(optimizer->name() + "-optimizer",
                                               optimizer->get_algorithm()),
                                std::make_pair("initial-parameters", x)});

      } else if (!gradStrategyName.empty() && subAlgo == "QAOA" &&
                 optimizer->isGradientBased()) {

        gradientStrategy =
            xacc::getService<AlgorithmGradientStrategy>(gradStrategyName);
        gradientStrategy->initialize(
            {std::make_pair("observable", observable)});
        x.insert(x.begin(), xacc::constants::pi / 2.0);
        newOptimizer = xacc::getOptimizer(
            optimizer->name(), {std::make_pair(optimizer->name() + "-optimizer",
                                               optimizer->get_algorithm()),
                                std::make_pair("initial-parameters", x)});

      } else {
        gradientStrategy = nullptr;
        newOptimizer = xacc::getOptimizer(
            optimizer->name(), {std::make_pair(optimizer->name() + "-optimizer",
                                               optimizer->get_algorithm())});
      }

      // Start subAlgo optimization
      auto sub_opt = xacc::getAlgorithm(
          subAlgo, {std::make_pair("observable", observable),
                    std::make_pair("optimizer", newOptimizer),
                    std::make_pair("accelerator", accelerator),
                    std::make_pair("gradient_strategy", gradientStrategy),
                    std::make_pair("ansatz", ansatzInstructions)});
      sub_opt->execute(buffer);

      auto newEnergy = (*buffer)["opt-val"].as<double>();
      x = (*buffer)["opt-params"].as<std::vector<double>>();
      oldEnergy = newEnergy;

      ss << std::setprecision(12) << "Energy at ADAPT iteration " << iter + 1
         << ": " << newEnergy;
      xacc::info(ss.str());
      ss.str(std::string());

      ss << std::setprecision(12) << "Parameters at ADAPT iteration "
         << iter + 1 << ": ";
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
      xacc::info("ADAPT-" + subAlgo + " did not converge in " +
                 std::to_string(_maxIter) + " iterations.");
      return;
    }
  }
}

} // namespace algorithm
} // namespace xacc