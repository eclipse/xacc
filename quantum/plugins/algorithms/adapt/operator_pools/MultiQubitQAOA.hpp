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
#ifndef XACC_MULTI_QUBIT_QAOA_POOL_HPP_
#define XACC_MULTI_QUBIT_QAOA_POOL_HPP_

#include "adapt.hpp"
#include "OperatorPool.hpp"
#include "variant.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "xacc_observable.hpp"
#include "Circuit.hpp"
#include <memory>
#include <string>

using namespace xacc;

namespace xacc{
// namespace algorithm{
namespace quantum{

class MultiQubitQAOA : public OperatorPool {

protected:

  std::vector<std::shared_ptr<Observable>> pool;
  std::shared_ptr<Observable> costHamiltonian;

public:

  MultiQubitQAOA() = default;

  // this is a required parameter for QAOA
  bool optionalParameters(const HeterogeneousMap parameters) override {

    if(!parameters.pointerLikeExists<Observable>("cost-hamiltonian")){
      return false;
    }

    costHamiltonian = parameters.get<std::shared_ptr<Observable>>("cost-hamiltonian");
    return true;
  }

  // generate the pool
  std::vector<std::shared_ptr<Observable>> generate(const int &nQubits) override {

    // Single-qubit pool in ADAPT-QAOA

    std::vector<PauliOperator> pauliOps;
    PauliOperator op, sumX, sumY;

    for (int q = 0; q < nQubits; q++){

      op.fromString("X" + std::to_string(q));
      sumX += op;
      pauliOps.push_back(op);

      op.fromString("Y" + std::to_string(q));
      sumY += op;
      pauliOps.push_back(op);

    }

    pauliOps.push_back(sumX);
    pauliOps.push_back(sumY);

    for (int q0 = 0; q0 < nQubits; q0++){

      for (auto op0 : {"X", "Y", "Z"}){

        for (int q1 = q0 + 1; q1 < nQubits; q1++){

          for (auto op1 : {"X", "Y", "Z"}){

            op.fromString(op0 + std::to_string(q0) + op1 + std::to_string(q1));
            pauliOps.push_back(op);

          }

        }

      }

    }

    for (auto op: pauliOps){
      pool.push_back(std::dynamic_pointer_cast<Observable>(std::make_shared<PauliOperator>(op)));
    }

    return pool;
  }

  std::string operatorString(const int index) override {

    return pool[index]->toString();

  }

  std::shared_ptr<CompositeInstruction> 
  getOperatorInstructions(const int opIdx, const int varIdx) const override {

    // Now do the same for the mixer
    auto mixerInstructions = std::dynamic_pointer_cast<quantum::Circuit>(
        xacc::getService<Instruction>("exp_i_theta"));

    // Create instruction for new operator
    mixerInstructions->expand(
        {std::make_pair("pauli", pool[opIdx]->toString()),
        std::make_pair("param_id", "x" + std::to_string(varIdx))
        });

    return mixerInstructions;

  }

  const std::string name() const override { return "multi-qubit-qaoa"; }
  const std::string description() const override { return ""; }
};

} // namespace algorithm
} // namespace xacc

#endif