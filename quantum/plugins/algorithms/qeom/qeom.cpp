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
#include <iostream>
#include <memory>

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
          "operators") &&
      (!parameters.keyExists<int>("n-occupied") &&
       !parameters.keyExists<int>("n-virtual"))) {
    xacc::error("qEOM needs either excitation operators or the number of "
                "occupied and virtual orbitals");
    return false;
  }

  accelerator = parameters.getPointerLike<Accelerator>("accelerator");
  kernel = parameters.getPointerLike<CompositeInstruction>("ansatz");

  if (parameters.keyExists<std::vector<std::shared_ptr<Observable>>>(
          "operators")) {
    operators =
        parameters.get<std::vector<std::shared_ptr<Observable>>>("operators");
  }

  if (parameters.keyExists<int>("n-occupied")) {
    xacc::info("Using single and double excitation operators");
    auto nOccupied = parameters.get<int>("n-occupied");
    auto nVirtual = parameters.get<int>("n-virtual");
    singlesDoublesOperators(nOccupied, nVirtual);
  }

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
  return {"observable", "accelerator", "ansatz"};
}

void qEOM::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  int nOperators = operators.size();
  Eigen::MatrixXd M = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd Q = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd V = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd W = Eigen::MatrixXd::Zero(nOperators, nOperators);

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

    // 1/2 x ([[A,H],B] + [A,[H,B]])
    auto ret = 0.5 * (AH_B + A_HB);
    return std::dynamic_pointer_cast<Observable>(
        std::make_shared<PauliOperator>(ret));
  };

  // Here we loop over operators on the left and on the right,
  // which I loosely refer to as bra and ket
  // loop over bra
  for (int i = 0; i < nOperators; i++) {

    auto tmp_bra = (*std::dynamic_pointer_cast<PauliOperator>(operators[i]))
                       .hermitianConjugate();
    auto bra = std::dynamic_pointer_cast<Observable>(
        std::make_shared<PauliOperator>(tmp_bra));

    // loop over ket
    for (int j = i; j < nOperators; j++) {

      auto ket = operators[j];
      auto tmp_ketConj =
          (*std::dynamic_pointer_cast<PauliOperator>(operators[j]))
              .hermitianConjugate();
      auto ketConj = std::dynamic_pointer_cast<Observable>(
          std::make_shared<PauliOperator>(tmp_ketConj));

      // qEOM matrix elements
      M(i, j) = VQEWrapper(doubleCommutator(bra, ket));
      Q(i, j) = -VQEWrapper(doubleCommutator(bra, ketConj));
      V(i, j) = VQEWrapper(bra->commutator(ket));
      W(i, j) = -VQEWrapper(bra->commutator(ketConj));

      M(j, i) = M(i, j);
      Q(j, i) = Q(i, j);
      V(j, i) = V(i, j);
      W(j, i) = W(i, j);
    }
  }

  Eigen::MatrixXd MQ = Eigen::MatrixXd::Zero(2 * nOperators, 2 * nOperators);
  Eigen::MatrixXd VW = Eigen::MatrixXd::Zero(2 * nOperators, 2 * nOperators);
  // LHS matrix
  // | M  Q  |
  // | Q^ M^ |
  MQ.topLeftCorner(nOperators, nOperators) = M;
  MQ.topRightCorner(nOperators, nOperators) = Q;
  MQ.bottomLeftCorner(nOperators, nOperators) = Q.adjoint();
  MQ.bottomRightCorner(nOperators, nOperators) = M.adjoint();

  // RHS matrix
  // |  V    W  |
  // | -V^  -W^ |
  VW.topLeftCorner(nOperators, nOperators) = V;
  VW.topRightCorner(nOperators, nOperators) = W;
  VW.bottomLeftCorner(nOperators, nOperators) = -W.adjoint();
  VW.bottomRightCorner(nOperators, nOperators) = -V.adjoint();

  // Compute eigenvalues
  // | M  Q  | |X| = E |  V    W  | |X|
  // | Q^ M^ | |Y|     | -V^  -W^ | |Y|
  Eigen::GeneralizedEigenSolver<Eigen::MatrixXd> ges;
  ges.compute(MQ, VW);
  std::vector<double> excitationEnergies;
  for (auto e : ges.eigenvalues()) {
    if (std::real(e) >= 0.0) {
      excitationEnergies.push_back(std::real(e));
    }
  }
  std::sort(excitationEnergies.begin(), excitationEnergies.end());
  buffer->addExtraInfo("excitation-energies", excitationEnergies);

  return;
}

double qEOM::VQEWrapper(const std::shared_ptr<Observable> obs) const {
  auto vqe = xacc::getAlgorithm(
      "vqe",
      {{"observable", obs}, {"ansatz", kernel}, {"accelerator", accelerator}});
  auto tmpBuffer = xacc::qalloc(observable->nBits());
  return vqe->execute(tmpBuffer, {})[0];
}

void qEOM::singlesDoublesOperators(const int nOccupied, const int nVirtual) {

  std::shared_ptr<Observable> fermiOp;
  auto jw = xacc::getService<ObservableTransform>("jw");
  auto nOrbitals = nOccupied + nVirtual;

  // singles
  for (int i = 0; i < nOccupied; i++) {
    int ia = i;
    int ib = i + nOrbitals;
    for (int a = 0; a < nVirtual; a++) {
      int aa = a + nOccupied;
      int ab = a + nOccupied + nOrbitals;

      fermiOp = std::make_shared<FermionOperator>(
          FermionOperator({{aa, 1}, {ia, 0}}, 4.0));
      operators.push_back(jw->transform(fermiOp));

      fermiOp = std::make_shared<FermionOperator>(
          FermionOperator({{ab, 1}, {ib, 0}}, 4.0));
      operators.push_back(jw->transform(fermiOp));

    }
  }

  // double excitations
  for (int i = 0; i < nOccupied; i++) {
    int ia = i;
    for (int j = i; j < nOccupied; j++) {
      int jb = j + nOrbitals;
      for (int a = 0; a < nVirtual; a++) {
        int aa = a + nOccupied;
        for (int b = a; b < nVirtual; b++) {
          int bb = b + nOccupied + nOrbitals;

          fermiOp = std::make_shared<FermionOperator>(
              FermionOperator({{aa, 1}, {ia, 0}, {bb, 1}, {jb, 0}}, 16.0));
          operators.push_back(jw->transform(fermiOp));

        }
      }
    }
  }
}

} // namespace algorithm
} // namespace xacc