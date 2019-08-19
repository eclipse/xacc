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
#ifndef QUANTUM_GATE_IR_ALGORITHMS_INVERSEQFT_HPP_
#define QUANTUM_GATE_IR_ALGORITHMS_INVERSEQFT_HPP_

#include "Circuit.hpp"

namespace xacc {
namespace circuits {

/**
 * InverseQFT is a realization of the AlgorithmGenerator
 * interface that produces an XACC IR representation
 * of the Inverse Quantum Fourier Transform.
 */
class InverseQFT : public xacc::quantum::Circuit {
public:
  InverseQFT() : Circuit("iqft") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  DEFINE_CLONE(InverseQFT);
};
} // namespace circuits
} // namespace xacc

#endif
