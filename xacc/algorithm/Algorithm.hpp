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
class Algorithm : public xacc::Identifiable, public xacc::Cloneable<Algorithm> {

public:
  //   bool initialize(const HeterogeneousMap &&parameters) {
  //     return initialize(parameters);
  //   }
  virtual bool initialize(const HeterogeneousMap &parameters) = 0;
  virtual const std::vector<std::string> requiredParameters() const = 0;

  virtual void
  execute(const std::shared_ptr<AcceleratorBuffer> buffer) const = 0;

  virtual std::vector<double>
  execute(const std::shared_ptr<AcceleratorBuffer> buffer,
          const std::vector<double> &parameters) {
    XACCLogger::instance()->error(
        "Algorithm::execute(buffer, vector<double>) not implemented for " +
        name());
    exit(0);
    return {};
  }

  virtual double calculate(const std::string &calculation_task,
                           const std::shared_ptr<AcceleratorBuffer> buffer,
                           const HeterogeneousMap &extra_data = {}) {
    return {};
  }

#define DEFINE_ALGORITHM_CLONE(CLASS)                                          \
  std::shared_ptr<Algorithm> clone() override {                                \
    return std::make_shared<CLASS>();                                          \
  }
};

} // namespace xacc

#endif