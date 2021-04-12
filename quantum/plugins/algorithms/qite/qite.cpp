/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include "qite.hpp"

#include "Observable.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "PauliOperator.hpp"
#include "Circuit.hpp"
#include <memory>
#include <armadillo>
#include <cassert>

namespace {
const std::complex<double> I{0.0, 1.0};
int findMatchingPauliIndex(const std::vector<std::string> &in_OpList,
                           const std::string &in_obsStr) {
  // Returns true if the *operators* of the two terms are identical
  // e.g. a Z0X1 and b Z0X1 -> true
  const auto comparePauliString = [](const std::string &in_a,
                                     const std::string &in_b) -> bool {
    // Strip the coefficient part
    auto opA = in_a.substr(in_a.find_last_of(")") + 1);
    auto opB = in_b.substr(in_b.find_last_of(")") + 1);
    opA.erase(std::remove(opA.begin(), opA.end(), ' '), opA.end());
    opB.erase(std::remove(opB.begin(), opB.end(), ' '), opB.end());
    return opA == opB;
  };

  for (int i = 0; i < in_OpList.size(); ++i) {
    std::shared_ptr<xacc::Observable> obs =
        std::make_shared<xacc::quantum::PauliOperator>();
    const std::string pauliObsStr = "1.0 " + in_OpList[i];
    obs->fromString(pauliObsStr);

    if (comparePauliString(obs->toString(), in_obsStr)) {
      return i;
    }
  }
  // Failed!
  return -1;
}

// Project/flatten the target observable into the full list of all
// possible Pauli operator combinations.
// e.g. H = a X + b Z (1 qubit)
// -> { 0.0, a, 0.0, b } (the ordering is I, X, Y, Z)
std::vector<double>
observableToVec(std::shared_ptr<xacc::Observable> in_observable,
                const std::vector<std::string> &in_pauliObsList) {
  std::vector<double> obsProjCoeffs(in_pauliObsList.size(), 0.0);
  for (const auto &term : in_observable->getNonIdentitySubTerms()) {
    const auto index =
        findMatchingPauliIndex(in_pauliObsList, term->toString());
    assert(index >= 0);
    obsProjCoeffs[index] = term->coefficient().real();
  }
  return obsProjCoeffs;
};

// Helper to generate all permutation of Pauli observables:
// e.g.
// 1-qubit: I, X, Y, Z
// 2-qubit: II, IX, IY, IZ, XI, XX, XY, XZ, YI, YX, YY, YZ, ZI, ZX, ZY, ZZ
std::vector<std::string> generatePauliPermutation(int in_nbQubits) {
  assert(in_nbQubits > 0);
  const int nbPermutations = std::pow(4, in_nbQubits);
  std::vector<std::string> opsList;
  opsList.reserve(nbPermutations);

  const std::vector<std::string> pauliOps{"X", "Y", "Z"};
  const auto addQubitPauli = [&opsList, &pauliOps](int in_qubitIdx) {
    const auto currentOpListSize = opsList.size();
    for (int i = 0; i < currentOpListSize; ++i) {
      auto &currentOp = opsList[i];
      for (const auto &pauliOp : pauliOps) {
        const auto newOp = currentOp + pauliOp + std::to_string(in_qubitIdx);
        opsList.emplace_back(newOp);
      }
    }
  };

  opsList = {"", "X0", "Y0", "Z0"};
  for (int i = 1; i < in_nbQubits; ++i) {
    addQubitPauli(i);
  }

  assert(opsList.size() == nbPermutations);
  std::sort(opsList.begin(), opsList.end());

  return opsList;
};

arma::cx_mat createSMatrix(const std::vector<std::string> &in_pauliOps,
                           const std::vector<double> &in_tomoExp) {
  const auto sMatDim = in_pauliOps.size();
  arma::cx_mat S_Mat(sMatDim, sMatDim, arma::fill::zeros);
  arma::cx_vec b_Vec(sMatDim, arma::fill::zeros);

  const auto calcSmatEntry = [&](int in_row,
                                 int in_col) -> std::complex<double> {
    // Map the tomography expectation to the S matrix
    // S(i, j) = <psi|sigma_dagger(i)sigma(j)|psi>
    // sigma_dagger(i)sigma(j) will produce another Pauli operator with an
    // additional coefficient. e.g. sigma_x * sigma_y = i*sigma_z
    const auto leftOp = "1.0 " + in_pauliOps[in_row];
    const auto rightOp = "1.0 " + in_pauliOps[in_col];
    xacc::quantum::PauliOperator left(leftOp);
    xacc::quantum::PauliOperator right(rightOp);
    auto product = left * right;
    const auto index = findMatchingPauliIndex(in_pauliOps, product.toString());
    return in_tomoExp[index] * product.coefficient();
  };

  // S matrix:
  for (int i = 0; i < sMatDim; ++i) {
    for (int j = 0; j < sMatDim; ++j) {
      S_Mat(i, j) = calcSmatEntry(i, j);
    }
  }
  return S_Mat;
}

template <typename T> std::vector<T> arange(T start, T stop, T step = 1) {
  std::vector<T> values;
  for (T value = start; value < stop; value += step) {
    values.emplace_back(value);
  }
  return values;
}
} // namespace

using namespace xacc;
namespace xacc {
namespace algorithm {
bool QITE::initialize(const HeterogeneousMap &parameters) {
  static std::stringstream nullStream;
  // Prevents Armadillo from writing expected 'approx. solution' warnings.
  arma::set_cerr_stream(nullStream);
  bool initializeOk = true;
  if (!parameters.pointerLikeExists<Accelerator>("accelerator")) {
    std::cout << "'accelerator' is required.\n";
    initializeOk = false;
  }

  if (parameters
          .keyExists<std::vector<std::shared_ptr<xacc::IRTransformation>>>(
              "circuit-optimizers")) {
    custom_optimizers =
        parameters.get<std::vector<std::shared_ptr<xacc::IRTransformation>>>(
            "circuit-optimizers");
  }

  if (parameters.keyExists<std::shared_ptr<xacc::IRTransformation>>(
          "circuit-optimizer")) {
    custom_optimizers.push_back(
        parameters.get<std::shared_ptr<xacc::IRTransformation>>(
            "circuit-optimizer"));
  }

  if (!parameters.keyExists<int>("steps")) {
    std::cout << "'steps' is required.\n";
    initializeOk = false;
  }

  if (!parameters.keyExists<double>("step-size")) {
    std::cout << "'step-size' is required.\n";
    initializeOk = false;
  }

  if (!parameters.pointerLikeExists<Observable>("observable")) {
    std::cout << "'observable' is required.\n";
    initializeOk = false;
  }

  if (initializeOk) {
    m_accelerator = xacc::as_shared_ptr(
        parameters.getPointerLike<Accelerator>("accelerator"));
    m_nbSteps = parameters.get<int>("steps");
    m_dBeta = parameters.get<double>("step-size");
    m_observable = xacc::as_shared_ptr(
        parameters.getPointerLike<Observable>("observable"));
  }

  m_analytical = false;
  if (parameters.keyExists<bool>("analytical")) {
    m_analytical = parameters.get<bool>("analytical");
    if (m_analytical) {
      // Default initial state is 0
      m_initialState = 0;
      if (parameters.keyExists<int>("initial-state")) {
        m_initialState = parameters.get<int>("initial-state");
      }
    }
  }

  m_ansatz = nullptr;
  // Ansatz here is just a state preparation circuit:
  // e.g. if we want to start in state |01>, not |00>
  if (parameters.pointerLikeExists<CompositeInstruction>("ansatz")) {
    m_ansatz = parameters.getPointerLike<CompositeInstruction>("ansatz");
  }

  m_approxOps.clear();
  m_energyAtStep.clear();

  input_parameters = parameters;
  return initializeOk;
}

const std::vector<std::string> QITE::requiredParameters() const {
  return {"accelerator", "steps", "step-size", "observable"};
}

std::shared_ptr<CompositeInstruction> QITE::constructPropagateCircuit() const {
  auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
  auto propagateKernel = gateRegistry->createComposite("statePropCircuit");

  // Adds ansatz if provided
  if (m_ansatz) {
    propagateKernel->addInstructions(m_ansatz->getInstructions());
  }

  const auto pauliTermToString =
      [](const std::shared_ptr<xacc::Observable> &in_term) {
        std::string pauliTermStr = in_term->toString();
        std::stringstream s;
        s.precision(12);
        s << std::fixed << in_term->coefficient();
        // Find the parenthesis
        const auto startPosition = pauliTermStr.find("(");
        const auto endPosition = pauliTermStr.find(")");

        if (startPosition != std::string::npos &&
            endPosition != std::string::npos) {
          const auto length = endPosition - startPosition + 1;
          pauliTermStr.replace(startPosition, length, s.str());
        }
        return pauliTermStr;
      };

  // Progagates by Trotter steps
  // Using those A operators that have been
  // optimized up to this point.
  for (const auto &aObs : m_approxOps) {
    // Circuit is: exp(-idt*A),
    // i.e. regular evolution which approximates the imaginary time evolution.
    for (const auto &term : aObs->getNonIdentitySubTerms()) {
      const auto pauliStr = pauliTermToString(term);
      auto expCirc = std::dynamic_pointer_cast<xacc::quantum::Circuit>(
          xacc::getService<Instruction>("exp_i_theta"));
      const bool expandOk =
          expCirc->expand({std::make_pair("pauli", pauliStr)});
      assert(expandOk);
      auto evaled = expCirc->operator()({0.5 * m_dBeta});
      propagateKernel->addInstructions(evaled->getInstructions());
    }
  }

  // std::cout << "Progagated kernel:\n" << propagateKernel->toString() << "\n";
  return propagateKernel;
}

double
QITE::calcCurrentEnergy(int in_nbQubits, double in_identityCoeff,
                        const std::vector<double> &in_coefficients,
                        const std::vector<std::shared_ptr<AcceleratorBuffer>>
                            &in_resultBuffers) const {
  double energy = in_identityCoeff;
  for (int i = 0; i < in_resultBuffers.size(); ++i) {
    auto expval = in_resultBuffers[i]->getExpectationValueZ();
    energy += expval * in_coefficients[i];
  }
  xacc::info("Energy = " + std::to_string(energy));
  return energy;
}

double QITE::calculate(const std::string &calculation_task,
                       const std::shared_ptr<AcceleratorBuffer> buffer,
                       const HeterogeneousMap &extra_data) {
  // Calculate A-ops
  if (calculation_task == "approximate-ops") {
    if (!extra_data.keyExists<std::vector<std::string>>("pauli-ops") ||
        !extra_data.keyExists<std::vector<double>>("pauli-ops-exp-val")) {
      xacc::error("[QITE::calculate] Cannot calculate approximate-ops without "
                  "'pauli-ops' and 'pauli-ops-exp-val' vectors.");
    }

    const auto pauliOps = extra_data.get<std::vector<std::string>>("pauli-ops");
    const auto sigmaExpectation =
        extra_data.get<std::vector<double>>("pauli-ops-exp-val");
    if (pauliOps.size() != sigmaExpectation.size()) {
      xacc::error("[QITE::calculate]: 'pauli-ops' and 'pauli-ops-exp-val' "
                  "vectors must have the same length.");
    }

    auto hamOp = std::make_shared<xacc::quantum::PauliOperator>();
    for (const auto &hamTerm : m_observable->getNonIdentitySubTerms()) {
      *hamOp =
          *hamOp +
          *(std::dynamic_pointer_cast<xacc::quantum::PauliOperator>(hamTerm));
    }
    auto [normVal, nextAOps] =
        internalCalcAOps(pauliOps, sigmaExpectation, hamOp);
    buffer->addExtraInfo("Aops-str", nextAOps->toString());
    return normVal;
  }
  return 0.0;
}

std::pair<double, std::shared_ptr<Observable>>
QITE::internalCalcAOps(const std::vector<std::string> &pauliOps,
                       const std::vector<double> &sigmaExpectation,
                       std::shared_ptr<Observable> in_hmTerm) const {
  // Calculate S matrix and b vector
  // i.e. set up the linear equation Sa = b
  const auto sMatDim = pauliOps.size();
  arma::cx_mat S_Mat(sMatDim, sMatDim, arma::fill::zeros);
  arma::cx_vec b_Vec(sMatDim, arma::fill::zeros);

  const auto calcSmatEntry = [&](const std::vector<double> &in_tomoExp,
                                 int in_row,
                                 int in_col) -> std::complex<double> {
    // Map the tomography expectation to the S matrix
    // S(i, j) = <psi|sigma_dagger(i)sigma(j)|psi>
    // sigma_dagger(i)sigma(j) will produce another Pauli operator with an
    // additional coefficient. e.g. sigma_x * sigma_y = i*sigma_z
    const auto leftOp = "1.0 " + pauliOps[in_row];
    const auto rightOp = "1.0 " + pauliOps[in_col];
    xacc::quantum::PauliOperator left(leftOp);
    xacc::quantum::PauliOperator right(rightOp);
    auto product = left * right;
    const auto index = findMatchingPauliIndex(pauliOps, product.toString());
    return in_tomoExp[index] * product.coefficient();
  };

  // S matrix:
  for (int i = 0; i < sMatDim; ++i) {
    for (int j = 0; j < sMatDim; ++j) {
      const auto entry = calcSmatEntry(sigmaExpectation, i, j);
      S_Mat(i, j) = std::abs(entry) > 1e-12 ? entry : 0.0;
    }
  }

  // b vector:
  const auto obsProjCoeffs = observableToVec(in_hmTerm, pauliOps);

  // Calculate c: Eq. 3 in https://arxiv.org/pdf/1901.07653.pdf
  double c = 1.0;
  for (int i = 0; i < obsProjCoeffs.size(); ++i) {
    c -= 2.0 * m_dBeta * obsProjCoeffs[i] * sigmaExpectation[i];
  }

  for (int i = 0; i < sMatDim; ++i) {
    std::complex<double> b =
        (sigmaExpectation[i] / std::sqrt(c) - sigmaExpectation[i]) / m_dBeta;
    for (int j = 0; j < obsProjCoeffs.size(); ++j) {
      // The expectation of the pauli product of the Hamiltonian term
      // and the sweeping pauli term.
      const auto expectVal = calcSmatEntry(sigmaExpectation, i, j);
      b -= obsProjCoeffs[j] * expectVal / std::sqrt(c);
    }
    b = I * b - I * std::conj(b);
    // Set b_Vec
    b_Vec(i) = std::abs(b) > 1e-12 ? b : 0.0;
  }

  auto lhs = S_Mat + S_Mat.st();
  auto rhs = -b_Vec;
  arma::cx_vec a_Vec = arma::solve(lhs, rhs);

  // Now, we have the decomposition of A observable in the basis of
  // all possible Pauli combinations.
  assert(a_Vec.n_elem == pauliOps.size());
  const std::string aObsStr = [&]() {
    std::stringstream s;
    s.precision(12);
    s << std::fixed << a_Vec(0);

    for (int i = 1; i < pauliOps.size(); ++i) {
      s << " + " << (-2.0 * a_Vec(i)) << " " << pauliOps[i];
    }

    return s.str();
  }();

  // Compute the approximate A observable/Hamiltonian.
  // This operator will drive the exp_i_theta evolution
  // which emulate the imaginary time evolution of the original observable.
  std::shared_ptr<Observable> updatedAham =
      std::make_shared<xacc::quantum::PauliOperator>();
  updatedAham->fromString(aObsStr);
  return std::make_pair(std::sqrt(c), updatedAham);
}

std::tuple<double, double, std::shared_ptr<Observable>>
QITE::calcQiteEvolve(const std::shared_ptr<AcceleratorBuffer> &in_buffer,
                     std::shared_ptr<CompositeInstruction> in_kernel,
                     std::shared_ptr<Observable> in_hmTerm,
                     bool energyOnly) const {
  std::vector<std::shared_ptr<CompositeInstruction>> fsToExec;

  if (!custom_optimizers.empty()) {
    for (auto opt : custom_optimizers) {
      xacc::info("Applying " + opt->name() + " Optimizer");
      opt->apply(in_kernel, m_accelerator, input_parameters);
    }
  }
  
  // First, create the sub-circuits to evaluate current energy values:
  auto kernels = m_observable->observe(in_kernel);
  std::vector<double> coefficients;
  std::vector<std::string> kernelNames;
  double identityCoeff = 0.0;
  for (auto &f : kernels) {
    kernelNames.push_back(f->name());
    std::complex<double> coeff = f->getCoefficient();
    int nFunctionInstructions = 0;
    if (f->nInstructions() > 0 && f->getInstruction(0)->isComposite()) {
      nFunctionInstructions =
          in_kernel->nInstructions() + f->nInstructions() - 1;
    } else {
      nFunctionInstructions = f->nInstructions();
    }

    if (nFunctionInstructions > in_kernel->nInstructions()) {
      fsToExec.push_back(f);
      coefficients.push_back(std::real(coeff));
    } else {
      identityCoeff += std::real(coeff);
    }
  }
  // Cache the number of kernels for energy evaluation:
  const size_t nbEnergyKernels = fsToExec.size();
  // If we only need to evaluate the energy: no need any Pauli operators.
  const auto pauliOps = energyOnly
                            ? std::vector<std::string>{""}
                            : generatePauliPermutation(in_buffer->size());

  // Observe the kernels using the various Pauli
  // operators to calculate S and b.
  std::vector<double> sigmaExpectation;
  // Identity observable:
  sigmaExpectation.emplace_back(1.0);
  for (int i = 1; i < pauliOps.size(); ++i) {
    std::shared_ptr<Observable> tomoObservable =
        std::make_shared<xacc::quantum::PauliOperator>();
    const std::string pauliObsStr = "1.0 " + pauliOps[i];
    tomoObservable->fromString(pauliObsStr);
    assert(tomoObservable->getSubTerms().size() == 1);
    assert(tomoObservable->getNonIdentitySubTerms().size() == 1);
    auto obsKernel = tomoObservable->observe(in_kernel).front();
    // Add tomography kernels:
    fsToExec.emplace_back(obsKernel);
  }

  // Execute all kernels:
  auto tmpBuffer = xacc::qalloc(in_buffer->size());
  m_accelerator->execute(tmpBuffer, fsToExec);
  // Get child buffers:
  auto childBuffers = tmpBuffer->getChildren();
  std::vector<std::shared_ptr<AcceleratorBuffer>> energyBuffers;
  // Get energy buffers:
  std::copy_n(childBuffers.begin(), nbEnergyKernels,
              std::back_inserter(energyBuffers));
  assert(energyBuffers.size() == nbEnergyKernels);

  // Get tomography buffers:
  std::vector<std::shared_ptr<AcceleratorBuffer>> tommoBuffers;
  std::copy_n(childBuffers.begin() + nbEnergyKernels, pauliOps.size() - 1,
              std::back_inserter(tommoBuffers));

  // Process buffer results:
  const double currentEnergy = calcCurrentEnergy(
      in_buffer->size(), identityCoeff, coefficients, energyBuffers);
  if (energyOnly) {
    assert(childBuffers.size() == nbEnergyKernels);
    return std::make_tuple(currentEnergy, 0.0, nullptr);
  }

  for (auto &childBuffer : tommoBuffers) {
    const auto expval = childBuffer->getExpectationValueZ();
    sigmaExpectation.emplace_back(expval);
  }
  assert(sigmaExpectation.size() == pauliOps.size());
  auto [norm, Aops] = internalCalcAOps(pauliOps, sigmaExpectation, in_hmTerm);
  return std::make_tuple(currentEnergy, norm, Aops);
}

void QITE::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {
  if (!m_analytical) {
    // Run on hardware/simulator using quantum gates/measure
    auto hamOp = std::make_shared<xacc::quantum::PauliOperator>();
    for (const auto &hamTerm : m_observable->getNonIdentitySubTerms()) {
      *hamOp =
          *hamOp +
          *(std::dynamic_pointer_cast<xacc::quantum::PauliOperator>(hamTerm));
    }

    // Time stepping:
    for (int i = 0; i < m_nbSteps; ++i) {
      // Propagates the state via Trotter steps:
      auto kernel = constructPropagateCircuit();
      // Optimizes/calculates next A ops
      auto [energyVal, normVal, nextAOps] =
          calcQiteEvolve(buffer, kernel, hamOp);
      // The energy at this step (before adding the newly calculated A-op)
      m_energyAtStep.emplace_back(energyVal);
      m_approxOps.emplace_back(nextAOps);
    }

    // We need to execute an extra call to evaluate the end energy:
    auto finalKernel = constructPropagateCircuit();
    auto [energyVal, normVal, nextAOps] =
        calcQiteEvolve(buffer, finalKernel, hamOp, true);
    assert(nextAOps == nullptr);
    m_energyAtStep.emplace_back(energyVal);
    assert(m_energyAtStep.size() == m_nbSteps + 1);
    // Last energy value
    buffer->addExtraInfo("opt-val", ExtraInfo(m_energyAtStep.back()));
    // Also returns the full list of energy values
    // at each Trotter step.
    buffer->addExtraInfo("exp-vals", ExtraInfo(m_energyAtStep));

    // Final kernel:
    auto staq = xacc::getCompiler("staq");
    const auto openQASMSrc = staq->translate(finalKernel);
    // Returns the QITE circuit as a QASM string:
    buffer->addExtraInfo("qasm", ExtraInfo(openQASMSrc));
  } else {
    // Analytical run:
    // This serves two purposes:
    // (1) Validate the convergence (e.g. Trotter step size) before running via
    // gates. (2) Derive the circuit analytically for running. exp(-dtH)
    const auto expMinusHamTerm = [](const arma::cx_mat &in_hMat,
                                    const arma::cx_vec &in_psi, double in_dt) {
      assert(in_hMat.n_rows == in_hMat.n_cols);
      assert(in_hMat.n_rows == in_psi.n_elem);
      arma::cx_mat hMatExp = arma::expmat(-in_dt * in_hMat);
      arma::cx_vec result = hMatExp * in_psi;
      const double norm = arma::norm(result, 2);
      result = result / norm;
      return std::make_pair(result, norm);
    };

    const auto getTomographyExpVec = [](int in_nbQubits,
                                        const arma::cx_vec &in_psi,
                                        const arma::cx_vec &in_delta) {
      const auto pauliOps = generatePauliPermutation(in_nbQubits);
      std::vector<std::complex<double>> sigmaExpectation(pauliOps.size());
      std::vector<std::complex<double>> bVec(pauliOps.size());

      sigmaExpectation[0] = 1.0;
      bVec[0] = arma::cdot(in_delta, in_psi);

      for (int i = 1; i < pauliOps.size(); ++i) {
        auto tomoObservable = std::make_shared<xacc::quantum::PauliOperator>();
        const std::string pauliObsStr = "1.0 " + pauliOps[i];
        tomoObservable->fromString(pauliObsStr);
        assert(tomoObservable->getSubTerms().size() == 1);
        assert(tomoObservable->getNonIdentitySubTerms().size() == 1);
        arma::cx_mat hMat(1 << in_nbQubits, 1 << in_nbQubits,
                          arma::fill::zeros);
        const auto hamMat = tomoObservable->toDenseMatrix(in_nbQubits);
        for (int i = 0; i < hMat.n_rows; ++i) {
          for (int j = 0; j < hMat.n_cols; ++j) {
            const int index = i * hMat.n_rows + j;
            hMat(i, j) = hamMat[index];
          }
        }

        arma::cx_vec pauliApplied = hMat * in_psi;
        sigmaExpectation[i] = arma::cdot(in_psi, pauliApplied);
        bVec[i] = arma::cdot(in_delta, pauliApplied);
      }

      return std::make_pair(sigmaExpectation, bVec);
    };

    // Initial state
    arma::cx_vec psiVec(1 << buffer->size(), arma::fill::zeros);
    psiVec(m_initialState) = 1.0;
    arma::cx_mat hMat(1 << buffer->size(), 1 << buffer->size(),
                      arma::fill::zeros);
    auto identityTerm = m_observable->getIdentitySubTerm();
    if (identityTerm) {
      arma::cx_mat idTerm(1 << buffer->size(), 1 << buffer->size(),
                          arma::fill::eye);
      hMat += identityTerm->coefficient() * idTerm;
    }

    for (const auto &hamTerm : m_observable->getNonIdentitySubTerms()) {
      auto pauliCast =
          std::dynamic_pointer_cast<xacc::quantum::PauliOperator>(hamTerm);
      const auto hamMat = pauliCast->toDenseMatrix(buffer->size());

      for (int i = 0; i < hMat.n_rows; ++i) {
        for (int j = 0; j < hMat.n_cols; ++j) {
          const int index = i * hMat.n_rows + j;
          hMat(i, j) += hamMat[index];
        }
      }
    }

    // Time stepping:
    for (int i = 0; i < m_nbSteps; ++i) {
      double normAfter = 0.0;
      arma::cx_vec dPsiVec(1 << buffer->size(), arma::fill::zeros);
      std::tie(dPsiVec, normAfter) = expMinusHamTerm(hMat, psiVec, m_dBeta);
      // Eq. 8, SI of https://arxiv.org/pdf/1901.07653.pdf
      dPsiVec = dPsiVec - psiVec;
      std::vector<std::complex<double>> pauliExp;
      std::vector<std::complex<double>> bVec;
      std::tie(pauliExp, bVec) =
          getTomographyExpVec(buffer->size(), psiVec, dPsiVec);
      std::vector<double> pauliExpValues;
      for (const auto &val : pauliExp) {
        pauliExpValues.emplace_back(val.real());
      }

      arma::cx_mat sMat = createSMatrix(
          generatePauliPermutation(buffer->size()), pauliExpValues);
      arma::cx_vec b_Vec(bVec.size(), arma::fill::zeros);
      for (int i = 0; i < bVec.size(); ++i) {
        b_Vec(i) = -I * bVec[i] + I * std::conj(bVec[i]);
      }

      auto lhs = sMat + sMat.st();
      auto rhs = b_Vec;
      arma::cx_vec a_Vec = arma::solve(lhs, rhs);
      const auto pauliOps = generatePauliPermutation(buffer->size());
      const std::string aObsStr = [&]() {
        std::stringstream s;
        s.precision(12);
        s << std::fixed << a_Vec(0);

        for (int i = 1; i < pauliOps.size(); ++i) {
          s << " + " << a_Vec(i) << " " << pauliOps[i];
        }

        return s.str();
      }();

      std::shared_ptr<xacc::quantum::PauliOperator> updatedAham =
          std::make_shared<xacc::quantum::PauliOperator>();
      updatedAham->fromString(aObsStr);
      const auto aHamMat = updatedAham->toDenseMatrix(buffer->size());
      arma::cx_mat aMat(1 << buffer->size(), 1 << buffer->size(),
                        arma::fill::zeros);

      for (int i = 0; i < aMat.n_rows; ++i) {
        for (int j = 0; j < aMat.n_cols; ++j) {
          const int index = i * aMat.n_rows + j;
          aMat(i, j) = aHamMat[index];
        }
      }

      // Evolve exp(-iAt)
      arma::cx_mat aMatExp = arma::expmat(-I * aMat);
      arma::cx_mat psiUpdate = aMatExp * psiVec;
      psiVec = psiUpdate;
      const std::complex<double> energyRaw =
          arma::cdot(psiUpdate, hMat * psiUpdate);
      std::cout << "Energy = " << energyRaw << "\n";
      m_energyAtStep.emplace_back(energyRaw.real());
      // First step: add the approximate operator info to the buffer.
      // Users can use this analytical solver to compute the A operator:
      // e.g. for deuteron problems, we can recover the UCC ansatz by QITE.
      if (i == 0) {
        buffer->addExtraInfo("A-op", updatedAham->toString());
      }
    }

    m_energyAtStep.emplace_back(m_energyAtStep.back());
    assert(m_energyAtStep.size() == m_nbSteps + 1);
    // Last energy value
    buffer->addExtraInfo("opt-val", ExtraInfo(m_energyAtStep.back()));
    // Also returns the full list of energy values
    // at each Trotter step.
    buffer->addExtraInfo("exp-vals", ExtraInfo(m_energyAtStep));
  }
}

std::vector<double>
QITE::execute(const std::shared_ptr<AcceleratorBuffer> buffer,
              const std::vector<double> &x) {
  // We don't do any parameter optimization here,
  // hence don't support this!
  xacc::error("This method is unsupported!");
  return {};
}

bool QLanczos::initialize(const HeterogeneousMap &parameters) {
  // Calls the base (QITE) initialization
  if (QITE::initialize(parameters)) {
    // Gets some QLanczos-specific parameters if provided:
    // Default params:
    m_sLim = 0.75;
    m_epsLim = 1e-12;
    if (parameters.keyExists<double>("s-param")) {
      m_sLim = parameters.get<double>("s-param");
    }
    if (parameters.keyExists<double>("epsilon-param")) {
      m_epsLim = parameters.get<double>("epsilon-param");
    }

    return true;
  }

  return false;
}

void QLanczos::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {
  std::vector<double> normAtStep;
  std::vector<double> lanczosEnergy;
  // Run on hardware/simulator using quantum gates/measure
  // Initial norm
  normAtStep.emplace_back(1.0);
  auto hamOp = std::make_shared<xacc::quantum::PauliOperator>();
  for (const auto &hamTerm : m_observable->getNonIdentitySubTerms()) {
    *hamOp =
        *hamOp +
        *(std::dynamic_pointer_cast<xacc::quantum::PauliOperator>(hamTerm));
  }

  // Time stepping:
  for (int i = 0; i < m_nbSteps; ++i) {
    // Propagates the state via Trotter steps:
    auto kernel = constructPropagateCircuit();
    // Optimizes/calculates next A ops
    auto [energyVal, normVal, nextAOps] = calcQiteEvolve(buffer, kernel, hamOp);
    m_approxOps.emplace_back(nextAOps);
    m_energyAtStep.emplace_back(energyVal);
    // Odd steps (back processing):
    if ((i % 2) == 1) {
      lanczosEnergy.emplace_back(calcQlanczosEnergy(normAtStep));
    }
    normAtStep.emplace_back(normVal * normAtStep.back());
  }
  auto finalKernel = constructPropagateCircuit();
  auto [energyVal, normVal, nextAOps] =
      calcQiteEvolve(buffer, finalKernel, hamOp, true);
  assert(nextAOps == nullptr);
  m_energyAtStep.emplace_back(energyVal);
  assert(m_energyAtStep.size() == m_nbSteps + 1);
  lanczosEnergy.emplace_back(calcQlanczosEnergy(normAtStep));

  // Last QLanczos energy value
  buffer->addExtraInfo("opt-val", ExtraInfo(lanczosEnergy.back()));
  // Also returns the full list of energy values
  // at each QLanczos step.
  buffer->addExtraInfo("exp-vals", ExtraInfo(lanczosEnergy));
}

double QLanczos::calcQlanczosEnergy(const std::vector<double> &normVec) const {
  const std::vector<size_t> lanczosSteps =
      arange(1UL, m_energyAtStep.size() + 1, 2UL);
  const auto n = lanczosSteps.size();
  // H and S matrices (Eq. 60)
  arma::mat H(n, n, arma::fill::zeros);
  arma::mat S(n, n, arma::fill::zeros);
  int j = 0;
  int k = 0;
  // Iterate over l and l'
  for (const auto &l : lanczosSteps) {
    int k = 0;
    for (const auto &lp : lanczosSteps) {
      // Defining 2r = l + l'
      const auto r = (l + lp) / 2;
      // Eq. 61
      S(j, k) = normVec[r] * normVec[r] / (normVec[l] * normVec[lp]);
      // Eq. 62
      H(j, k) = m_energyAtStep[r] * S(j, k);
      k++;
    }
    j++;
  }

  const auto matFieldSampling = [](const arma::mat &in_H, const arma::mat &in_S,
                                   const std::vector<size_t> in_indexVec) {
    arma::mat H(in_indexVec.size(), in_indexVec.size(), arma::fill::zeros);
    arma::mat S(in_indexVec.size(), in_indexVec.size(), arma::fill::zeros);
    assert(in_S.n_cols == in_H.n_cols && in_S.n_rows == in_H.n_rows);
    for (size_t i = 0; i < in_H.n_rows; ++i) {
      for (size_t j = 0; j < in_H.n_cols; ++j) {
        if (xacc::container::contains(in_indexVec, i) &&
            xacc::container::contains(in_indexVec, j)) {
          const size_t rowIdx = std::distance(
              in_indexVec.begin(),
              std::find(in_indexVec.begin(), in_indexVec.end(), i));
          const size_t colIdx = std::distance(
              in_indexVec.begin(),
              std::find(in_indexVec.begin(), in_indexVec.end(), j));
          S(rowIdx, colIdx) = in_S(i, j);
          H(rowIdx, colIdx) = in_H(i, j);
        }
      }
    }

    return std::make_pair(H, S);
  };

  // Regularize/stabilize H and S matrices with s and epsilon stabilization
  // parameters. Returns the stabilized (H, S) matrices.
  const auto regularizeMatrices =
      [&](double s, double eps) -> std::pair<arma::mat, arma::mat> {
    std::vector<size_t> indexVec{0};
    size_t ii = 0;
    size_t jj = 0;
    while (ii < H.n_rows && jj < (H.n_rows - 1)) {
      for (jj = ii + 1; jj < H.n_rows; ++jj) {
        if (S(ii, jj) < s) {
          indexVec.emplace_back(jj);
          break;
        }
      }

      ii = indexVec.back();
    }
    if (!xacc::container::contains(indexVec, H.n_rows - 1)) {
      indexVec.emplace_back(H.n_rows - 1);
    }

    auto [Hnew, Snew] = matFieldSampling(H, S, indexVec);

    // Handles an edge case where Hnew and Snew are just single-element
    // matrices; just returns those matrices.
    if (indexVec.size() == 1) {
      assert(Hnew.n_elem == 1 && Snew.n_elem == 1);
      // Just returns these matrices,
      // no need to regularize any further.
      return std::make_pair(Hnew, Snew);
    }

    // Truncates eigenvalues if less than epsilon
    arma::vec sigma;
    arma::mat V;
    arma::eig_sym(sigma, V, Snew);
    std::vector<size_t> indexVecEig;

    for (size_t i = 0; i < sigma.n_elem; ++i) {
      if (sigma[i] > eps) {
        indexVecEig.emplace_back(i);
      }
    }

    const auto eigenTransform = [](const arma::mat &in_mat,
                                   const arma::mat &in_eigenMat) {
      // Performs V_T * Matrix * V;
      // where V is the eigenvector matrix.
      arma::mat result = in_eigenMat.t() * in_mat;
      result = result * in_eigenMat;
      return result;
    };
    const arma::mat Snew2 = eigenTransform(Snew, V);
    const arma::mat Hnew2 = eigenTransform(Hnew, V);
    auto [Hnew3, Snew3] = matFieldSampling(Hnew2, Snew2, indexVecEig);
    return std::make_pair(Hnew3, Snew3);
  };

  auto [Hreg, Sreg] = regularizeMatrices(m_sLim, m_epsLim);

  arma::cx_vec eigval;
  arma::cx_mat eigvec;
  // Solves the generalized eigen val
  arma::eig_pair(eigval, eigvec, Hreg, Sreg);
  std::vector<double> energies;
  for (size_t i = 0; i < eigval.n_elem; ++i) {
    // Energy values should be real
    assert(std::abs(eigval(i).imag()) < 1e-9);
    energies.emplace_back(eigval(i).real());
  }
  std::sort(energies.begin(), energies.end());
  // Returns the smallest eigenvalue from the previous step.
  return energies.front();
}

} // namespace algorithm
} // namespace xacc