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
//
// This implements the Quantum Connected Moment Expansion (QCMX) algorithm
// This implementation follows arXiv:2009.05709v2
//
#ifndef XACC_ALGORITHM_QCMX_HPP_
#define XACC_ALGORITHM_QCMX_HPP_

#include "Algorithm.hpp"
#include "Observable.hpp"

namespace xacc {
namespace algorithm {

class QCMX : public Algorithm {

protected:
  std::shared_ptr<Observable> observable;
  Accelerator *accelerator;
  CompositeInstruction *kernel;
  HeterogeneousMap _parameters;

  // CMX order, also K in the paper
  int maxOrder;
  // threshold below which we ignore measurement
  double threshold = 0.0;
  // x is the vector of parameters if the provided ansatz is parameterized
  std::vector<double> x;
  // spectrum is only for PDS
  mutable std::vector<double> spectrum;

  // Compute energy from PDS CMX
  double PDS(const std::vector<double> &moments, const int order) const;

  // Compute energy from Cioslowski's original CMX
  double Cioslowski(const std::vector<double> &moments, const int order) const;

  // Compute energy from Knowles' generalized Pade approximants CMX
  double Knowles(const std::vector<double> &moments, const int order) const;

  // Compute energy from Soldatov's approximate functional
  double Soldatov(const std::vector<double> &moments) const;

  // get unique Pauli words
  std::shared_ptr<Observable>
  getUniqueTerms(const std::vector<std::shared_ptr<Observable>>) const;

  // get moments from measured unique Pauli words
  std::vector<double>
  getMoments(const std::vector<std::shared_ptr<Observable>>,
             const std::vector<std::shared_ptr<AcceleratorBuffer>>) const;

public:
  bool initialize(const HeterogeneousMap &parameters) override;
  const std::vector<std::string> requiredParameters() const override;
  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
  const std::string name() const override { return "qcmx"; }
  const std::string description() const override { return ""; }

  DEFINE_ALGORITHM_CLONE(QCMX)
};

} // namespace algorithm
} // namespace xacc

#endif