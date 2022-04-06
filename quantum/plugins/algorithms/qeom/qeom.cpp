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

#include "Observable.hpp"
#include "Utils.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "xacc_plugin.hpp"
#include "PauliOperator.hpp"
#include "FermionOperator.hpp"
#include "ObservableTransform.hpp"
#include "xacc_observable.hpp"
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <memory>
#include <sstream>
#include <vector>
#include "OperatorPool.hpp"

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
      !parameters.keyExists<int>("n-electrons")) {
    xacc::error("qEOM needs either excitation operators or the number of "
                "electrons");
    return false;
  }

  accelerator = parameters.getPointerLike<Accelerator>("accelerator");
  kernel = parameters.getPointerLike<CompositeInstruction>("ansatz");
  observable =
      xacc::as_shared_ptr(parameters.getPointerLike<Observable>("observable"));

  if (parameters.keyExists<std::vector<std::shared_ptr<Observable>>>(
          "operators")) {
    operators =
        parameters.get<std::vector<std::shared_ptr<Observable>>>("operators");
  }

  auto jw = xacc::getService<ObservableTransform>("jw");
  // if no vector<Observable> was given
  if (parameters.keyExists<int>("n-electrons")) {
    auto nOrbitals = observable->nBits();
    auto nOccupied = parameters.get<int>("n-electrons");

    std::shared_ptr<OperatorPool> pool;
    // check if the name for a pool was given
    // if not, default to singles and doubles
    if (parameters.stringExists("pool")) {
      pool = xacc::getService<OperatorPool>(parameters.getString("pool"));
    } else {
      xacc::warning("No operator pool specified. Defaulting to single and "
                    "double excitation operators");
      pool = xacc::getService<OperatorPool>("singles-doubles-pool");
    }

    pool->optionalParameters(parameters);
    for (auto &op : pool->getExcitationOperators(nOrbitals)) {
      operators.push_back(jw->transform(op));
    }
  }

  if (observable->toString().find("^") != std::string::npos) {

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

  if (parameters.keyExists<bool>("compute-deexcitations")) {
    computeDeexcitations = parameters.get<bool>("compute-deexcitations");
  }

  return true;
}

const std::vector<std::string> qEOM::requiredParameters() const {
  return {"observable", "accelerator", "ansatz"};
}

void qEOM::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

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
  int nOperators = operators.size();
  std::vector<std::shared_ptr<Observable>> matElementOps;

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
      // first the diagonal blocks
      auto M_MatOperator = doubleCommutator(bra, ket);
      matElementOps.push_back(M_MatOperator);
      auto V_MatOperator = bra->commutator(ket);
      matElementOps.push_back(V_MatOperator);

      // off-diagonal blocks
      // only if computing de-excitations
      if (computeDeexcitations) {
        auto Q_MatOperator = doubleCommutator(bra, ketConj);
        matElementOps.push_back(Q_MatOperator);
        auto W_MatOperator = bra->commutator(ketConj);
        matElementOps.push_back(W_MatOperator);
      }
    }
  }

  // call Observable::observe() and Accelerator::execute() only once
  auto uniqueTermsPtr = getUniqueTerms(matElementOps);
  auto kernels = uniqueTermsPtr->observe(xacc::as_shared_ptr(kernel));
  accelerator->execute(buffer, kernels);
  auto buffers = buffer->getChildren();

  // compute matrix elements
  int counter = 0;
  Eigen::MatrixXd M = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd Q = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd V = Eigen::MatrixXd::Zero(nOperators, nOperators);
  Eigen::MatrixXd W = Eigen::MatrixXd::Zero(nOperators, nOperators);
  for (int i = 0; i < nOperators; i++) {
    for (int j = i; j < nOperators; j++) {

      M(i, j) = computeOperatorExpValue(matElementOps[counter++], buffers);
      M(j, i) = M(i, j);

      V(i, j) = computeOperatorExpValue(matElementOps[counter++], buffers);
      V(j, i) = V(i, j);

      if (computeDeexcitations) {
        Q(i, j) = computeOperatorExpValue(matElementOps[counter++], buffers);
        Q(j, i) = Q(i, j);

        W(i, j) = computeOperatorExpValue(matElementOps[counter++], buffers);
        W(j, i) = W(i, j);
      }
    }
  }

  Eigen::MatrixXd MQ = Eigen::MatrixXd::Zero(2 * nOperators, 2 * nOperators);
  Eigen::MatrixXd VW = Eigen::MatrixXd::Zero(2 * nOperators, 2 * nOperators);
  // LHS matrix
  // | M  Q  |
  // | Q^ M^ |
  MQ.topLeftCorner(nOperators, nOperators) = M;
  MQ.bottomRightCorner(nOperators, nOperators) = M.adjoint();
  if (computeDeexcitations) {
    MQ.topRightCorner(nOperators, nOperators) = Q;
    MQ.bottomLeftCorner(nOperators, nOperators) = Q.adjoint();
  }

  // RHS matrix
  // |  V    W  |
  // | -V^  -W^ |
  VW.topLeftCorner(nOperators, nOperators) = V;
  VW.bottomRightCorner(nOperators, nOperators) = -V.adjoint();
  if (computeDeexcitations) {
    VW.topRightCorner(nOperators, nOperators) = W;
    VW.bottomLeftCorner(nOperators, nOperators) = -W.adjoint();
  }

  // Compute eigenvalues
  // | M  Q  | |X| = E |  V    W  | |X|
  // | Q^ M^ | |Y|     | -W^  -V^ | |Y|
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

  // Printing spectrum
  std::stringstream ss;
  ss << "QEOM spectrum = ";
  for (int i = 0; i < excitationEnergies.size() - 1; i++) {
    ss << excitationEnergies[i] << ", ";
  }
  ss << excitationEnergies.back();
  xacc::info(ss.str());

  return;
}

std::shared_ptr<Observable> qEOM::getUniqueTerms(
    const std::vector<std::shared_ptr<Observable>> excitationOps) const {

  // loop over all operators and all terms for each operator
  // then check if that operator has been added to found
  // and add it if not
  // since each term is measured separately, we can have an operator
  // that's the sum of all unique terms
  auto uniqueTermsPtr = std::make_shared<PauliOperator>();
  for (auto &op : excitationOps) {
    for (auto &subTerm : op->getNonIdentitySubTerms()) {

      auto term1 = std::dynamic_pointer_cast<PauliOperator>(subTerm)->begin();
      if (!uniqueTermsPtr->getNonIdentitySubTerms().empty()) {

        for (auto uniqueTerm : uniqueTermsPtr->getNonIdentitySubTerms()) {
          auto term2 =
              std::dynamic_pointer_cast<PauliOperator>(uniqueTerm)->begin();

          if (term1->second.id() == term2->second.id()) {
            continue;
          } else {
            uniqueTermsPtr->operator+=(
                *std::make_shared<PauliOperator>(term1->second.ops()));
            break;
          }

        }

      } else {
        uniqueTermsPtr->operator+=(
            *std::make_shared<PauliOperator>(term1->second.ops()));
      }
    }
  }

  return uniqueTermsPtr;
}

double qEOM::computeOperatorExpValue(
    const std::shared_ptr<Observable> &excitationOp,
    const std::vector<std::shared_ptr<AcceleratorBuffer>> &buffers) const {

  double expval = 0.0;
  for (auto subTerm : excitationOp->getNonIdentitySubTerms()) {
    auto term =
        std::dynamic_pointer_cast<PauliOperator>(subTerm)->begin()->second;

    for (auto buffer : buffers) {
      if (buffer->name() == term.id()) {
        expval += std::real(term.coeff() * buffer->getExpectationValueZ());
        break;
      }
    }
  }

  return expval;
}

} // namespace algorithm
} // namespace xacc

REGISTER_ALGORITHM(xacc::algorithm::qEOM)