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

#include "IRGenerator.hpp"
#include "IRProvider.hpp"

namespace xacc {
namespace quantum {

/**
 * QFT is a realization of the AlgorithmGenerator
 * interface that produces an XACC IR representation
 * of the Quantum Fourier Transform.
 *
 * @author Alex McCaskey
 */
class QFT : public IRGenerator {
public:
  /**
   * Implementations of this method generate a Function IR
   * instance corresponding to the implementation's modeled
   * algorithm. The algorithm is specified to operate over the
   * provided AcceleratorBuffer and can take an optional
   * vector of InstructionParameters.
   *
   * @param bits The bits this algorithm operates on
   * @return function The algorithm represented as an IR Function
   */
  virtual std::shared_ptr<Function>
  generate(std::shared_ptr<AcceleratorBuffer> buffer,
           std::vector<InstructionParameter> parameters =
               std::vector<InstructionParameter>{});

  virtual const std::string name() const { return "QFT"; }

  virtual const std::string description() const {
    return "The QFT Algorithm Generator generates IR representing "
           "a Quantum Fourier Transform.";
  }

  /**
   * The destructor
   */
  virtual ~QFT() {}
};
} // namespace quantum
} // namespace xacc

#endif
