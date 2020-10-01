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
#include "qeom.hpp"

#include "Accelerator.hpp"
#include "Observable.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "OperatorPool.hpp"
#include "PauliOperator.hpp"
#include <memory>
#include <iomanip>
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace algorithm {

bool qEOM::initialize(const HeterogeneousMap &parameters) {
  if (!parameters.pointerLikeExists<Observable>("observable")) {
    xacc::error("Obs was false");
    return false;
  }

  if (!parameters.pointerLikeExists<CompositeInstruction>("ansatz")) {
    xacc::error("Ansatz was false");
    return false;
  }

  if (!parameters.pointerLikeExists<Accelerator>("accelerator")) {
    xacc::error("Accelerator was false");
    return false;
  }

  observable = parameters.getPointerLike<Observable>("observable");
  if (parameters.pointerLikeExists<Optimizer>("optimizer")) {
    optimizer = parameters.getPointerLike<Optimizer>("optimizer");
  }
  accelerator = parameters.getPointerLike<Accelerator>("accelerator");
  kernel = parameters.getPointerLike<CompositeInstruction>("ansatz");

  return true;
}

const std::vector<std::string> qEOM::requiredParameters() const {
  return {"observable", "optimizer", "accelerator", "ansatz"};
}

void qEOM::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // call vqe here
  auto vqe =
      xacc::getAlgorithm("vqe", {{"optimizer", xacc::as_shared_ptr(optimizer)},
                                 {"observable", observable},
                                 {"ansatz", kernel},
                                 {"accelerator", accelerator}});

  vqe->execute(buffer);
  auto x = (*buffer)["opt-params"].as<std::vector<double>>();
  auto spectrum = EOM(buffer, x);

  return;
}

std::vector<double>
qEOM::execute(const std::shared_ptr<AcceleratorBuffer> buffer,
              const std::vector<double> &x) {

  // auto spectrum = {};//qEOM(buffer, x);
  return {0.0}; // spectrum;
}

// this is where the actual qEOM takes place
std::vector<double> qEOM::EOM(const std::shared_ptr<AcceleratorBuffer> buffer,
                              std::vector<double> x) const {

  auto pool = xacc::getService<OperatorPool>("singlet-adapted-uccsd");
  auto operators = pool->generate(buffer->size());
  int nOperators = operators.size();
  Eigen::MatrixXd M = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd Q = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd V = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd W = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd MQ =
      Eigen::MatrixXd::Zero(2 * nOperators, 2 * nOperators);
  Eigen::MatrixXd VW =
      Eigen::MatrixXd::Zero(2 * nOperators, 2 * nOperators);

  // this lambda computes
  // [A, H, B] = 1/2 x ([[A,H],B] + [A,[H,B]])
  auto doubleCommutator = [&](std::shared_ptr<Observable> A,
                              std::shared_ptr<Observable> B) {
    // [A,H]
    auto AH = A->commutator(xacc::as_shared_ptr(observable));
    // [[A,H],B]
    auto AH_B = *std::dynamic_pointer_cast<PauliOperator>(AH->commutator(B));
    // [H,B]
    auto HB = observable->commutator(B);
    // [A,[H,B]]
    auto A_HB = *std::dynamic_pointer_cast<PauliOperator>(A->commutator(HB));

    auto sum = 0.5 * (AH_B + A_HB);

    return std::dynamic_pointer_cast<Observable>(
        std::make_shared<PauliOperator>(sum));
  };

  // loop over bra
  for (int mu = 0; mu < nOperators; mu++) {

    auto tmp_bra = (*std::dynamic_pointer_cast<PauliOperator>(operators[mu]))
                       .hermitianConjugate();
    auto bra = std::dynamic_pointer_cast<Observable>(
        std::make_shared<PauliOperator>(tmp_bra));

    // loop over ket
    for (int nu = 0; nu < nOperators; nu++) {

      auto ket = operators[nu];
      auto tmp_ketConj =
          (*std::dynamic_pointer_cast<PauliOperator>(operators[nu]))
              .hermitianConjugate();
      auto ketConj = std::dynamic_pointer_cast<Observable>(
          std::make_shared<PauliOperator>(tmp_ketConj));

      // Observables to compute the qEOM matrix elements
      auto MOp = doubleCommutator(bra, ket);
      M(mu, nu) = VQEWrapper(buffer, x, MOp);

      auto QOp = doubleCommutator(bra, ketConj);
      Q(mu, nu) = -VQEWrapper(buffer, x, QOp);

      auto VOp = bra->commutator(ket);
      V(mu, nu) = VQEWrapper(buffer, x, VOp);

      auto WOp = bra->commutator(ketConj);
      W(mu, nu) = -VQEWrapper(buffer, x, WOp);
    }
  }

  // LHS matrix composed of M and Q
  // | M  Q  |
  // | Q^ M^ |git lo
  MQ.topLeftCorner(nOperators, nOperators) = M;
  MQ.topRightCorner(nOperators, nOperators) = Q;
  MQ.bottomLeftCorner(nOperators, nOperators) = Q.adjoint();
  MQ.bottomRightCorner(nOperators, nOperators) = M.adjoint();

  // RHS matrix composed of V and W
  // |  V   W  |
  // | -W^ -V^ |
  VW.topLeftCorner(nOperators, nOperators) = V;
  VW.topRightCorner(nOperators, nOperators) = W;
  VW.bottomLeftCorner(nOperators, nOperators) = -W.adjoint();
  VW.bottomRightCorner(nOperators, nOperators) = -V.adjoint();

  // Compute eigenvalues
  Eigen::GeneralizedEigenSolver<Eigen::MatrixXd> ges;
  ges.compute(MQ, VW);
  auto spectrum = ges.eigenvalues();
  std::cout << spectrum.transpose() << "\n";

  return {};
}

double qEOM::VQEWrapper(const std::shared_ptr<AcceleratorBuffer> buffer,
                        const std::vector<double> &x,
                        const std::shared_ptr<Observable> obs) const {

  auto vqe = xacc::getAlgorithm("vqe", {{"optimizer", optimizer},
                                        {"observable", obs},
                                        {"ansatz", kernel},
                                        {"accelerator", accelerator}});
  auto tmpBuffer = xacc::qalloc(buffer->size());
  return vqe->execute(tmpBuffer, x)[0];
}

} // namespace algorithm
} // namespace xacc