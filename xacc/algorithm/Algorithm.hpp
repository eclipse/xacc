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
#ifndef XACC_ALGORITHM_HPP_
#define XACC_ALGORITHM_HPP_

#include "Identifiable.hpp"
#include "heterogeneous.hpp"
#include "Observable.hpp"
#include "Accelerator.hpp"
#include "Optimizer.hpp"

#include <memory>

namespace xacc {

// The Algorithm provides a general extension point for
// algorithms that leverage Accelerator co-processors.
//
// Algorithms must be initialized with a HeterogeneousMap of
// input parameters.
//
// Algorithms expose an execute method that persists results
// to the AcceleratorBuffer.
class Algorithm : public xacc::Identifiable {

public:
  bool initialize(const HeterogeneousMap &&parameters) { return initialize(parameters); }
  virtual bool initialize(const HeterogeneousMap &parameters) = 0;
  virtual const std::vector<std::string> requiredParameters() const = 0;

  virtual void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const = 0;
};

} // namespace xacc

#endif