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
// This implements the Connected Moment Expansion (CMX) algorithm
// This implementation follows arXiv:2009.05709v2
//
#ifndef XACC_ALGORITHM_CMX_HPP_
#define XACC_ALGORITHM_CMX_HPP_

#include "Algorithm.hpp"
#include "Observable.hpp"

namespace xacc {
namespace algorithm {

class CMX : public Algorithm {

protected:
  std::shared_ptr<Observable> observable;
  Accelerator *accelerator;
  CompositeInstruction *kernel;
  HeterogeneousMap _parameters;

  // CMX order, also K in the paper 
  int order;
  // String to identify the CMX instance
  std::string expansion;
  // Currently implemented expansions
  std::vector<std::string> expansions{"PDS", "Cioslowski", "Knowles"};

  // Wrapper to compute <H^n> using VQE::execute()
  double expValue(const std::shared_ptr<Observable> observable,
                    const std::shared_ptr<AcceleratorBuffer> buffer) const;

  // Compute energy from PDS CMX
  double PDS(const std::vector<double> moments) const;

  // Compute energy from Cioslowski's original CMX
  double Cioslowski(const std::vector<double> moments) const;

  // Compute energy from Knowles' generalized Pade approximants CMX
  double Knowles(const std::vector<double> moments) const;

public:
  bool initialize(const HeterogeneousMap &parameters) override;
  const std::vector<std::string> requiredParameters() const override;
  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
  const std::string name() const override { return "cmx"; }
  const std::string description() const override { return ""; }

  DEFINE_ALGORITHM_CLONE(CMX)
};

} // namespace algorithm
} // namespace xacc

#endif