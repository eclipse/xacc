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
#include <cassert>
#include <complex>

namespace xacc {
namespace quantum {
// Forward declaration
class Gate;
} // namespace quantum

// Readout error: pair of meas0Prep1, meas1Prep0
using RoErrors = std::pair<double, double>;

// The LSB, MSB bit-order that Kraus matrices are defined in.
enum class KrausMatBitOrder { LSB, MSB };

// Represent a generic noise channel in terms of
// Kraus operator matrices to be applied *post-operation*.
// The list of matrices always satisfies CPTP condition.
struct NoiseChannelKraus {
  using KrausMatType = std::vector<std::vector<std::complex<double>>>;
  std::vector<size_t> noise_qubits;
  std::vector<KrausMatType> mats;
  KrausMatBitOrder bit_order;
  NoiseChannelKraus(const std::vector<size_t> &in_qubits,
                    const std::vector<KrausMatType> &in_mats,
                    KrausMatBitOrder in_bitOrder)
      : noise_qubits(in_qubits), mats(in_mats), bit_order(in_bitOrder) {
    const auto expectedDim = 1ULL << in_qubits.size();
    for (const auto &mat : mats) {
      assert(mat.size() == expectedDim);
      for (const auto &row : mat) {
        assert(row.size() == expectedDim);
      }
    }
  }
};

class NoiseModel : public Identifiable {
public:
  NoiseModel() = default;
  virtual void initialize(const HeterogeneousMap &params) = 0;
  virtual std::string toJson() const = 0;
  // Readout errors:
  virtual RoErrors readoutError(size_t qubitIdx) const = 0;
  virtual std::vector<RoErrors> readoutErrors() const = 0;
  // Returns a list of noise channels (in terms of Kraus matrices)
  // for an XACC gate.
  virtual std::vector<NoiseChannelKraus>
  getNoiseChannels(xacc::quantum::Gate &gate) const {
    return {};
  }

  // Get gate error probability:
  virtual double gateErrorProb(xacc::quantum::Gate &gate) const = 0;
  // Query Fidelity information:
  virtual size_t nQubits() const = 0;
  virtual std::vector<double> averageSingleQubitGateFidelity() const = 0;
  virtual std::vector<std::tuple<size_t, size_t, double>>
  averageTwoQubitGateFidelity() const = 0;
};

// Utils to convert b/w different noise channel representations.
// The default implementation to be contributed elsewhere.
class NoiseModelUtils : public Identifiable {
public:
  using cMat = std::vector<std::vector<std::complex<double>>>;
  virtual cMat krausToChoi(const std::vector<cMat> &in_krausMats) const = 0;
  virtual std::vector<cMat> choiToKraus(const cMat &in_choiMat) const = 0;
  // Combine Choi matrices acting on a single channel:
  virtual cMat
  combineChannelOps(const std::vector<cMat> &in_choiMats) const = 0;
};
} // namespace xacc