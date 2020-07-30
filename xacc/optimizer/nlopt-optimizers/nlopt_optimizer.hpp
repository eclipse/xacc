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
#ifndef XACC_NLOPT_OPTIMIZER_HPP_
#define XACC_NLOPT_OPTIMIZER_HPP_

#include <type_traits>
#include <utility>

#include "Optimizer.hpp"

namespace xacc {

struct ExtraNLOptData {
    std::function<double(const std::vector<double>&, std::vector<double>&)> f;
};

class NLOptimizer : public Optimizer {
public:
  OptResult optimize(OptFunction &function) override;
  const bool isGradientBased() const override;
  virtual const std::string get_algorithm() const;

  const std::string name() const override { return "nlopt"; }
  const std::string description() const override { return ""; }
};
} // namespace xacc
#endif
