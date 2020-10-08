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
 ******************************************************************************/
#include "cmx.hpp"
#include "ObservableTransform.hpp"
#include "Utils.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "PauliOperator.hpp"

#include <Eigen/Dense>
#include <boost/math/special_functions/binomial.hpp>

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace algorithm {

bool CMX::initialize(const HeterogeneousMap &parameters) {

  if (!parameters.pointerLikeExists<Observable>("observable")) {
    xacc::error("Obs was false.");
    return false;
  }

  if (!parameters.pointerLikeExists<Accelerator>("accelerator")) {
    xacc::error("Acc was false.");
    return false;
  }

  if (!parameters.keyExists<int>("cmx-order")) {
    xacc::error("CMX order was false.");
    return false;
  }

  if (!parameters.stringExists("expansion-type")) {
    xacc::error("Expansion type was false.");
    return false;
  }

  // Map H to qubits
  auto jw = xacc::getService<ObservableTransform>("jw");
  observable = jw->transform(
      xacc::as_shared_ptr(parameters.getPointerLike<Observable>("observable")));

  accelerator = parameters.getPointerLike<Accelerator>("accelerator");
  order = parameters.get<int>("cmx-order");

  // Check if expansoon is valid
  expansion = parameters.getString("expansion-type");
  if (!xacc::container::contains(expansions, expansion)) {
    xacc::error(expansion + " is not a valid expansion. Choose Cioslowski, "
                "Knowles, or PDS.");
  }
  
  // use initial state if provided, otherwise prepare HF state
  // to prepare HF, we need the number of electrons
  if (parameters.pointerLikeExists<CompositeInstruction>("ansatz")) {
    kernel = parameters.getPointerLike<CompositeInstruction>("ansatz");
  } else {

    if (parameters.keyExists<int>("n-electrons")) {

      auto nElectrons = parameters.get<int>("n-electrons");
      auto provider = xacc::getIRProvider("quantum");
      std::size_t j;
      for (int i = 0; i < nElectrons / 2; i++) {
        j = (std::size_t)i;
        kernel->addInstruction(provider->createInstruction("X", {j}));
        j = (std::size_t)(i + observable->nBits() / 2);
        kernel->addInstruction(provider->createInstruction("X", {j}));
      }

    } else {

      xacc::error("Need n-electrons to prepare HF state.");
      return false;
    }
  }

  return true;
}

const std::vector<std::string> CMX::requiredParameters() const {
  return {"observable", "accelerator", "cmx-order", "expansion-type"};
}

void CMX::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // compute moments of the Hamiltonian <H^n>
  auto H = *std::dynamic_pointer_cast<PauliOperator>(observable);
  auto momentOperator = H;
  std::vector<double> moments;
  for (int i = 0; i < 2 * order - 1; i++) {
    auto momentExpValue =
        expValue(std::make_shared<PauliOperator>(momentOperator), buffer);
    moments.push_back(momentExpValue);
    momentOperator *= H;
  }

  // compute ground state energy with the chosen CMX
  double energy;
  if (expansion == "pds") {
    energy = PDS(moments);
  } else if (expansion == "Cioslowski") {
    energy = Cioslowski(moments);
  } else if (expansion == "Knowles") {
    energy = Knowles(moments);
  }

  // add energy to the buffer
  buffer->addExtraInfo("opt-val", energy);
  return;
}

double CMX::expValue(const std::shared_ptr<Observable> moment,
                     const std::shared_ptr<AcceleratorBuffer> buffer) const {

  auto q = xacc::qalloc(buffer->size());
  auto vqe = xacc::getAlgorithm("vqe", {{"observable", moment},
                                        {"accelerator", accelerator},
                                        {"ansatz", kernel}});
  return vqe->execute(q, {})[0];
}

// Compute energy from CMX in J. Phys. A: Math. Gen. 17, 625 (1984)
// and Int. J. Mod. Phys. B 9, 2899 (1995)
double CMX::PDS(const std::vector<double> moments) const {

  Eigen::MatrixXd M(order, order);
  Eigen::VectorXd b(order);

  // Compute matrix elements
  for (int i = 0; i < order; i++) {
    b(i) = moments[2 * order - i - 1];
    for (int j = i; j < order; j++) {
      M(i, j) = moments[2 * order - (i + j) - 1];
      M(j, i) = M(i, j);
    }
  }

  // Solve linear system
  Eigen::VectorXd a = M.colPivHouseholderQr().solve(b);

  // Find roots by computing the matrix corresponding to the
  // polynomial and diagonalizing it
  auto degree = b.size() - 1;
  Eigen::MatrixXd companionMatrix(degree, degree);
  for (int i = 0; i < order; i++) {
    for (int j = 0; j < order; j++) {

      if (i == j + 1) {
        companionMatrix(i, j) = 1.0;
      }

      if (j == degree - 1) {
        companionMatrix(i, j) = -b(i) / b(order);
      }
    }
  }

  // get roots from eigenvalues
  // and return lowest energy eigenvalue
  Eigen::EigenSolver<Eigen::MatrixXd> es(companionMatrix);
  std::vector<double> spectrum;
  std::transform(es.eigenvalues().begin(), es.eigenvalues().end(),
                 spectrum.begin(),
                 [](std::complex<double> c) { return std::real(c); });
  auto energy = *std::min_element(spectrum.begin(), spectrum.end());

  return energy;
}

// Compute energy from CMX in Phys. Rev. Lett, 58, 83 (1987)
double CMX::Cioslowski(const std::vector<double> moments) const {

  // compute connected moments
  std::vector<double> I;
  for (int k = 0; k < moments.size(); k++) {
    I[k] = moments[k];
    for (int i = 0; i < k - 2; k++) {
      I[k] -= boost::math::binomial_coefficient<double>(k, i) * I[i + 1] *
              moments[k - i];
    }
  }

  // recursion to compute S
  std::function<double(const int, const int, const std::vector<double>)> S;
  S = [&S](const int k, const int i, const std::vector<double> I) {
    if (i == 0) {
      return 1.0;
    } else if (i == 1) {
      return I[k];
    } else {
      return S(k, 1, I) * S(k + 2, i - 1, I) -
             S(k + 1, i - 1, I) * S(k + 1, i - 1, I);
    }
  };

  // compute energy
  auto energy = I[0];
  double previous = 1.0;
  for (int K = 2; K < order; K++) {
    energy -= previous * std::pow(S(2, K - 1, I), 2) /
              (std::pow(S(2, K - 2, I), 2) * S(3, K - 1, I));
    previous = std::pow(S(2, K - 1, I), 2) /
               (std::pow(S(2, K - 2, I), 2) * S(3, K - 1, I));
  }

  return energy;
}

// Compute energy from CMX in Chem. Phys. Lett., 143, p.512 (1987)
double CMX::Knowles(const std::vector<double> moments) const {

  // compute connected moments I_k Eq. 3
  std::vector<double> I;
  for (int k = 0; k < moments.size(); k++) {
    I[k] = moments[k];
    for (int i = 0; i < k - 2; k++) {
      I[k] -= boost::math::binomial_coefficient<double>(k, i) * I[i + 1] *
              moments[k - i];
    }
  }

  Eigen::MatrixXd M(order - 1, order - 1);
  Eigen::VectorXd b(order - 1);

  // Compute matrix elements
  for (int i = 0; i < order; i++) {
    b(i) = I[i + 1];
    for (int j = i; j < order; j++) {
      M(i, j) = I[i + j + 2];
    }
  }

  return b.transpose() * M * b;
}

} // namespace algorithm
} // namespace xacc
