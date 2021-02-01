/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
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
#ifndef XACC_SPIN_FLIP_POOL_HPP_
#define XACC_SPIN_FLIP_POOL_HPP_

#include "OperatorPool.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "xacc_observable.hpp"
#include "FermionOperator.hpp"
#include "Circuit.hpp"
#include "ObservableTransform.hpp"

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace quantum {

class SpinFlip : public OperatorPool {

protected:
  int _nElectrons, _rank = 1;
  std::vector<std::shared_ptr<Observable>> pool, operators;

public:
  SpinFlip() = default;

  bool optionalParameters(const HeterogeneousMap parameters) override {

    if (!parameters.keyExists<int>("n-electrons")) {
      xacc::info("Pool needs number of electrons.");
      return false;
    }
    _nElectrons = parameters.get<int>("n-electrons");

    if (parameters.keyExists<int>("rank")) {
      _rank = parameters.get<int>("rank");
    }

    if (_rank > 3) {
      xacc::error("Cannot flip more than three spins");
    }

    return true;
  }

  // generate the pool
  std::vector<std::shared_ptr<Observable>>
  getExcitationOperators(const int &nQubits) override {

    auto _nOccupied = (int)std::ceil(_nElectrons / 2.0);
    auto _nVirtual = nQubits / 2 - _nOccupied;
    auto _nOrbs = _nOccupied + _nVirtual;

    if (_rank == 1) {
      for (int i = 0; i < _nOccupied; i++) {
        int ia = i;
        int ib = i + _nOrbs;
        operators.push_back(std::make_shared<FermionOperator>(
            FermionOperator({{ib, 1}, {ia, 0}}, 4.0)));

        operators.push_back(std::make_shared<FermionOperator>(
            FermionOperator({{ia, 1}, {ib, 0}}, 4.0)));
      }
    }

    if (_rank == 2) {

      for (int i = 0; i < _nOccupied; i++) {
        int ia = i;
        int ib = i + _nOrbs;
        for (int j = i + 1; j < _nOccupied; j++) {
          int ja = j;
          int jb = j + _nOrbs;

          operators.push_back(std::make_shared<FermionOperator>(
              FermionOperator({{ib, 1}, {ia, 0}, {jb, 1}, {ja, 0}}, 16.0)));

          operators.push_back(std::make_shared<FermionOperator>(
              FermionOperator({{ia, 1}, {ib, 0}, {ja, 1}, {jb, 0}}, 16.0)));
        }
      }
    }

    if (_rank == 3) {

      for (int i = 0; i < _nOccupied; i++) {
        int ia = i;
        int ib = i + _nOrbs;

        for (int j = i + 1; j < _nOccupied; j++) {
          int ja = j;
          int jb = j + _nOrbs;
          for (int k = j + 1; k < _nOccupied; k++) {
            int ka = k;
            int kb = k + _nOrbs;

            operators.push_back(
                std::make_shared<FermionOperator>(FermionOperator(
                    {{ib, 1}, {ia, 0}, {jb, 1}, {ja, 0}, {kb, 1}, {ka, 0}},
                    16.0)));
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

  const std::string name() const override { return "spin-flip"; }
  const std::string description() const override { return ""; }
};

} // namespace quantum
} // namespace xacc

#endif