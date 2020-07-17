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
#ifndef XACC_CUSTOM_POOL_HPP_
#define XACC_CUSTOM_POOL_HPP_

#include "OperatorPool.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "xacc_observable.hpp"
#include "FermionOperator.hpp"
#include "Circuit.hpp"
#include "ObservableTransform.hpp"
#include <memory>

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace quantum {

// The idea of this pool is to expose the OperatorPool 
// so any collection of operators can be passed
// to an algorithm

class CustomPool : public OperatorPool {

protected:

  std::vector<std::shared_ptr<Observable>> pool;

public:

  CustomPool() = default;

  // pass a single FermionOperator, likely a sum of operators
  void setOperators(const FermionOperator op) override {
    pool = {std::dynamic_pointer_cast<Observable>(std::make_shared<FermionOperator>(op))};
    return;
  }

  // pass a single PauliOperator, likely a sum of operators
  void setOperators(const PauliOperator op) override {
    pool = {std::dynamic_pointer_cast<Observable>(std::make_shared<PauliOperator>(op))};
    return;
  }

  // pass a sum of Pauli/FermionOperators as shared_ptr<Observable>
  // from getObservable
  void setOperators(const std::shared_ptr<Observable> op) override {

    std::shared_ptr<Observable> obs;
    if (op->toString().find("^") != std::string::npos) {

      auto jw = xacc::getService<ObservableTransform>("jw");
      if (std::dynamic_pointer_cast<FermionOperator>(op)) {
        obs = jw->transform(op);
      } else {
        auto fermionOp = xacc::quantum::getObservable("fermion", op->toString());
        obs = jw->transform(std::dynamic_pointer_cast<Observable>(fermionOp));      
      }

    } else if (op->toString().find("X") != std::string::npos
              || op->toString().find("Y") != std::string::npos
              || op->toString().find("Z") != std::string::npos
              && !std::dynamic_pointer_cast<PauliOperator>(op)){

      auto pauliOp = xacc::quantum::getObservable("pauli", op->toString());
      obs = std::dynamic_pointer_cast<Observable>(pauliOp);

    } else {

      pool = {op};

    }

    pool = {obs};
    return;

  }

  // pass a vector of FermionOperator, likely a sum of operators
  void setOperators(const std::vector<FermionOperator> ops) override {
    for (auto op : ops) {
      pool.push_back(std::dynamic_pointer_cast<Observable>((std::make_shared<FermionOperator>(op))));
    } 
  }

  // pass a vector of PauliOperator, likely a sum of operators
  void setOperators(const std::vector<PauliOperator> ops) override {
    for (auto op : ops) {
      pool.push_back(std::dynamic_pointer_cast<Observable>((std::make_shared<PauliOperator>(op))));
    } 
  } 
 
  bool optionalParameters(const HeterogeneousMap parameters) override {
    return true;
  }

  // generate the pool
  std::vector<std::shared_ptr<Observable>> generate(const int &nQubits) override {

    if(std::dynamic_pointer_cast<PauliOperator>(pool[0])){
      return pool;
    } else {
      std::vector<std::shared_ptr<Observable>> jwPool;
      auto jw = xacc::getService<ObservableTransform>("jw");

      for(auto op : pool){
        jwPool.push_back(jw->transform(op));
      }
      return jwPool;
    }
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
    std::shared_ptr<Observable> pauliOp;
    if (std::dynamic_pointer_cast<PauliOperator>(pool[opIdx])) {
      pauliOp = pool[opIdx];
    } else { 
      pauliOp = xacc::getService<ObservableTransform>("jw")->transform(pool[opIdx]);
    }
    gate->expand(
        {std::make_pair("pauli", pauliOp->toString()),
        std::make_pair("param_id", "x" + std::to_string(varIdx))
        });

    return gate;

  }

  const std::string name() const override { return "custom-pool"; }
  const std::string description() const override { return ""; }
};


} // namespace quantum
} // namespace xacc

#endif