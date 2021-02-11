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

  // the energies are stored in this map
  std::map<std::string, double> energies;

  auto H = *std::dynamic_pointer_cast<PauliOperator>(observable);
  auto momentOperator = H;
  std::vector<double> moments;
  // The energy computation itself is just classical post processing
  // So we can compute the energy from all implemented expansions
  // because the bottleneck is the quantum computation of the moments
  for (int i = 0; i < 2 * maxOrder - 1; i++) {
    auto momentExpValue = measureOperator(
        std::make_shared<PauliOperator>(momentOperator), buffer->size());
    moments.push_back(momentExpValue);

    double e;
    if ((i != 0) && (i % 2 == 0)) {
      e = Cioslowski(moments, i / 2 + 1);
      energies.emplace("CMX(" + std::to_string(i / 2 + 1) + ")", e);
      std::stringstream ss;
      ss << std::setprecision(12) << "CMX(" << i / 2 + 1 << ") = " << e;
      xacc::info(ss.str());
      ss.str(std::string());

      e = PDS(moments, i / 2 + 1);
      energies.emplace("PDS(" + std::to_string(i / 2 + 1) + ")", e);
      ss << std::setprecision(12) << "PDS(" << i / 2 + 1 << ") = " << e;
      xacc::info(ss.str());
      ss.str(std::string());

      e = Knowles(moments, i / 2 + 1);
      energies.emplace("Knowles(" + std::to_string(i / 2 + 1) + ")", e);
      ss << std::setprecision(12) << "Knowles(" << i / 2 + 1 << ") = " << e;
      xacc::info(ss.str());
      ss.str(std::string());
    }

    if (i == 2) {
      e = Soldatov(moments);
      energies.emplace("Soldatov", e);
      std::stringstream ss;
      ss << std::setprecision(12) << "Soldatov = " << e;
      xacc::info(ss.str());
      ss.str(std::string());
    }

    momentOperator *= H;
  }

  // add energy to the buffer
  buffer->addExtraInfo("energies", energies);
  buffer->addExtraInfo("spectrum", spectrum);
  return;
}

double QCMX::measureOperator(const std::shared_ptr<Observable> obs,
                             const int bufferSize) const {

  std::vector<std::shared_ptr<CompositeInstruction>> kernels;
  if (x.empty()) {
    kernels = obs->observe(xacc::as_shared_ptr(kernel));
  } else {
    auto evaled = kernel->operator()(x);
    kernels = obs->observe(evaled);
  }

  // we loop over all measured circuits
  // and check if that term has been measured
  // if so, we just multiply the measurement by the coefficient
  // We gather all new circuits into fsToExec and execute
  // Because these are all commutators, we don't need to worry about the I term
  // Also, terms with small coeffs can be ignored by setting threshold below
  std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;
  std::vector<std::complex<double>> coefficients;
  double total = 0.0;
  for (auto &f : kernels) {
    std::complex<double> coeff = f->getCoefficient();
    if (cachedMeasurements.find(f->name()) != cachedMeasurements.end()) {
      total += std::real(coeff * cachedMeasurements[f->name()]);
    } else if (std::fabs(coeff.real()) >= threshold) {
      fsToExec.push_back(f);
      coefficients.push_back(coeff);
    }
  }

  // for circuits that have not been executed previously
  auto tmpBuffer = xacc::qalloc(bufferSize);
  accelerator->execute(tmpBuffer, fsToExec);
  auto buffers = tmpBuffer->getChildren();
  xacc::info("Number of terms to be measured = " +
             std::to_string(fsToExec.size()));
  for (int i = 0; i < fsToExec.size(); i++) {
    auto expval = buffers[i]->getExpectationValueZ();
    total += std::real(expval * coefficients[i]);
    cachedMeasurements.emplace(fsToExec[i]->name(), expval);
  }

  return total;
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

} // namespace algorithm
} // namespace xacc
