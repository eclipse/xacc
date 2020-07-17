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
#ifndef XACC_SINGLET_ADAPTED_UCCSD_POOL_HPP_
#define XACC_SINGLET_ADAPTED_UCCSD_POOL_HPP_

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

namespace xacc{
// namespace algorithm{
namespace quantum{

class SingletAdaptedUCCSD : public OperatorPool {

protected:

  int _nElectrons;
  std::vector<std::shared_ptr<Observable>> pool;

public:

  SingletAdaptedUCCSD() = default;

  bool optionalParameters(const HeterogeneousMap parameters) override {

    if(!parameters.keyExists<int>("n-electrons")){
      xacc::info("SingletAdaptedUCCSD pool needs number of electrons.");
      return false;
    }

    _nElectrons = parameters.get<int>("n-electrons");

    return true;
  }

  // generate the pool
  std::vector<std::shared_ptr<Observable>> generate(const int &nQubits) override {

    auto _nOccupied = (int)std::ceil(_nElectrons / 2.0);
    auto _nVirtual = nQubits / 2 - _nOccupied;
    auto _nOrbs = _nOccupied + _nVirtual;

    // single excitations
    for (int i = 0; i < _nOccupied; i++){
      int ia = i;
      int ib = i + _nOrbs;
      for (int a = 0; a < _nVirtual; a++){
        int aa = a + _nOccupied;
        int ab = a + _nOccupied + _nOrbs;

        // spin-adapted singles
        FermionOperator fermiOp;
        fermiOp = FermionOperator({{aa, 1}, {ia, 0}}, 0.5);
        fermiOp += FermionOperator({{ab, 1}, {ib, 0}}, 0.5);
        fermiOp -= fermiOp.hermitianConjugate();
        pool.push_back(std::dynamic_pointer_cast<Observable>(std::make_shared<FermionOperator>(fermiOp)));

      }
    }

    // double excitations
    for (int i = 0; i < _nOccupied; i++){
      int ia = i;
      int ib = i + _nOrbs;
      for (int j = i; j < _nOccupied; j++){
        int ja = j;
        int jb = j + _nOrbs;
        for (int a = 0; a < _nVirtual; a++){
          int aa = a + _nOccupied;
          int ab = a + _nOccupied + _nOrbs;
          for ( int b = a; b < _nVirtual; b++){
            int ba = b + _nOccupied;
            int bb = b + _nOccupied + _nOrbs;

            FermionOperator fermiOp;

            fermiOp = FermionOperator({{aa, 1}, {ia, 0}, {ba, 1}, {ja, 0}}, 2.0 / std::sqrt(24.0));
            fermiOp += FermionOperator({{ab, 1}, {ib, 0}, {bb, 1}, {jb, 0}}, 2.0 / std::sqrt(24.0));
            fermiOp += FermionOperator({{aa, 1}, {ia, 0}, {bb, 1}, {jb, 0}}, 1.0 / std::sqrt(24.0));
            fermiOp += FermionOperator({{ab, 1}, {ib, 0}, {ba, 1}, {ja, 0}}, 1.0 / std::sqrt(24.0));
            fermiOp += FermionOperator({{aa, 1}, {ib, 0}, {bb, 1}, {ja, 0}}, 1.0 / std::sqrt(24.0));
            fermiOp += FermionOperator({{ab, 1}, {ia, 0}, {ba, 1}, {jb, 0}}, 1.0 / std::sqrt(24.0));
            fermiOp -= fermiOp.hermitianConjugate();
            pool.push_back(std::dynamic_pointer_cast<Observable>(std::make_shared<FermionOperator>(fermiOp)));

            fermiOp = FermionOperator({{aa, 1}, {ia, 0}, {bb, 1}, {jb, 0}}, 1.0 / (2.0 * std::sqrt(2.0)));
            fermiOp += FermionOperator({{ab, 1}, {ib, 0}, {ba, 1}, {ja, 0}}, 1.0 / (2.0 * std::sqrt(2.0)));
            fermiOp += FermionOperator({{aa, 1}, {ib, 0}, {bb, 1}, {ja, 0}}, -1.0 / (2.0 * std::sqrt(2.0)));
            fermiOp += FermionOperator({{ab, 1}, {ia, 0}, {ba, 1}, {jb, 0}}, -1.0 / (2.0 * std::sqrt(2.0)));
            fermiOp -= fermiOp.hermitianConjugate();
            pool.push_back(std::dynamic_pointer_cast<Observable>(std::make_shared<FermionOperator>(fermiOp)));

          }
        }
      }
    }

    std::vector<std::shared_ptr<Observable>> jwPool;
    auto jw = xacc::getService<ObservableTransform>("jw");

    for(auto op : pool){
      jwPool.push_back(jw->transform(op));
    }

    return jwPool;
  }

  std::string operatorString(const int index) override {

    return pool[index]->toString();

  }

  std::shared_ptr<CompositeInstruction> 
  getOperatorInstructions(const int opIdx, const int varIdx) const override {

    // Instruction service for the operator to be added to the ansatz
    auto gate = std::dynamic_pointer_cast<quantum::Circuit>(
        xacc::getService<Instruction>("exp_i_theta"));

    // Create instruction for new operator
    auto jwOp = xacc::getService<ObservableTransform>("jw")->transform(pool[opIdx]);
    gate->expand(
        {std::make_pair("pauli", jwOp->toString()),
        std::make_pair("param_id", "x" + std::to_string(varIdx))});

    return gate;

  }

  const std::string name() const override { return "singlet-adapted-uccsd"; }
  const std::string description() const override { return ""; }
};


} // namespace algorithm
} // namespace xacc

#endif