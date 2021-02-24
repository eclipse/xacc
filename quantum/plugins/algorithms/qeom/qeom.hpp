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
 ******************************************************************************/

// This implements the quantum Equation of Motion (qEOM) algorithm
// as in arXiv:1910.12890
//
// This algorithm works assumes an optimized ansatz, which is
// used to compute the matrix elements in the EOM equations.
// This becomes a generalized eigenvalue problem, from which we
// extract only the real and positive eigenvalues (positive excitation energies)

#ifndef XACC_ALGORITHM_QEOM_HPP_
#define XACC_ALGORITHM_QEOM_HPP_

#include "Algorithm.hpp"
#include "xacc.hpp"
#include <unordered_map>
#include <vector>

namespace xacc {
namespace algorithm {
class qEOM : public Algorithm {
protected:
  std::shared_ptr<Observable> observable;
  CompositeInstruction *kernel;
  Accelerator *accelerator;
  HeterogeneousMap parameters;
  std::vector<std::shared_ptr<Observable>> operators;
  mutable std::unordered_map<std::string, double> cachedMeasurements;

  double measureOperator(const std::shared_ptr<Observable> obs, const std::shared_ptr<AcceleratorBuffer> buffer) const;

public:
  bool initialize(const HeterogeneousMap &parameters) override;
  const std::vector<std::string> requiredParameters() const override;
  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;

  const std::string name() const override { return "qeom"; }
  const std::string description() const override { return ""; }
  DEFINE_ALGORITHM_CLONE(qEOM)
};
} // namespace algorithm
} // namespace xacc
#endif