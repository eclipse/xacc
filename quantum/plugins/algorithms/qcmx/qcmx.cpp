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
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

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

  if (!parameters.pointerLikeExists<CompositeInstruction>("ansatz")) {
    xacc::error("Ansatz was false.");
    return false;
  }

  accelerator = parameters.getPointerLike<Accelerator>("accelerator");
  maxOrder = parameters.get<int>("cmx-order");
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

  if (parameters.keyExists<double>("threshold")) {
    threshold = parameters.get<double>("threshold");
    xacc::info("Ignoring measurements with coefficient below = " +
               std::to_string(threshold));
  }

  // in case the ansatz is parameterized
  if (parameters.keyExists<std::vector<double>>("parameters")) {
    x = parameters.get<std::vector<double>>("parameters");
    std::reverse(x.begin(), x.end());
  }

  return true;
}

const std::vector<std::string> QCMX::requiredParameters() const {
  return {"observable", "accelerator", "cmx-order", "ansatz"};
}

void QCMX::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // First gather the operators for all required moments
  std::vector<std::shared_ptr<Observable>> momentOperators;
  auto momentOperator = PauliOperator("I");
  for (int i = 0; i < 2 * maxOrder - 1; i++) {
    momentOperator *= (*std::dynamic_pointer_cast<PauliOperator>(observable));
    momentOperators.push_back(std::make_shared<PauliOperator>(momentOperator));
  }

  // now get the unique terms in all moments operators
  auto uniqueTerms = getUniqueTerms(momentOperators);
  auto kernels = uniqueTerms->observe(xacc::as_shared_ptr(kernel));
  accelerator->execute(buffer, kernels);
  auto buffers = buffer->getChildren();

  // compute moments
  auto moments = getMoments(momentOperators, buffers);

  // the energies are stored in this map
  std::map<std::string, double> energies;
  for (int i = 2; i <= maxOrder; i++) {

    // The energy computation itself is just classical post processing
    // So we can compute the energy from all implemented expansions
    // because the bottleneck is the quantum computation of the moments
    double e;
    std::vector<double> momentsUpToOrder(moments.begin(),
                                         moments.begin() + 2 * i - 1);
    e = Cioslowski(momentsUpToOrder, i);
    energies.emplace("CMX(" + std::to_string(i) + ")", e);
    std::stringstream ss;
    ss << std::setprecision(12) << "CMX(" << i << ") = " << e;
    xacc::info(ss.str());
    ss.str(std::string());

    e = PDS(momentsUpToOrder, i / 2 + 1);
    energies.emplace("PDS(" + std::to_string(i) + ")", e);
    ss << std::setprecision(12) << "PDS(" << i << ") = " << e;
    xacc::info(ss.str());
    ss.str(std::string());

    e = Knowles(momentsUpToOrder, i / 2 + 1);
    energies.emplace("Knowles(" + std::to_string(i) + ")", e);
    ss << std::setprecision(12) << "Knowles(" << i << ") = " << e;
    xacc::info(ss.str());
    ss.str(std::string());

    if (i == 2) {
      e = Soldatov(momentsUpToOrder);
      energies.emplace("Soldatov", e);
      std::stringstream ss;
      ss << std::setprecision(12) << "Soldatov = " << e;
      xacc::info(ss.str());
      ss.str(std::string());
    }
  }

  // add energy to the buffer
  buffer->addExtraInfo("energies", energies);
  buffer->addExtraInfo("spectrum", spectrum);
  return;
}

// Compute energy from CMX
// J. Phys. A: Math. Gen. 17, 625 (1984)
// and Int. J. Mod. Phys. B 9, 2899 (1995)
double QCMX::PDS(const std::vector<double> &moments, const int order) const {

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
  spectrum.resize(order);
  std::transform(es.eigenvalues().begin(), es.eigenvalues().end(),
                 spectrum.begin(),
                 [](std::complex<double> c) { return std::real(c); });
  auto energy = *std::min_element(spectrum.begin(), spectrum.end());

  return energy;
}

// Compute energy from CMX in Phys. Rev. Lett, 58, 83 (1987)
double QCMX::Cioslowski(const std::vector<double> &moments,
                        const int order) const {

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

  // recursion formula to compute the energy correction order-by-order
  std::function<double(const int, const std::vector<double>)> S;
  S = [=, &S](const int k, const std::vector<double> I) {
    if (k == 1) {
      return std::pow(I[0], 2) / I[1];
    } else {
      std::vector<double> Ip(2 * k - 2);
      for (int i = 0; i < 2 * k - 2; i++) {
        Ip[i] = I[i] * I[i + 2] - std::pow(I[i + 1], 2);
      }
      return S(k - 1, Ip) / I[1];
    }
  };

  // compute energy
  auto energy = I[0];
  for (int K = 1; K < order; K++) {
    energy -= S(K, std::vector<double>(I.begin() + 1, I.end()));
  }

  return energy;
}

// Compute energy from CMX in Chem. Phys. Lett., 143, p.512 (1987)
double QCMX::Knowles(const std::vector<double> &moments,
                     const int order) const {

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

// This is an approximate functional, not an order-by-order expansion
double QCMX::Soldatov(const std::vector<double> &moments) const {

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

  return I[0] + I[2] / (2 * I[1]) - sqrt(std::pow(I[2] / (2 * I[1]), 2) + I[1]);
}

std::shared_ptr<Observable> QCMX::getUniqueTerms(
    const std::vector<std::shared_ptr<Observable>> momentOps) const {

  auto uniqueTermsPtr = std::make_shared<PauliOperator>();
  for (auto &momentOp : momentOps) {
    for (auto &term : momentOp->getNonIdentitySubTerms()) {

      auto op =
          std::dynamic_pointer_cast<PauliOperator>(term)->begin()->second.ops();
      auto coeff = std::dynamic_pointer_cast<PauliOperator>(term)
                       ->begin()
                       ->second.coeff();
      if (std::fabs(coeff.real()) < threshold)
        continue;
      uniqueTermsPtr->operator+=(*std::make_shared<PauliOperator>(op));
    }
  }

  return uniqueTermsPtr;
}

std::vector<double> QCMX::getMoments(
    const std::vector<std::shared_ptr<Observable>> momentOperators,
    const std::vector<std::shared_ptr<AcceleratorBuffer>> buffers) const {

  std::vector<double> moments;
  for (int i = 0; i < 2 * maxOrder - 1; i++) {
    double expval = 0.0;
    if (momentOperators[i]->getIdentitySubTerm()) {
      expval +=
          std::real(momentOperators[i]->getIdentitySubTerm()->coefficient());
    }

    for (auto subTerm : momentOperators[i]->getNonIdentitySubTerms()) {
      auto term =
          std::dynamic_pointer_cast<PauliOperator>(subTerm)->begin()->second;

      for (auto buffer : buffers) {
        if (buffer->name() == term.id()) {
          expval += std::real(term.coeff() * buffer->getExpectationValueZ());
          break;
        }
      }
    }
    moments.push_back(expval);
  }
  return moments;
}

} // namespace algorithm
} // namespace xacc
