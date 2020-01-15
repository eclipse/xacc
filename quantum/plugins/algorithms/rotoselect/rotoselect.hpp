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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#pragma once

#include "Algorithm.hpp"

namespace xacc {
namespace algorithm {
class Rotoselect : public Algorithm {
public:
  bool initialize(const HeterogeneousMap& in_parameters) override;
  const std::vector<std::string> requiredParameters() const override;

  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
  
  const std::string name() const override { return "rotoselect"; }
  const std::string description() const override { return ""; }
  DEFINE_ALGORITHM_CLONE(Rotoselect)

private:
  HeterogeneousMap m_parameters;
};
} // namespace algorithm
} // namespace xacc
