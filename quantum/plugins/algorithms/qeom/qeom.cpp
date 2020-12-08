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

#include "Utils.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "PauliOperator.hpp"
#include "FermionOperator.hpp"
#include "ObservableTransform.hpp"
#include "xacc_observable.hpp"
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

  if (!parameters.keyExists<std::vector<std::shared_ptr<Observable>>>(
          "operators")) {
    xacc::error("Excitation operators was false");
    return false;
  }

  accelerator = parameters.getPointerLike<Accelerator>("accelerator");
  kernel = parameters.getPointerLike<CompositeInstruction>("ansatz");
  operators =
      parameters.get<std::vector<std::shared_ptr<Observable>>>("operators");

  observable =
      xacc::as_shared_ptr(parameters.getPointerLike<Observable>("observable"));
  if (observable->toString().find("^") != std::string::npos) {

    auto jw = xacc::getService<ObservableTransform>("jw");
    if (std::dynamic_pointer_cast<FermionOperator>(observable)) {
      observable = jw->transform(observable);
    } else {
      auto fermionObservable =
          xacc::quantum::getObservable("fermion", observable->toString());
      observable = jw->transform(
          std::dynamic_pointer_cast<Observable>(fermionObservable));
    }

    // observable is PauliOperator, but does not cast down to it
    // Not sure about the likelihood of this happening, but want to cover all
    // bases
  } else if (observable->toString().find("X") != std::string::npos ||
             observable->toString().find("Y") != std::string::npos ||
             observable->toString().find("Z") != std::string::npos &&
                 !std::dynamic_pointer_cast<PauliOperator>(observable)) {

    auto pauliObservable =
        xacc::quantum::getObservable("pauli", observable->toString());
    observable = std::dynamic_pointer_cast<Observable>(pauliObservable);
  }


  return true;
}

const std::vector<std::string> qEOM::requiredParameters() const {
  return {"observable", "accelerator", "ansatz", "operators"};
}

void qEOM::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  int nOperators = operators.size();
  Eigen::MatrixXd M = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd Q = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd V = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd W = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd MQ = Eigen::MatrixXd::Zero(2 * nOperators, 2 * nOperators);
  Eigen::MatrixXd VW = Eigen::MatrixXd::Zero(2 * nOperators, 2 * nOperators);

  // this lambda computes
  // [A, H, B] = 1/2 x ([[A,H],B] + [A,[H,B]])
  auto doubleCommutator = [&](std::shared_ptr<Observable> A,
                              std::shared_ptr<Observable> B) {
    // [A,H]
  
    auto AH = A->commutator(observable);
    // [[A,H],B]
    auto AH_B = *std::dynamic_pointer_cast<PauliOperator>(AH->commutator(B));
    // [H,B]
    auto HB = observable->commutator(B);
    // [A,[H,B]]
    auto A_HB = *std::dynamic_pointer_cast<PauliOperator>(A->commutator(HB));

/*
    auto AH_B = *std::dynamic_pointer_cast<PauliOperator>(A->commutator(observable)->commutator(B));
    auto A_HB = *std::dynamic_pointer_cast<PauliOperator>(A->commutator(observable->commutator(B)));
    //auto sum = 0.5 * (2*X*H*Y + 2*Y*H*X - Y*X*H - H*X*Y - H*Y*X - X*Y*H);
*/
auto ret = 0.5 * (AH_B + A_HB);
    return std::dynamic_pointer_cast<Observable>(
        std::make_shared<PauliOperator>(ret));
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

      // qEOM matrix elements
      std::cout << "LEFT\n" << bra->toString() << "\n";
      std::cout << "RIGHT\n" << ket->toString() << "\n";
      std::cout << doubleCommutator(bra, ket)->toString() << "\n\n";
      M(mu, nu) = VQEWrapper(buffer, doubleCommutator(bra, ket));
      Q(mu, nu) = -VQEWrapper(buffer, doubleCommutator(bra, ketConj));
      V(mu, nu) = VQEWrapper(buffer, bra->commutator(ket));
      W(mu, nu) = -VQEWrapper(buffer, bra->commutator(ketConj));

    }
  }

  std::cout << "Computed matrix elements\n";
  // LHS matrix
  // | M  Q  |
  // | Q^ M^ |
  MQ.topLeftCorner(nOperators, nOperators) = M;
  MQ.topRightCorner(nOperators, nOperators) = Q;
  MQ.bottomLeftCorner(nOperators, nOperators) = Q.adjoint();
  MQ.bottomRightCorner(nOperators, nOperators) = M.adjoint();

  // RHS matrix
  // |  V   W  |
  // | -W^ -V^ |
  VW.topLeftCorner(nOperators, nOperators) = V;
  VW.topRightCorner(nOperators, nOperators) = W;
  VW.bottomLeftCorner(nOperators, nOperators) = -W.adjoint();
  VW.bottomRightCorner(nOperators, nOperators) = -V.adjoint();
  std::cout << M << "\n";
  std::cout << Q << "\n";
  std::cout << V << "\n";
  std::cout << V << "\n";

  // Compute eigenvalues
  Eigen::GeneralizedEigenSolver<Eigen::MatrixXd> ges;
  ges.compute(MQ, VW);
  auto spectrum = ges.eigenvalues();
  std::cout << spectrum.transpose() << "\n";

  return;
}

double qEOM::VQEWrapper(const std::shared_ptr<AcceleratorBuffer> buffer,
                        const std::shared_ptr<Observable> obs) const {
  auto vqe = xacc::getAlgorithm(
      "vqe",
      {{"observable", obs}, {"ansatz", kernel}, {"accelerator", accelerator}});
  auto tmpBuffer = xacc::qalloc(buffer->size());
  return vqe->execute(tmpBuffer, {})[0];
}

} // namespace algorithm
} // namespace xacc