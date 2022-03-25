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
#ifndef XACC_QUBIT_POOL_HPP_
#define XACC_QUBIT_POOL_HPP_

#include "adapt.hpp"
#include "OperatorPool.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "xacc_observable.hpp"
#include "Circuit.hpp"
#include <memory>

using namespace xacc;

namespace xacc{
// namespace algorithm{
namespace quantum{

class QubitPool : public OperatorPool {

protected:

  std::vector<std::shared_ptr<Observable>> pool;

public:

  QubitPool() = default;

  bool optionalParameters(const HeterogeneousMap parameters) override {
    return true;
  }

  // generate the pool
  std::vector<std::shared_ptr<Observable>> generate(const int &nQubits) override {

    // The qubit pool vanishes for strings with an even number of Pauli Y's
    // and can have at most 4 operators
    // We loop over the indices for qubits q0-q3 and
    // {X, Y, X} for each qubit and make sure that we have the appropriate 
    // number of Y's and only add unique operators, i.e., those which 
    // survive symmetry operations in the PauliOperator class.

    std::vector<PauliOperator> pauliOps;
    std::vector<std::string> label;
    std::string opStr;
    PauliOperator op;

    for (int q0 = 0; q0 < nQubits; q0++){

      for (auto op0 : {"X", "Y", "Z"}){

        label.push_back(op0);
        if (op0 == "Y"){
          opStr = "Y" + std::to_string(q0);
          op.fromString("(0,1)" + opStr);
          pauliOps.push_back(op);
        }

        for (int q1 = q0 + 1; q1 < nQubits; q1++){

          for (auto op1 : {"X", "Y", "Z"}){

            label.push_back(op1);
            if (std::count(label.begin(), label.end(), "Y") == 1){
              opStr = op0 + std::to_string(q0) + op1 + std::to_string(q1);
              op.fromString("(0,1)" + opStr);
              if(!(std::find(pauliOps.begin(), pauliOps.end(), op) != pauliOps.end())){
                pauliOps.push_back(op);
              }

            }

            for (int q2 = q1 + 1; q2 < nQubits; q2++){

              for (auto op2 : {"X", "Y", "Z"}){

                label.push_back(op2);
                if (std::count(label.begin(), label.end(), "Y") == 1 || std::count(label.begin(), label.end(), "Y") == 3){

                  opStr = op0 + std::to_string(q0) + op1 + std::to_string(q1) + op2 + std::to_string(q2);
                  op.fromString("(0,1)" + opStr);
                  if(!(std::find(pauliOps.begin(), pauliOps.end(), op) != pauliOps.end())){
                    pauliOps.push_back(op);
                  }
                }

                for (int q3 = q2 + 1; q3 < nQubits; q3++){

                  for (auto op3 : {"X", "Y", "Z"}){

                    label.push_back(op3);
                    if (std::count(label.begin(), label.end(), "Y") == 1 || std::count(label.begin(), label.end(), "Y") == 3){

                      opStr = op0 + std::to_string(q0) + op1 + std::to_string(q1) + op2 + std::to_string(q2) + op3 + std::to_string(q3);
                      op.fromString("(0,1)" + opStr);
                      if(!(std::find(pauliOps.begin(), pauliOps.end(), op) != pauliOps.end())){
                        pauliOps.push_back(op);
                      }

                    } 

                    label.pop_back();
                  }
                }
                label.pop_back();
              }
            }
            label.pop_back();
          }
        }
        label.pop_back();
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

  double getNormalizationConstant(const int index) const override {

    if (pool.empty()) {
      xacc::error("You need to call generate() first.");
    }
    auto tmp = *std::dynamic_pointer_cast<PauliOperator>(pool[index]);
    tmp -= tmp.hermitianConjugate();
    return 1.0 / tmp.operatorNorm();
  }

  std::shared_ptr<CompositeInstruction> 
  getOperatorInstructions(const int opIdx, const int varIdx) const override {

    // Instruction service for the operator to be added to the ansatz
    auto gate = std::dynamic_pointer_cast<quantum::Circuit>(
        xacc::getService<Instruction>("exp_i_theta"));

    // Create instruction for new operator
    gate->expand(
        {std::make_pair("pauli", pool[opIdx]->toString()),
        std::make_pair("param_id", "x" + std::to_string(varIdx))});

    return gate;

  }

  const std::string name() const override { return "qubit-pool"; }
  const std::string description() const override { return ""; }
};

} // namespace algorithm
} // namespace xacc

#endif