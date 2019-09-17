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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "exp.hpp"
#include "FermionOperator.hpp"
#include "IRProvider.hpp"
#include "ObservableTransform.hpp"
#include "PauliOperator.hpp"

#include "xacc.hpp"
#include "xacc_service.hpp"
#include <memory>
#include <regex>

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace circuits {
const std::vector<std::string> Exp::requiredKeys() {
  return {"pauli", "fermion"};
}

bool Exp::expand(const HeterogeneousMap &parameters) {

  std::string pauli_or_fermion = "pauli";
  if (!parameters.stringExists("pauli")) {
    if (!parameters.stringExists("fermion")) {
      return false;
    }
    pauli_or_fermion = "fermion";
  }

  std::string paramLetter = "t";
  if (parameters.stringExists("param_id")) {
    paramLetter = parameters.getString("param_id");
  }

  std::unordered_map<std::string, xacc::quantum::Term> terms;
  if (pauli_or_fermion == "fermion") {
    auto fermionStr = parameters.getString("fermion");
    auto op = std::make_shared<FermionOperator>(fermionStr);
    terms = std::dynamic_pointer_cast<PauliOperator>(
                xacc::getService<ObservableTransform>("jw")->transform(op))
                ->getTerms();
  } else {
    auto pauliStr = parameters.getString("pauli");
    PauliOperator op(pauliStr);
    terms = op.getTerms();
  }

  double pi = xacc::constants::pi;
  auto gateRegistry = xacc::getService<IRProvider>("quantum");
  addVariable(paramLetter);
  for (auto inst : terms) {

    Term spinInst = inst.second;

    // Get the individual pauli terms
    auto termsMap = std::get<2>(spinInst);

    std::vector<std::pair<int, std::string>> terms;
    for (auto &kv : termsMap) {
      if (kv.second != "I" && !kv.second.empty()) {
        terms.push_back({kv.first, kv.second});
      }
    }
    // The largest qubit index is on the last term
    int largestQbitIdx = terms[terms.size() - 1].first;
    auto tempFunction = gateRegistry->createComposite("temp", {paramLetter});

    for (int i = 0; i < terms.size(); i++) {

      std::size_t qbitIdx = terms[i].first;
      auto gateName = terms[i].second;

      if (i < terms.size() - 1) {
        std::size_t tmp = terms[i + 1].first;
        auto cnot = gateRegistry->createInstruction(
            "CNOT", std::vector<std::size_t>{qbitIdx, tmp});
        tempFunction->addInstruction(cnot);
      }

      if (gateName == "X") {
        auto hadamard = gateRegistry->createInstruction(
            "H", std::vector<std::size_t>{qbitIdx});
        tempFunction->insertInstruction(0, hadamard);
      } else if (gateName == "Y") {
        auto rx = gateRegistry->createInstruction(
            "Rx", std::vector<std::size_t>{qbitIdx});
        InstructionParameter p(pi / 2.0);
        rx->setParameter(0, p);
        tempFunction->insertInstruction(0, rx);
      }

      // Add the Rotation for the last term
      if (i == terms.size() - 1) {
        // FIXME DONT FORGET DIVIDE BY 2
        std::stringstream ss;
        if (pauli_or_fermion == "pauli") {
          ss << std::to_string(std::real(spinInst.coeff())) << " * "
             << paramLetter;
        } else {
          ss << std::to_string(std::imag(spinInst.coeff())) << " * "
             << paramLetter;
        }
        auto rz = gateRegistry->createInstruction(
            "Rz", std::vector<std::size_t>{qbitIdx});

        InstructionParameter p(ss.str());
        rz->setParameter(0, p);
        tempFunction->addInstruction(rz);
      }
    }

    int counter = tempFunction->nInstructions();
    // Add the instruction on the backend of the circuit
    for (int i = terms.size() - 1; i >= 0; i--) {

      std::size_t qbitIdx = terms[i].first;
      auto gateName = terms[i].second;

      if (i < terms.size() - 1) {
        std::size_t tmp = terms[i + 1].first;
        auto cnot = gateRegistry->createInstruction(
            "CNOT", std::vector<std::size_t>{qbitIdx, tmp});
        tempFunction->insertInstruction(counter, cnot);
        counter++;
      }

      if (gateName == "X") {
        auto hadamard = gateRegistry->createInstruction(
            "H", std::vector<std::size_t>{qbitIdx});
        tempFunction->addInstruction(hadamard);
      } else if (gateName == "Y") {
        auto rx = gateRegistry->createInstruction(
            "Rx", std::vector<std::size_t>{qbitIdx});
        InstructionParameter p(-1.0 * (pi / 2.0));
        rx->setParameter(0, p);
        tempFunction->addInstruction(rx);
      }
    }
    // Add to the total UCCSD State Prep function
    for (auto inst : tempFunction->getInstructions()) {
      addInstruction(inst);
    }
  }

  return true;
} // namespace instructions

} // namespace circuits
} // namespace xacc