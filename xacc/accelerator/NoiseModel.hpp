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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#pragma once
#include "Identifiable.hpp"
#include "heterogeneous.hpp"

namespace xacc {
namespace quantum {
// Forward declaration
class Gate;
} // namespace quantum

// Readout error: pair of meas0Prep1, meas1Prep0
using RoErrors = std::pair<double, double>;

struct KrausOp {
  size_t qubit;
  // Choi matrix
  std::vector<std::vector<std::complex<double>>> mats;
};

class NoiseModel : public Identifiable {
public:
  NoiseModel() = default;
  virtual void initialize(const HeterogeneousMap &params) = 0;
  virtual std::string toJson() const = 0;
  // Readout errors:
  virtual RoErrors readoutError(size_t qubitIdx) const = 0;
  virtual std::vector<RoErrors> readoutErrors() const = 0;
  // Gate errors:
  // Returns a list of Kraus operators represent quantum noise processes
  // associated with a quantum gate.
  // Note: we use Kraus operators to capture generic noise processes.
  // Any probabilistic gate-based noise representations must be converted to
  // the equivalent Kraus operators.
  virtual std::vector<KrausOp>
  gateError(xacc::quantum::Gate &gate) const = 0;
};

// Utils to convert b/w different noise channel representations.
// The default implementation to be contributed elsewhere.
class NoiseModelUtils : public Identifiable {
public:
  using cMat = std::vector<std::vector<std::complex<double>>>;
  virtual cMat krausToChoi(const std::vector<cMat> &in_krausMats) const = 0;
  virtual std::vector<cMat> choiToKraus(const cMat &in_choiMat) const = 0;
};
} // namespace xacc