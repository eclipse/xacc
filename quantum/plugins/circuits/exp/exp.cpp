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
#include "Instruction.hpp"
#include "ObservableTransform.hpp"
#include "PauliOperator.hpp"

#include "Utils.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include <memory>
#include <regex>

#include <Eigen/Dense>

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

  std::string paramLetter = nVariables() == 0 ? "t" : getVariables()[0];
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
  std::string xasm_src = "";

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

    // this is required when the operator has scalar terms
    if(terms.size() == 0){
      continue;
    }

    // The largest qubit index is on the last term
    int largestQbitIdx = terms[terms.size() - 1].first;

    std::vector<std::size_t> qidxs;
    std::stringstream basis_front, basis_back;

    for (auto &term : terms) {

      auto qid = term.first;
      auto pop = term.second;

      qidxs.push_back(qid);

      if (pop == "X") {

        basis_front << "H(q[" << qid << "]);\n";
        basis_back << "H(q[" << qid << "]);\n";

      } else if (pop == "Y") {
        basis_front << "Rx(q[" << qid << "], " << 1.57079362679 << ");\n";
        basis_back << "Rx(q[" << qid << "], " << -1.57079362679 << ");\n";
      }
    }

    // std::cout << "QIDS:  " << qidxs << "\n";

    Eigen::MatrixXi cnot_pairs(2, qidxs.size() - 1);
    for (int i = 0; i < qidxs.size() - 1; i++) {
      cnot_pairs(0, i) = qidxs[i];
    }
    for (int i = 0; i < qidxs.size() - 1; i++) {
      cnot_pairs(1, i) = qidxs[i + 1];
    }

    // std::cout << "HOWDY: \n" << cnot_pairs << "\n";
    std::stringstream cnot_front, cnot_back;
    for (int i = 0; i < qidxs.size() - 1; i++) {
      Eigen::VectorXi pairs = cnot_pairs.col(i);
      auto c = pairs(0);
      auto t = pairs(1);
      cnot_front << "CNOT(q[" << c << "], q[" << t << "]);\n";
    }

    for (int i = qidxs.size() - 2; i >= 0; i--) {
      Eigen::VectorXi pairs = cnot_pairs.col(i);
      auto c = pairs(0);
      auto t = pairs(1);
      cnot_back << "CNOT(q[" << c << "], q[" << t << "]);\n";
    }

    xasm_src = xasm_src + "\n" + basis_front.str() + cnot_front.str();

    // Since the coefficient here is either purely imaginary
    // or purely real, I did away with the no-i boolean and
    // added this if here. 
    double coeff;
    if (std::real(spinInst.coeff()) != 0.0) {
      coeff = std::real(spinInst.coeff());
    } else {
      coeff = std::imag(spinInst.coeff());
    }

    xasm_src = xasm_src + "Rz(q[" + std::to_string(qidxs[qidxs.size() - 1]) +
                "], " + std::to_string(2.*coeff) + " * " +
                paramLetter + ");\n";

    xasm_src = xasm_src + cnot_back.str() + basis_back.str();
  }

  int name_counter = 1;
  std::string name = "exp_tmp";
  while (xacc::hasCompiled(name)) {
    name += std::to_string(name_counter);
  }

  xasm_src = "__qpu__ void " + name + "(qbit q, double " + paramLetter +
             ") {\n" + xasm_src + "}";

  auto xasm = xacc::getCompiler("xasm");
  auto tmp = xasm->compile(xasm_src)->getComposites()[0];

  for (auto inst : tmp->getInstructions())
    addInstruction(inst);

  return true;
}

void Exp::applyRuntimeArguments() {

  // we expect first argument to be the variable name
  // we expect the second argument to be the observable

  std::string variable_name = arguments[0]->name;

  double x_val;
  if (arguments[0]->type.find("std::vector<double>") != std::string::npos) {
    x_val = arguments[0]->runtimeValue.get<std::vector<double>>(
        INTERNAL_ARGUMENT_VALUE_KEY)[vector_mapping[variable_name]];
    variable_name =
        arguments[0]->name +std::to_string(vector_mapping[variable_name]);
    addVariable(variable_name);
  } else {
    x_val = arguments[0]->runtimeValue.get<double>(INTERNAL_ARGUMENT_VALUE_KEY);
  }

  auto observable = arguments[1]->runtimeValue.getPointerLike<Observable>(
      INTERNAL_ARGUMENT_VALUE_KEY);

  // Have to make sure this wasn't already expanded
  if (nInstructions() == 0) {
    std::unordered_map<std::string, xacc::quantum::Term> terms;
    if (dynamic_cast<FermionOperator *>(observable)) {
      terms = std::dynamic_pointer_cast<PauliOperator>(
                  xacc::getService<ObservableTransform>("jw")->transform(
                      xacc::as_shared_ptr(observable)))

                  ->getTerms();
    } else {
      terms = dynamic_cast<PauliOperator *>(observable)->getTerms();
    }

    double pi = xacc::constants::pi;
    auto gateRegistry = xacc::getService<IRProvider>("quantum");
    std::string xasm_src = "";

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

      std::vector<std::size_t> qidxs;
      std::stringstream basis_front, basis_back;

      for (auto &term : terms) {

        auto qid = term.first;
        auto pop = term.second;

        qidxs.push_back(qid);

        if (pop == "X") {

          basis_front << "H(q[" << qid << "]);\n";
          basis_back << "H(q[" << qid << "]);\n";

        } else if (pop == "Y") {
          basis_front << "Rx(q[" << qid << "], " << 1.57079362679 << ");\n";
          basis_back << "Rx(q[" << qid << "], " << -1.57079362679 << ");\n";
        }
      }

      // std::cout << "QIDS:  " << qidxs << "\n";

      Eigen::MatrixXi cnot_pairs(2, qidxs.size() - 1);
      for (int i = 0; i < qidxs.size() - 1; i++) {
        cnot_pairs(0, i) = qidxs[i];
      }
      for (int i = 0; i < qidxs.size() - 1; i++) {
        cnot_pairs(1, i) = qidxs[i + 1];
      }

      // std::cout << "HOWDY: \n" << cnot_pairs << "\n";
      std::stringstream cnot_front, cnot_back;
      for (int i = 0; i < qidxs.size() - 1; i++) {
        Eigen::VectorXi pairs = cnot_pairs.col(i);
        auto c = pairs(0);
        auto t = pairs(1);
        cnot_front << "CNOT(q[" << c << "], q[" << t << "]);\n";
      }

      for (int i = qidxs.size() - 2; i >= 0; i--) {
        Eigen::VectorXi pairs = cnot_pairs.col(i);
        auto c = pairs(0);
        auto t = pairs(1);
        cnot_back << "CNOT(q[" << c << "], q[" << t << "]);\n";
      }

      xasm_src = xasm_src + "\n" + basis_front.str() + cnot_front.str();

      xasm_src = xasm_src + "Rz(q[" + std::to_string(qidxs[qidxs.size() - 1]) +
                 "], " + std::to_string(std::real(spinInst.coeff())) + " * " +
                 variable_name + ");\n";

      xasm_src = xasm_src + cnot_back.str() + basis_back.str();
    }

    int name_counter = 1;
    std::string name = "exp_tmp";
    while (xacc::hasCompiled(name)) {
      name += std::to_string(name_counter);
    }

    xasm_src = "__qpu__ void " + name + "(qbit q, double " +
               variable_name + ") {\n" + xasm_src + "}";

    // std::cout << xasm_src << "\n";
    auto xasm = xacc::getCompiler("xasm");
    auto tmp = xasm->compile(xasm_src)->getComposites()[0];

    for (auto inst : tmp->getInstructions()) {
      addInstruction(inst);
    }

    // store the Rz expressions
    for (auto &i : instructions) {
      if (i->name() == "Rz") {
        rz_expressions.push_back(i->getParameter(0).toString());
      }
    }

    parsingUtil = xacc::getService<ExpressionParsingUtil>("exprtk");
  }

  int counter = 0;
  for (auto &i : instructions) {
    if (i->name() == "Rz") {
      double x_val_ref = 0.0;
      parsingUtil->evaluate(rz_expressions[counter], {variable_name}, {x_val},
                            x_val_ref);
      i->setParameter(0, x_val_ref);
      counter++;
    }
  }
}

} // namespace circuits
} // namespace xacc