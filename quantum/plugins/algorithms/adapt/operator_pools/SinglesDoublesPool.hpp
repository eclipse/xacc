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
#ifndef XACC_SINGLES_DOUBLES_POOL_HPP_
#define XACC_SINGLES_DOUBLES_POOL_HPP_

#include "adapt.hpp"
#include "OperatorPool.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "xacc_observable.hpp"
#include "FermionOperator.hpp"
#include "Circuit.hpp"
#include "ObservableTransform.hpp"
#include <memory>
#include <string>

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace quantum {

class SinglesDoublesPool : public OperatorPool {

protected:
  int _nElectrons;
  std::vector<std::shared_ptr<Observable>> pool, operators;
  std::string excitationLevel = "singles-doubles";

public:
  SinglesDoublesPool() = default;

  bool isNumberOfParticlesRequired() const override { return true; };

  bool optionalParameters(const HeterogeneousMap parameters) override {

    if (!parameters.keyExists<int>("n-electrons")) {
      xacc::info("UCCSD pool needs number of electrons.");
      return false;
    }

    if (parameters.stringExists("excitation-level")) {
      excitationLevel = parameters.getString("excitation-level");
    }

    _nElectrons = parameters.get<int>("n-electrons");

    return true;
  }

  // generate the pool
  std::vector<std::shared_ptr<Observable>>
  getExcitationOperators(const int &nQubits) override {

    auto _nOccupied = (int)std::ceil(_nElectrons / 2.0);
    auto _nVirtual = nQubits / 2 - _nOccupied;
    auto _nOrbs = _nOccupied + _nVirtual;

    std::shared_ptr<Observable> fermiOp;
    // singles
    if (excitationLevel == "singles" || excitationLevel == "singles-doubles") {
      for (int i = 0; i < _nOccupied; i++) {
        int ia = i;
        int ib = i + _nOrbs;
        for (int a = 0; a < _nVirtual; a++) {
          int aa = a + _nOccupied;
          int ab = a + _nOccupied + _nOrbs;

          operators.push_back(std::make_shared<FermionOperator>(
              FermionOperator({{aa, 1}, {ia, 0}}, 4.0)));

          operators.push_back(std::make_shared<FermionOperator>(
              FermionOperator({{ab, 1}, {ib, 0}}, 4.0)));
        }
      }
    }

    // double excitations
    if (excitationLevel == "doubles" || excitationLevel == "singles-doubles") {
      for (int i = 0; i < _nOccupied; i++) {
        int ia = i;
        for (int j = i; j < _nOccupied; j++) {
          int jb = j + _nOrbs;
          for (int a = 0; a < _nVirtual; a++) {
            int aa = a + _nOccupied;
            for (int b = a; b < _nVirtual; b++) {
              int bb = b + _nOccupied + _nOrbs;

              operators.push_back(std::make_shared<FermionOperator>(
                  FermionOperator({{aa, 1}, {ia, 0}, {bb, 1}, {jb, 0}}, 16.0)));
            }
          }
        }
      }
    }

    return operators;
  }

  // generate the pool
  std::vector<std::shared_ptr<Observable>>
  generate(const int &nQubits) override {

    auto ops = getExcitationOperators(nQubits);
    auto jw = xacc::getService<ObservableTransform>("jw");
    for (auto &op : ops) {
      auto tmp = *std::dynamic_pointer_cast<FermionOperator>(op);
      tmp -= tmp.hermitianConjugate();
      tmp.normalize();
      pool.push_back(jw->transform(std::make_shared<FermionOperator>(tmp)));
    }
    return pool;
  }

  double getNormalizationConstant(const int index) const override {

    if (operators.empty()) {
      xacc::error("You need to call generate() first.");
    }
    auto tmp = *std::dynamic_pointer_cast<FermionOperator>(operators[index]);
    tmp -= tmp.hermitianConjugate();
    return 1.0 / tmp.operatorNorm();
  }

  std::string operatorString(const int index) override {

    return operators[index]->toString();
  }

  std::shared_ptr<CompositeInstruction>
  getOperatorInstructions(const int opIdx, const int varIdx) const override {

    // Instruction service for the operator to be added to the ansatz
    auto gate = std::dynamic_pointer_cast<quantum::Circuit>(
        xacc::getService<Instruction>("exp_i_theta"));

    // Create instruction for new operator
    gate->expand({std::make_pair("pauli", pool[opIdx]->toString()),
                  std::make_pair("param_id", "x" + std::to_string(varIdx))});

    return gate;
  }

  const std::string name() const override { return "singles-doubles-pool"; }
  const std::string description() const override { return ""; }
};

} // namespace quantum
} // namespace xacc

#endif