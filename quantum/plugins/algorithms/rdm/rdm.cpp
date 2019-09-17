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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "rdm.hpp"

#include "Observable.hpp"
#include "xacc.hpp"

#include "FermionOperator.hpp"
#include <unsupported/Eigen/CXX11/Tensor>
#include <unsupported/Eigen/CXX11/TensorSymmetry>

#include <memory>
using namespace xacc;

namespace xacc {
namespace algorithm {
bool RDM::initialize(const HeterogeneousMap &parameters) {
  if (!parameters.keyExists<std::shared_ptr<CompositeInstruction>>("ansatz")) {
    return false;
  } else if (!parameters.keyExists<std::shared_ptr<Accelerator>>("accelerator")) {
    return false;
  }

  ansatz = parameters.get<std::shared_ptr<CompositeInstruction>>("ansatz");
  accelerator = parameters.get<std::shared_ptr<Accelerator>>("accelerator");

  return true;
}

const std::vector<std::string> RDM::requiredParameters() const {
  return {"accelerator", "ansatz"};
}

void RDM::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  // Reset
  int nQubits = buffer->size(), nExecs = 0;

  Eigen::Tensor<std::complex<double>, 4> rho_pqrs(nQubits, nQubits, nQubits,
                                                  nQubits);
  rho_pqrs.setZero();

  // Get the Accelerator we're running on, the
  // number of orbitals/qubits in the problem,
  // the MPIProvider Communicator reference, and the
  // VQE state preparation ansatz.

  Eigen::DynamicSGroup rho_pq_Sym, rho_pqrs_Sym;
  rho_pq_Sym.addHermiticity(0, 1);
  rho_pqrs_Sym.addAntiSymmetry(0, 1);
  rho_pqrs_Sym.addAntiSymmetry(2, 3);

  // Map p,q,r,s indices to a coefficient for all
  // identity terms encountered
  std::map<std::vector<int>, double> rho_element_2_identity_coeff;

  // Map function names to the actual function and all
  // p,q,r,s and coefficients that contribute to that element
  std::map<std::string,
           std::pair<std::shared_ptr<CompositeInstruction>,
                     std::vector<std::pair<std::vector<int>, double>>>>
      functions;

  // Generate the 2-RDM circuits for executiong
  for (int m = 0; m < nQubits; m++) {
    for (int n = m + 1; n < nQubits; n++) {
      for (int v = m; v < nQubits; v++) {
        for (int w = v + 1; w < nQubits; w++) {
          std::stringstream xx;
          xx << "0.5 " << m << "^ " << n << "^ " << w << " " << v << " + "
             << "0.5 " << w << "^ " << v << "^ " << m << " " << n;
          auto op = std::make_shared<xacc::quantum::FermionOperator>(xx.str());
          auto hpqrs_ir = op->observe(ansatz);

          for (auto &kernel : hpqrs_ir) {
            auto t = std::real(
                kernel->getCoefficient());
            int nFunctionInstructions = 0;

            if (kernel->getInstruction(0)->isComposite()) {
              nFunctionInstructions =
                  ansatz->nInstructions() + kernel->nInstructions() - 1;
            } else {
              nFunctionInstructions = kernel->nInstructions();
            }

            if (nFunctionInstructions > ansatz->nInstructions()) {
              // if (kernel->nInstructions() > ansatz->nInstructions()) {
              auto name = kernel->name();
              if (functions.count(name)) {
                functions[name].second.push_back({{m, n, v, w}, t});
              } else {
                functions.insert({name, {kernel, {{{m, n, v, w}, t}}}});
              }
            } else {
              rho_element_2_identity_coeff.insert({{m, n, v, w}, t});
            }
          }
        }
      }
    }
  }

  std::vector<std::shared_ptr<CompositeInstruction>> fsToExecute;
  for (auto &kv : functions) {
    fsToExecute.push_back(kv.second.first);
  }

  int nPhysicalQubits =
      nQubits; //*std::max_element(qubitMap.begin(), qubitMap.end()) + 1;

  // Execute all nontrivial circuits
  xacc::info("[RDM] Executing " + std::to_string(fsToExecute.size()) +
             " circuits to compute rho_pqrs.");
  auto tmpBuffer = xacc::qalloc(buffer->size());
  accelerator->execute(tmpBuffer, fsToExecute);
  auto buffers = tmpBuffer->getChildren();

  bool useROExps = false;
  if (accelerator->name() == "ro-error") {
    useROExps = true;
  }

  // Create a mapping of rho_pqrs elements to summed expectation values for
  // each circuit contributing to it
  std::map<std::vector<int>, double> sumMap;
  for (int i = 0; i < buffers.size(); i++) {
    auto fName = fsToExecute[i]->name();
    auto p = functions[fName];
    std::vector<std::string> contributingIndices;
    std::vector<double> contributingCoeffs;
    for (auto &l : p.second) {
      auto elements = l.first;
      std::stringstream s;
      s << elements[0] << "," << elements[1] << "," << elements[2] << ","
        << elements[3];
      contributingIndices.push_back(s.str());
      double value;
      if (useROExps) {
        value = l.second * mpark::get<double>(buffers[i]->getInformation(
                               "ro-fixed-exp-val-z"));
      } else {
        value = l.second * buffers[i]->getExpectationValueZ();
      }
      contributingCoeffs.push_back(value);
      if (!sumMap.count(elements)) {
        sumMap.insert({elements, value});
      } else {
        sumMap[elements] += value;
      }
    }
    buffers[i]->addExtraInfo("kernel", ExtraInfo(fName));
    buffers[i]->addExtraInfo("contributing_rho_pqrs",
                             ExtraInfo(contributingIndices));
    buffers[i]->addExtraInfo("contributing_coeffs",
                             ExtraInfo(contributingCoeffs));
    buffer->appendChild(fName, buffers[i]);
  }

  // Add all identity terms, we don't execute them
  // but we still have to add their contribution
  for (auto &kv : rho_element_2_identity_coeff) {
    sumMap[kv.first] += kv.second;
  }

  // Set rho_pqrs. This is all we need
  // to get rho_pq as well
  for (auto &kv : sumMap) {
    auto elements = kv.first;
    rho_pqrs_Sym(rho_pqrs, elements[0], elements[1], elements[2], elements[3]) =
        kv.second;
    rho_pqrs_Sym(rho_pqrs, elements[2], elements[3], elements[0], elements[1]) =
        kv.second;
  }

  Eigen::Tensor<double, 4> realt = rho_pqrs.real();
  auto real = realt.data();
  std::vector<double> rho_pqrs_data(real, real + rho_pqrs.size());
  for (auto &a : rho_pqrs_data)
    if (std::fabs(a) < 1e-12)
      a = 0;

  buffer->addExtraInfo("2-rdm", ExtraInfo(rho_pqrs_data));
  return;
}
} // namespace algorithm
} // namespace xacc