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
#ifndef XACC_ALGORITHM_ADAPT_HPP_
#define XACC_ALGORITHM_ADAPT_HPP_

#include "Algorithm.hpp"
#include "Observable.hpp"
#include "PauliOperator.hpp"
#include "OperatorPool.hpp"

using namespace xacc::quantum;

namespace xacc{
namespace algorithm{

/*
class OperatorPool : public Identifiable {

public:

  virtual bool optionalParameters(const HeterogeneousMap parameters) = 0;

  virtual std::vector<std::shared_ptr<Observable>>
  generate(const int &nQubits) = 0;

  virtual std::string operatorString(const int index) = 0;

  virtual std::shared_ptr<CompositeInstruction>
  getOperatorInstructions(const int opIdx, const int varIdx) const = 0;
};  
*/

class ADAPT : public Algorithm {

protected:
  std::shared_ptr<Observable> observable;
  std::shared_ptr<Optimizer> optimizer;
  std::shared_ptr<Accelerator> accelerator;
  std::shared_ptr<OperatorPool> pool; 
  std::shared_ptr<CompositeInstruction> initialState; 
  std::string subAlgo; // sub-algorithm, either VQE or QAOA
  HeterogeneousMap _parameters;

  //ADAPT parameters
  int _maxIter = 50; // max # of ADAPT cycles // # of QAOA layers
  double _adaptThreshold = 1.0e-2; // gradient norm threshold
  double _printThreshold = 1.0e-10; // threshold to print commutator
  bool _printOps = false; // set to true to print operators at every iteration
  int _nElectrons; // # of electrons, used for VQE

  std::vector<int> checkpointOps; // indices of operators to construct initial ansatz
  std::vector<double> checkpointParams; // initial parameters for initial ansatz
  // name of class to compute gradient for optimization
  // defaults to parameter shift
  std::string gradStrategyName = "parameter-shift-gradient"; 

public:

  bool initialize(const HeterogeneousMap &parameters) override;
  const std::vector<std::string> requiredParameters() const override;
  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
  const std::string name() const override { return "adapt"; }
  const std::string description() const override { return ""; }

  DEFINE_ALGORITHM_CLONE(ADAPT)

};


} // namespace algorithm
} // namespace xacc

#endif 