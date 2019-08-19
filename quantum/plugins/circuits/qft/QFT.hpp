/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#ifndef QUANTUM_GATE_IR_ALGORITHMS_QFT_HPP_
#define QUANTUM_GATE_IR_ALGORITHMS_QFT_HPP_

#include "Circuit.hpp"
#include "IRProvider.hpp"

namespace xacc {
namespace circuits {

class QFT : public xacc::quantum::Circuit {
public:
  QFT() : Circuit("qft") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  DEFINE_CLONE(QFT);
};
} // namespace circuits
} // namespace xacc

#endif
