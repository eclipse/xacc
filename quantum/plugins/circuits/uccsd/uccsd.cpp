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
#include "uccsd.hpp"
#include "PauliOperatorParser.h"
#include "xacc_service.hpp"
#include "FermionOperator.hpp"
#include "PauliOperator.hpp"
#include "OperatorPool.hpp"

#include "xacc.hpp"
#include "ObservableTransform.hpp"
#include <memory>
#include <vector>

using namespace xacc::quantum;

namespace xacc {
namespace circuits {

const std::vector<std::string> UCCSD::requiredKeys() { return {"nq", "ne"}; }

bool UCCSD::expand(const xacc::HeterogeneousMap &runtimeOptions) {

  if (!runtimeOptions.keyExists<int>("nq")) {
    return true;
  }

  if (!runtimeOptions.keyExists<int>("ne")) {
    return false;
  }

  int nQubits = runtimeOptions.get<int>("nq");
  int nElectrons = runtimeOptions.get<int>("ne");

  // Compute the number of parameters
  auto _nOccupied = (int)std::ceil(nElectrons / 2.0);
  auto _nVirtual = nQubits / 2 - _nOccupied;
  auto _nOrbitals = _nOccupied + _nVirtual;

  std::map<std::string, Term> terms;
  std::vector<xacc::InstructionParameter> variables;

  if (runtimeOptions.stringExists("pool")) {

    auto pool = xacc::getService<OperatorPool>(runtimeOptions.getString("pool"));
    pool->optionalParameters({std::make_pair("n-electrons", nElectrons)});
    auto operators = pool->generate(nQubits);
    PauliOperator sum, refOp;

    int i = 0;
    // loop over pointers to operators
    for(auto& op : operators){

      // get reference to the pointer 
      refOp = *std::dynamic_pointer_cast<PauliOperator>(op);
      for (auto term : refOp.getTerms()){

        //construct operator with parameter label
        PauliOperator paramOp(term.second.ops(), term.second.coeff(), "theta" + std::to_string(i));
        sum += paramOp;
 
      }

      // add variable only if the operator is not zero
      if(!refOp.getTerms().empty()){
        addVariable("theta" + std::to_string(i));
        i++;
      }

    }

    terms = sum.getTerms();

  } else { // create UCCSD as it used to

    auto nSingle = _nOccupied * _nVirtual;
    auto nDouble = nSingle * (nSingle + 1) / 2;
    auto _nParameters = nSingle + nDouble;

    std::vector<std::string> params;

    if (variables.empty()) {
      for (int i = 0; i < _nParameters; i++) {
        params.push_back("theta" + std::to_string(i));
        variables.push_back(InstructionParameter("theta" + std::to_string(i)));
        addVariable("theta" + std::to_string(i));
      }
    } else {
      for (int i = 0; i < _nParameters; i++) {
        params.push_back(variables[i].as<std::string>());
      }
    }

    auto slice = [](const std::vector<std::string> &v, int start = 0,
                    int end = -1) {
      int oldlen = v.size();
      int newlen;
      if (end == -1 or end >= oldlen) {
        newlen = oldlen - start;
      } else {
        newlen = end - start;
      }
      std::vector<std::string> nv(newlen);
      for (int i = 0; i < newlen; i++) {
        nv[i] = v[start + i];
      }
      return nv;
    };
    auto singleParams = slice(params, 0, nSingle);
    auto doubleParams1 = slice(params, nSingle, 2 * nSingle);
    auto doubleParams2 = slice(params, 2 * nSingle);
    std::vector<std::function<int(int, int)>> fs{[](int i, int n) { return i; },
                                            [](int i, int n) { return i + n; }};

    using OpType = std::vector<std::pair<int, bool>>;
    int count = 0;
    std::stringstream fOpSS;
    FermionOperator myOp;
    for (int i = 0; i < _nVirtual; i++) {
      for (int j = 0; j < _nOccupied; j++) {
        auto vs = _nOccupied + i;
        auto os = j;
        for (int s = 0; s < 2; s++) {
          auto ti = fs[s];
          auto oi = fs[1 - s];
          auto vt = ti(vs, _nOrbitals);
          auto vo = oi(vs, _nOrbitals);
          auto ot = ti(os, _nOrbitals);
          auto oo = oi(os, _nOrbitals);

          OpType op1{{vt, 1}, {ot, 0}}, op2{{ot, 1}, {vt, 0}};
          FermionOperator op(op1, 1.0, singleParams[count]);
          FermionOperator opp(op2, -1., singleParams[count]);

          OpType op3{{vt, 1}, {ot, 0}, {vo, 1}, {oo, 0}},
              op4{{oo, 1}, {vo, 0}, {ot, 1}, {vt, 0}};
          FermionOperator oppp(op3, -1., doubleParams1[count]);
          FermionOperator opppp(op4, 1., doubleParams1[count]);

          myOp += op + opp + oppp + opppp;
        }
        count++;
      }
    }

    count = 0;
    // routine for converting amplitudes for use by UCCSD
    std::vector<std::tuple<int, int>> tupleVec;
    for (int i = 0; i < _nVirtual; i++) {
      for (int j = 0; j < _nOccupied; j++) {
        tupleVec.push_back(std::make_tuple(i, j));
      }
    }
    // Combination lambda used to determine indices
    auto Combination = [=](std::vector<std::tuple<int, int>> t) {
      std::vector<std::tuple<int, int, int, int>> comboVec;
      for (int i = 0; i < t.size(); i++) {
        for (int j = i + 1; j < t.size(); j++) {
          const std::tuple<int, int, int, int> newTuple =
              std::tuple_cat(t[i], t[j]);
          comboVec.push_back(newTuple);
        }
      }
      return comboVec;
    };

    auto combineVec = Combination(tupleVec);
    for (auto i : combineVec) {
      auto p = std::get<0>(i);
      auto q = std::get<1>(i);
      auto r = std::get<2>(i);
      auto s = std::get<3>(i);

      auto vs1 = _nOccupied + p;
      auto os1 = q;
      auto vs2 = _nOccupied + r;
      auto os2 = s;

      for (int sa = 0; sa < 2; sa++) {
        for (int sb = 0; sb < 2; sb++) {
          auto ia = fs[sa];
          auto ib = fs[sb];

          auto v1a = ia(vs1, _nOrbitals);
          auto o1a = ia(os1, _nOrbitals);
          auto v2b = ib(vs2, _nOrbitals);
          auto o2b = ib(os2, _nOrbitals);

          OpType op5{{v1a, 1}, {o1a, 0}, {v2b, 1}, {o2b, 0}},
              op6{{o2b, 1}, {o1a, 0}, {v2b, 1}, {o2b, 0}};
          FermionOperator op(op5, -1., doubleParams2[count]);
          FermionOperator op2(op6, 1., doubleParams2[count]);
          myOp += op + op2;
        }
      }
      count++;
    }

    auto jw = xacc::getService<ObservableTransform>("jw");

    auto compositeResult =
        jw->transform(std::shared_ptr<Observable>(&myOp, [](Observable *) {}));

    terms = std::dynamic_pointer_cast<PauliOperator>(compositeResult)->getTerms();

  }

  auto pi = xacc::constants::pi;
int co = 0;
  auto gateRegistry = xacc::getIRProvider("quantum");
  for (auto inst : terms) {

    // for (auto inst : s) {
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
    auto tempFunction = gateRegistry->createComposite("temp", {spinInst.var()});

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

        // this is necessary for the qubit pool
        // which has single Y terms
        if(tempFunction->nInstructions() == 0){
          tempFunction->addInstruction(rx);
        } else {
          tempFunction->insertInstruction(0, rx);
        }

      }

      // Add the Rotation for the last term
      if (i == terms.size() - 1) {
        // FIXME DONT FORGET DIVIDE BY 2
        std::stringstream ss;
        ss << 2 * std::imag(std::get<0>(spinInst)) << " * "
           << std::get<1>(spinInst);
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

        // this is necessary for the qubit pool 
        if(counter == tempFunction->nInstructions()){
          tempFunction->addInstruction(cnot);
        } else {
          tempFunction->insertInstruction(counter, cnot);
        }
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

  for (int i = (nElectrons / 2) - 1; i >= 0; i--) {
    std::size_t alpha = (std::size_t)i;
    auto xGate =
        gateRegistry->createInstruction("X", std::vector<std::size_t>{alpha});
    insertInstruction(0, xGate);
    std::size_t beta = (std::size_t)(i + _nOrbitals);
    xGate =
        gateRegistry->createInstruction("X", std::vector<std::size_t>{beta});
    insertInstruction(0, xGate);
  }
  return true;
}

} // namespace circuits
} // namespace xacc
