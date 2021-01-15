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
#include "qcmx.hpp"
#include "ObservableTransform.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "PauliOperator.hpp"

#include <Eigen/Dense>
#include <complex>
#include <memory>

using namespace xacc;
using namespace xacc::quantum;

#ifdef _XACC_NO_STD_BETA
#include "boost/math/special_functions/beta.hpp"
namespace std {
double beta(double x, double y) { return boost::math::beta(x, y); }
} // namespace std
#endif

namespace xacc {
namespace algorithm {

bool QCMX::initialize(const HeterogeneousMap &parameters) {

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

  if (!parameters.pointerLikeExists<CompositeInstruction>("ansatz")) {
    xacc::error("Ansatz was false.");
    return false;
  }

  accelerator = parameters.getPointerLike<Accelerator>("accelerator");
  order = parameters.get<int>("cmx-order");
  kernel = parameters.getPointerLike<CompositeInstruction>("ansatz");

  // check if Observable needs JW
  if (std::dynamic_pointer_cast<PauliOperator>(xacc::as_shared_ptr(
          parameters.getPointerLike<Observable>("observable")))) {
    observable = xacc::as_shared_ptr(
        parameters.getPointerLike<Observable>("observable"));
  } else {
    auto jw = xacc::getService<ObservableTransform>("jw");
    observable = jw->transform(xacc::as_shared_ptr(
        parameters.getPointerLike<Observable>("observable")));
  }

  // Check if expansion type is valid
  expansion = parameters.getString("expansion-type");
  if (!xacc::container::contains(expansions, expansion)) {
    xacc::error(expansion + " is not a valid expansion. Choose Cioslowski, "
                            "Knowles, or PDS.");
  }

  return true;
}

const std::vector<std::string> QCMX::requiredParameters() const {
  return {"observable", "accelerator", "cmx-order", "expansion-type", "ansatz"};
}

void QCMX::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // compute moments of the Hamiltonian: <H^n>
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
  if (expansion == "PDS") {
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

// This is just a wrapper to compute <H^n> with VQE::execute(q, {})
double QCMX::expValue(const std::shared_ptr<Observable> moment,
                      const std::shared_ptr<AcceleratorBuffer> buffer) const {

  auto q = xacc::qalloc(buffer->size());
  auto vqe = xacc::getAlgorithm("vqe", {{"observable", moment},
                                        {"accelerator", accelerator},
                                        {"ansatz", kernel}});
  return vqe->execute(q, {})[0];
}

// Compute energy from CMX
// J. Phys. A: Math. Gen. 17, 625 (1984)
// and Int. J. Mod. Phys. B 9, 2899 (1995)
double QCMX::PDS(const std::vector<double> moments) const {

  Eigen::MatrixXd M(order, order);
  Eigen::VectorXd b(order);

  // Compute matrix elements
  for (int i = 0; i < order; i++) {
    b(i) = moments[2 * order - i - 2];
    for (int j = i; j < order; j++) {
      if (i + j == 2 * (order - 1)) {
        M(i, j) = 1.0;
      } else {
        M(i, j) = moments[2 * order - (i + j) - 3];
      }
      M(j, i) = M(i, j);
    }
  }

  // Solve linear system Ma = -b
  Eigen::VectorXd a = M.colPivHouseholderQr().solve(-b).transpose();

  // Energy spectrum is given by roots of the polynomial
  // P_n(x) = \sum_0^n a_i x^(n - i)
  // Find roots by computing the companion matrix of P_n(x)
  // and diagonalizing it
  //
  // companionMatrix = |0    ... a_(n - 1)|
  //                   |I_n  ...    a_0   |
  //
  Eigen::MatrixXd companionMatrix = Eigen::MatrixXd::Zero(order, order);
  companionMatrix.bottomLeftCorner(order - 1, order - 1) =
      Eigen::MatrixXd::Identity(order - 1, order - 1);
  for (int i = 0; i < order; i++) {
    companionMatrix(i, order - 1) = -a(order - i - 1);
  }

  // get roots from eigenvalues
  // and return lowest energy eigenvalue
  Eigen::EigenSolver<Eigen::MatrixXd> es(companionMatrix);
  std::vector<double> spectrum(order);
  std::transform(es.eigenvalues().begin(), es.eigenvalues().end(),
                 spectrum.begin(),
                 [](std::complex<double> c) { return std::real(c); });
  auto energy = *std::min_element(spectrum.begin(), spectrum.end());

  return energy;
}

// Compute energy from CMX in Phys. Rev. Lett, 58, 83 (1987)
double QCMX::Cioslowski(const std::vector<double> moments) const {

  // compute connected moments
  // binomial coefficients (k choose i) given by:
  // (k, i) = 1 / ((k + 1) * std::beta(k - i + 1, i + 1)
  std::vector<double> I(moments);
  for (int k = 1; k < moments.size(); k++) {
    for (int i = 0; i < k; i++) {
      I[k] -= 1 / ((k + 1) * std::beta(k - i + 1, i + 1)) * I[i] *
              moments[k - i - 1];
    }
  }

  // recursion formula to compute S
  // S(k, i + 1) = S(k, 1) * S(k + 2, i) - S(k + 1, i)^2
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
  double previous = 1.0, current;
  for (int K = 1; K < order; K++) {
    current =
        std::pow(S(1, K, I), 2) / (std::pow(S(1, K - 1, I), 2) * S(2, K, I));
    energy -= previous * current;
    previous = current;
  }

  return energy;
}

// Compute energy from CMX in Chem. Phys. Lett., 143, p.512 (1987)
double QCMX::Knowles(const std::vector<double> moments) const {

  // compute connected moments
  std::vector<double> I(moments);
  for (int k = 1; k < moments.size(); k++) {
    for (int i = 0; i < k; i++) {
      I[k] -= 1 / ((k + 1) * std::beta(k - i + 1, i + 1)) * I[i] *
              moments[k - i - 1];
    }
  }

  // The correlation energy is b^T * M * b
  //
  //      |  I_2  |
  // b =  |  I_3  |
  //      |  ...  |
  //      |I_(m+1)|
  //
  //      | I_3  I_4 ... I_(m+2)  |
  // M =  | I_4                   |
  //      | ...                   |
  //      | I_(m+2) ...  I_(2m+1) |
  //
  Eigen::MatrixXd M(order - 1, order - 1);
  Eigen::VectorXd b(order - 1);

  // Compute matrix elements
  for (int i = 0; i < order - 1; i++) {
    b(i) = I[i + 1];
    for (int j = i; j < order - 1; j++) {
      M(i, j) = I[i + j + 2];
      M(j, i) = M(i, j);
    }
  }

  // Add <H> and return
  auto energy = I[0] - b.transpose() * M.inverse() * b;
  return energy;
}

} // namespace algorithm
} // namespace xacc
