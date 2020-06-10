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
private:
  // Construct the Trotter propagate circuit up to current time step.
  std::shared_ptr<CompositeInstruction> constructPropagateCircuit() const;

private:
  // Number of Trotter steps
  int m_nbSteps;
  // dBeta, i.e. step size
  double m_dBeta;
  // Accelerator
  std::shared_ptr<Accelerator> m_accelerator;
  // Hamiltonian Observable, i.e. H = Sum(h_i)
  Observable* m_observable;
  // Ansatz circuit (apply before Trotter steps)
  CompositeInstruction* m_ansatz;
  // List of A operators for each time step
  // which approximates the imaginary-time step 
  // of the Hamiltonian observable
  // i.e. exp(-iAt) -> exp(-Ht)
  mutable std::vector<std::shared_ptr<Observable>> m_approxOps;
};
} // namespace algorithm
} // namespace xacc
