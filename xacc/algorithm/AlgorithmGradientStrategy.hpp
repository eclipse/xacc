/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Daniel Claudino - initial API and implementation
 *******************************************************************************/

#ifndef XACC_ALGORITHM_GRADIENT_STRATEGY_HPP_
#define XACC_ALGORITHM_GRADIENT_STRATEGY_HPP_

#include "Identifiable.hpp"
#include "heterogeneous.hpp"
namespace xacc {
// Forward declare:
class CompositeInstruction;
class AcceleratorBuffer;
class AlgorithmGradientStrategy : public Identifiable {

protected:

  std::vector<int> nInstructionsElement; // # of instructions for each element in gradient vector
  std::vector<double> coefficients; // coefficient that multiplies Pauli term

public:

  // Pass parameters to a specific gradient implementation
  virtual bool optionalParameters(const HeterogeneousMap parameters) = 0;

  // Generate circuits to enable gradient computation
  virtual std::vector<std::shared_ptr<CompositeInstruction>>
  getGradientExecutions(std::shared_ptr<CompositeInstruction> circuit, const std::vector<double> &x) = 0;

  // Compute the gradient vector given a vector of AcceleratorBuffers with executed circuits
  virtual void compute(std::vector<double> &dx, std::vector<std::shared_ptr<AcceleratorBuffer>> results) = 0;

};

} 
#endif