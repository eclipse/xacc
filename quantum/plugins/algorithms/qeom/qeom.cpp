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

    pool->optionalParameters({{"n-electrons", nOccupied}});
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
      auto M_MatOperator = doubleCommutator(bra, ket);
      if (!M_MatOperator->getSubTerms().empty()) {
        M(i, j) = measureOperator(M_MatOperator, buffer);
        M(j, i) = M(i, j);
      }

      auto Q_MatOperator = doubleCommutator(bra, ketConj);
      if (!Q_MatOperator->getSubTerms().empty()) {
        Q(i, j) = measureOperator(Q_MatOperator, buffer);
        Q(j, i) = Q(i, j);
      }

      auto V_MatOperator = bra->commutator(ket);
      if (!V_MatOperator->getSubTerms().empty()) {
        V(i, j) = measureOperator(V_MatOperator, buffer);
        V(j, i) = V(i, j);
      }

      auto W_MatOperator = bra->commutator(ketConj);
      if (!W_MatOperator->getSubTerms().empty()) {
        W(i, j) = measureOperator(W_MatOperator, buffer);
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

double
qEOM::measureOperator(const std::shared_ptr<Observable> obs,
                      const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // observe
  auto kernels = obs->observe(xacc::as_shared_ptr(kernel));

  // we loop over all measured circuits
  // and check if that term has been measured
  // if so, we just multiply the measurement by the coefficient
  // We gather all new circuits into fsToExec and execute
  // Because these are all commutators, we don't need to worry about the I term
  std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;
  std::vector<std::complex<double>> coefficients;
  double total = 0.0;
  for (auto &f : kernels) {
    std::complex<double> coeff = f->getCoefficient();
    if (cachedMeasurements.find(f->name()) != cachedMeasurements.end()) {
      total += std::real(coeff * cachedMeasurements[f->name()]);
    } else {
      fsToExec.push_back(f);
      coefficients.push_back(coeff);
    }
  }

  // for circuits that have not been executed previously
  auto tmpBuffer = xacc::qalloc(buffer->size());
  accelerator->execute(tmpBuffer, fsToExec);
  auto buffers = tmpBuffer->getChildren();
  for (int i = 0; i < fsToExec.size(); i++) {
    auto expval = buffers[i]->getExpectationValueZ();
    total += std::real(expval * coefficients[i]);
    cachedMeasurements.emplace(fsToExec[i]->name(), expval);
  }

  return total;
}

} // namespace algorithm
} // namespace xacc

REGISTER_ALGORITHM(xacc::algorithm::qEOM)