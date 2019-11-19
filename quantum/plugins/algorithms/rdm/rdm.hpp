/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_ALGORITHM_RDM_HPP_
#define XACC_ALGORITHM_RDM_HPP_

#include "Algorithm.hpp"

namespace xacc {
namespace algorithm {
class RDM : public Algorithm {
protected:
  std::shared_ptr<CompositeInstruction> ansatz;
  std::shared_ptr<Accelerator> accelerator;

  HeterogeneousMap parameters;

public:
  bool initialize(const HeterogeneousMap &parameters) override;
  const std::vector<std::string> requiredParameters() const override;

  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
  const std::string name() const override { return "rdm"; }
  const std::string description() const override { return ""; }
  DEFINE_ALGORITHM_CLONE(RDM)
};
} // namespace algorithm
} // namespace xacc
#endif