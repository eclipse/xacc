/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
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

#include "hf.hpp"
#include "xacc.hpp"
#include "CommonGates.hpp"
#include <Eigen/Dense>
#include <unsupported/Eigen/KroneckerProduct>

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace circuits {
const std::vector<std::string> HF::requiredKeys() {
  return {"transform", "ne", "nq"};
}

bool HF::expand(const HeterogeneousMap &parameters) {

  if (!parameters.keyExists<int>("ne")) {
    xacc::error("HF needs number of electrons.");
    return false;
  }

  if (!parameters.keyExists<int>("nq")) {
    xacc::error("HF needs number of qubits/orbitals.");
    return false;
  }

  auto ne = parameters.get<int>("ne");
  auto nq = parameters.get<int>("nq");

  std::string transform = "jw";
  if (!parameters.stringExists("transform")) {
    xacc::warning("No transform provided. Defaulting to Jordan-Wigner");
  } else {
    transform = parameters.getString("transform");
  }

  if (transform == "jw") {

    // JW transform acts diagonally onto the occupation number basis
    for (std::size_t i = 0; i < ne / 2; i++) {
      addInstruction(std::make_shared<xacc::quantum::X>(i));
      addInstruction(std::make_shared<xacc::quantum::X>(i + nq / 2));
    }

  } else if (transform == "bk") {

    // BK transform is not trivial (not diagonal)
    // this implementation follows J. Chem. Phys. 137, 224109 (2012)

    // construct BK transform matrix
    Eigen::Matrix2i I = Eigen::Matrix2i::Identity(2, 2);
    Eigen::MatrixXi H, B = Eigen::Matrix2i::Identity(2, 2);
    B.row(0) = Eigen::VectorXi::Ones(B.rows());
    auto nBranches = (int)std::ceil(std::log2(nq));

    for (int i = 0; i < nBranches - 1; i++) {
      H = kroneckerProduct(I, B);
      H.row(0) = Eigen::VectorXi::Ones(B.rows());
      B = H;
    }

    // this is necessary if nq % 2 != 0
    auto dim = (int)std::pow(2, nBranches);
    auto subMatrixIndex = dim - nq;
    B = B.bottomRightCorner(dim - subMatrixIndex, dim - subMatrixIndex);

    // Construct occupation vector (MSB)
    Eigen::VectorXi occVec = Eigen::VectorXi::Zero(nq);
    for (std::size_t i = 0; i < ne / 2; i++) {
      occVec(nq - i - 1) = 1;
      occVec(nq / 2 + i - 1) = 1;
    }

    // transform occupation vector
    Eigen::VectorXi bkOccVec = B * occVec;

    // add X gates (LSB)
    for (int i = 0; i < nq; i++) {
      if ((bkOccVec(nq - i - 1) % 2)) {
        addInstruction(std::make_shared<xacc::quantum::X>(i));
      }
    }
  }

  return true;
}

} // namespace circuits
} // namespace xacc