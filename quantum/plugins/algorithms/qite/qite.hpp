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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#pragma once

#include "Algorithm.hpp"

namespace xacc {
namespace algorithm {
class QITE : public Algorithm {
public:
  bool initialize(const HeterogeneousMap &parameters) override;
  const std::vector<std::string> requiredParameters() const override;

  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
  std::vector<double> execute(const std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<double> &parameters) override;
  const std::string name() const override { return "qite"; }
  const std::string description() const override { return ""; }
  DEFINE_ALGORITHM_CLONE(QITE)
protected:
  // Construct the Trotter propagate circuit up to current time step.
  std::shared_ptr<CompositeInstruction> constructPropagateCircuit() const;
  // Calculate the current energy, i.e.
  // the value of the observable at the current Trotter step.
  double calcCurrentEnergy(int in_nbQubits) const;

  // Calculate approximate A operator observable at the current Trotter step.
  // Params:
  // in_kernel: the kernel to evolve the system to this time step
  // in_hmTerm: the H term to be approximate by the A term 
  // i.e. emulate the imaginary time evolution of that H term.
  // Returns the norm (as a double) and the A operator (Pauli observable)
  std::pair<double, std::shared_ptr<Observable>> calcAOps(const std::shared_ptr<AcceleratorBuffer>& in_buffer, std::shared_ptr<CompositeInstruction> in_kernel, std::shared_ptr<Observable> in_hmTerm) const;

protected:
  // Number of Trotter steps
  int m_nbSteps;
  // dBeta, i.e. step size
  double m_dBeta;
  // Accelerator
  std::shared_ptr<Accelerator> m_accelerator;
  // Hamiltonian Observable, i.e. H = Sum(h_i)
  std::shared_ptr<Observable> m_observable;
  // Ansatz circuit (apply before Trotter steps)
  CompositeInstruction* m_ansatz;
  // List of A operators for each time step
  // which approximates the imaginary-time step 
  // of the Hamiltonian observable
  // i.e. exp(-iAt) -> exp(-Ht)
  mutable std::vector<std::shared_ptr<Observable>> m_approxOps;
  // Energy value achieved at a Trotter step
  mutable std::vector<double> m_energyAtStep;
  // If a pure analytical run is requested.
  bool m_analytical;
  // For analytical solver only: the initial state
  // For accelerator-based simulation, the Ansatz is used to
  // prepare the initial state.
  int m_initialState;
};

// QLanczos Algorithm: extends QITE and 
// typically provides better energy convergence.
class QLanczos : public QITE {
public:
  const std::string name() const override { return "QLanczos"; }
  const std::string description() const override { return ""; }
  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
  DEFINE_ALGORITHM_CLONE(QLanczos)
private:
  double calcQlanczosEnergy() const;
};
} // namespace algorithm
} // namespace xacc
