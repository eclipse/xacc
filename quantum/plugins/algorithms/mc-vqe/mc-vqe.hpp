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
 *   Daniel Claudino - initial API and implementation
 *******************************************************************************/
#ifndef XACC_ALGORITHM_MC_VQE_HPP_
#define XACC_ALGORITHM_MC_VQE_HPP_

#include "Algorithm.hpp"
#include "xacc_service.hpp"
#include <Eigen/Dense>

namespace xacc {
namespace algorithm {
class MC_VQE : public Algorithm {
protected:
  Optimizer * optimizer;
  Accelerator * accelerator;

  HeterogeneousMap parameters;

  //MC-VQE stuff
  int nChromophores; // number of chromophores
  bool isCyclic; // true if the molecular system is cyclic
  Eigen::MatrixXd CISGateAngles;// state preparation angles
  std::shared_ptr<Observable> observable; // AIEM Hamiltonian

  std::shared_ptr<CompositeInstruction>
  circuit(const Eigen::VectorXd &stateAngles) const; // circuit for a given initial state

  // process quantum chemistry data and returns the Hamiltonian 
  // and the gates for state preparation
  void preProcessing();


public:
  bool initialize(const HeterogeneousMap &parameters) override;
  const std::vector<std::string> requiredParameters() const override;

  void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const override;
  std::vector<double> execute(const std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<double> &parameters) override;
  const std::string name() const override { return "mc-vqe"; }
  const std::string description() const override { return ""; }
  DEFINE_ALGORITHM_CLONE(MC_VQE)
};
} // namespace algorithm
} // namespace xacc
#endif