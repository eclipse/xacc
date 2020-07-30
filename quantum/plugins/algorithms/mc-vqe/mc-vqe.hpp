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
#include <chrono>
#include "AlgorithmGradientStrategy.hpp"

namespace xacc {
namespace algorithm {
class MC_VQE : public Algorithm {
protected:
  Optimizer *optimizer;
  Accelerator *accelerator;
  HeterogeneousMap parameters;
  std::shared_ptr<AlgorithmGradientStrategy> gradientStrategy;

  // MC-VQE stuff
  int nChromophores;             // number of chromophores
  bool isCyclic;                 // true if the molecular system is cyclic
  Eigen::MatrixXd CISGateAngles; // state preparation angles
  std::shared_ptr<Observable> observable; // AIEM Hamiltonian
  int nStates;                    // # number of CIS states = nChromophores + 1
  const int NPARAMSENTANGLER = 4; // # of parameters in a single entangler
  const double ANGSTROM2BOHR = 1.8897161646320724; // angstrom to bohr
  const double DEBYE2AU = 0.393430307;             // D to a.u.
  std::string dataPath; // path to file with quantum chemistry data
  std::chrono::system_clock::time_point start; // start time of simulation

  // constructs CIS state preparation circiuit
  std::shared_ptr<CompositeInstruction>
  statePreparationCircuit(const Eigen::VectorXd &stateAngles) const;

  // constructs entangler portion of MC-VQE circuit
  std::shared_ptr<CompositeInstruction> entanglerCircuit() const;

  // processes quantum chemistry data and returns the Hamiltonian
  // and the gates for state preparation
  void preProcessing();

  // controls the level of printing
  int logLevel = 1;
  bool tnqvmLog = false;
  void logControl(const std::string message, const int level) const;

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