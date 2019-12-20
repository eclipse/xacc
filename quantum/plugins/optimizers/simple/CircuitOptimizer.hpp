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
#ifndef QUANTUM_GATE_COMPILER_CIRCUIT_OPTIMIZER_HPP_
#define QUANTUM_GATE_COMPILER_CIRCUIT_OPTIMIZER_HPP_

#include "IRTransformation.hpp"
#include "InstructionIterator.hpp"
#include "OptionsProvider.hpp"

namespace xacc {
namespace quantum {

class CircuitOptimizer : public IRTransformation, public OptionsProvider {

public:
  CircuitOptimizer() {}
  void apply(std::shared_ptr<CompositeInstruction> program,
                     const std::shared_ptr<Accelerator> accelerator,
                     const HeterogeneousMap& options = {}) override;
  const IRTransformationType type() const override {return IRTransformationType::Optimization;}

  const std::string name() const override { return "circuit-optimizer"; }
  const std::string description() const override { return ""; }

private:
  // Remove adjacent NOT (X) gates:
  // If it encounters a X gate, it will push that gate to the right 
  // to find a matching X gate hence can cancel both of them.
  // We can push through the X gate if the qubit wire is the *target* of a CNOT gate.
  // e.g. X(q[1]); CX(q[0], q[1]); <=> CX(q[0], q[1]); X(q[1]);
  // If no cancellation can be realized, the move is abandoned, i.e. no change to the original circuit.
  // - io_program: the composite instruction that we will optimize.
  // - Return: true if it can cancel at least one X gate; false otherwise. 
  bool tryPermuteAndCancelXGate(std::shared_ptr<CompositeInstruction>& io_program);
  
  // Reduce the Hadamard gates in the circuit as much as possiple.
  // Rationale: Hadamard gates tend to block circuit optimization, i.e. they act as barriers.
  // Hence, reducing their count (even if the total gate count is the same) could help downstream optimization.
  // There are 5 circuit patterns of this reduction, please refer to Figure 4 of https://arxiv.org/pdf/1710.07345.pdf for details.
  // Notes: The phase gate is equivalent to Rz(pi/2).
  bool tryReduceHadamardGates(std::shared_ptr<CompositeInstruction>& io_program);
  
  // Identify and merge Rz rotation gates using the *phase polynomials* representation.
  // This implements routine #4 in https://arxiv.org/pdf/1710.07345.pdf
  bool tryRotationMergingUsingPhasePolynomials(std::shared_ptr<CompositeInstruction>& io_program);
};
} // namespace quantum
} // namespace xacc

#endif
