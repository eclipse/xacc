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
#ifndef XACC_ALGORITHM_OPERATOR_POOLS_HPP_
#define XACC_ALGORITHM_OPERATOR_POOLS_HPP_

#include "adapt_vqe.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "xacc_observable.hpp"
#include "FermionOperator.hpp"
#include "ObservableTransform.hpp"
#include <memory>

namespace xacc{
namespace algorithm{

class UCCSD : public OperatorPool {

public:

  UCCSD() = default;

  // check if pool exists
  bool isValidOperatorPool(const std::string &operatorPool) override {
    if(operatorPool == "UCCSD"){ 
      return true;
    }
    return false;
  }

  // generate the pool
  std::vector<std::shared_ptr<Observable>>
  generate(const int &nQubits, const int &nElectrons) override {

    auto _nOccupied = (int)std::ceil(nElectrons / 2.0);
    auto _nVirtual = nQubits / 2 - _nOccupied;
    auto _nOrbs = _nOccupied + _nVirtual;
    std::vector<std::shared_ptr<Observable>> pool;

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
        fermiOp -= FermionOperator({{ia, 1}, {aa, 0}}, 0.5);

        fermiOp += FermionOperator({{ab, 1}, {ib, 0}}, 0.5);
        fermiOp -= FermionOperator({{ib, 1}, {ab, 0}}, 0.5);

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
            fermiOp -= FermionOperator({{ja, 1}, {ba, 0}, {ia, 1}, {aa, 0}}, 2.0 / std::sqrt(24.0));

            fermiOp += FermionOperator({{ab, 1}, {ib, 0}, {bb, 1}, {jb, 0}}, 2.0 / std::sqrt(24.0));
            fermiOp -= FermionOperator({{jb, 1}, {bb, 0}, {ib, 1}, {ab, 0}}, 2.0 / std::sqrt(24.0));

            fermiOp += FermionOperator({{aa, 1}, {ia, 0}, {bb, 1}, {jb, 0}}, 1.0 / std::sqrt(24.0));
            fermiOp -= FermionOperator({{jb, 1}, {bb, 0}, {ia, 1}, {aa, 0}}, 1.0 / std::sqrt(24.0));

            fermiOp += FermionOperator({{ab, 1}, {ib, 0}, {ba, 1}, {ja, 0}}, 1.0 / std::sqrt(24.0));
            fermiOp -= FermionOperator({{ja, 1}, {ba, 0}, {ib, 1}, {ab, 0}}, 1.0 / std::sqrt(24.0));

            fermiOp += FermionOperator({{aa, 1}, {ib, 0}, {bb, 1}, {ja, 0}}, 1.0 / std::sqrt(24.0));
            fermiOp -= FermionOperator({{ja, 1}, {bb, 0}, {ib, 1}, {aa, 0}}, 1.0 / std::sqrt(24.0));

            fermiOp += FermionOperator({{ab, 1}, {ia, 0}, {ba, 1}, {jb, 0}}, 1.0 / std::sqrt(24.0));
            fermiOp -= FermionOperator({{jb, 1}, {ba, 0}, {ia, 1}, {ab, 0}}, 1.0 / std::sqrt(24.0));

            pool.push_back(std::dynamic_pointer_cast<Observable>(std::make_shared<FermionOperator>(fermiOp)));

            fermiOp = FermionOperator({{aa, 1}, {ia, 0}, {bb, 1}, {jb, 0}}, 1.0 / (2.0 * std::sqrt(2.0)));
            fermiOp -= FermionOperator({{jb, 1}, {bb, 0}, {ia, 1}, {aa, 0}}, 1.0 / (2.0 * std::sqrt(2.0)));

            fermiOp += FermionOperator({{ab, 1}, {ib, 0}, {ba, 1}, {ja, 0}}, 1.0 / (2.0 * std::sqrt(2.0)));
            fermiOp -= FermionOperator({{ja, 1}, {ba, 0}, {ib, 1}, {ab, 0}}, 1.0 / (2.0 * std::sqrt(2.0)));

            fermiOp += FermionOperator({{aa, 1}, {ib, 0}, {bb, 1}, {ja, 0}}, -1.0 / (2.0 * std::sqrt(2.0)));
            fermiOp -= FermionOperator({{ja, 1}, {bb, 0}, {ib, 1}, {aa, 0}}, -1.0 / (2.0 * std::sqrt(2.0)));

            fermiOp += FermionOperator({{ab, 1}, {ia, 0}, {ba, 1}, {jb, 0}}, -1.0 / (2.0 * std::sqrt(2.0)));
            fermiOp -= FermionOperator({{jb, 1}, {ba, 0}, {ia, 1}, {ab, 0}}, -1.0 / (2.0 * std::sqrt(2.0)));

            pool.push_back(std::dynamic_pointer_cast<Observable>(std::make_shared<FermionOperator>(fermiOp)));

          }
        }
      }
    }

    return pool;
  }

  const std::string name() const override { return "uccsd"; }
  const std::string description() const override { return ""; }
};


class QubitPool : public OperatorPool {

public:

  QubitPool() = default;

  // check if pool exists
  bool isValidOperatorPool(const std::string &operatorPool) override {
    if(operatorPool == "qubit-pool"){ 
      return true;
    }
    return false;
  }

  // generate the pool
  std::vector<std::shared_ptr<Observable>>
  generate(const int &nQubits, const int &nElectrons) override {

    // The qubit pool vanishes for strings with an even number of Pauli Y's
    // and can have at most 4 operators
    // We loop over the indices for qubits q0-q3 and
    // {X, Y, X} for each qubit and make sure that we have the appropriate 
    // number of Y's and only add unique operators, i.e., those which 
    // survive symmetry operations in the PauliOperator class.

    std::vector<std::shared_ptr<Observable>> pool;
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

  const std::string name() const override { return "qubit-pool"; }
  const std::string description() const override { return ""; }
};

} // namespace algorithm
} // namespace xacc

#endif