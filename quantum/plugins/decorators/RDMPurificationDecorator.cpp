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
#include "RDMPurificationDecorator.hpp"
#include "IRProvider.hpp"
#include "InstructionIterator.hpp"
#include "xacc.hpp"
#include "FermionOperator.hpp"
#include "PauliOperator.hpp"

#include <unsupported/Eigen/CXX11/TensorSymmetry>
#include <iomanip>

namespace xacc {
namespace quantum {
using IP = Eigen::IndexPair<int>;
using T2 = Eigen::Tensor<double, 2>;
using T4 = Eigen::Tensor<double, 4>;
using T0 = Eigen::Tensor<double, 0>;

void RDMPurificationDecorator::initialize(const HeterogeneousMap &params) {
  if (!params.keyExists<std::shared_ptr<Observable>>("fermion-observable") &&
      !std::dynamic_pointer_cast<quantum::FermionOperator>(
          params.get<std::shared_ptr<Observable>>("fermion-observable"))) {
    xacc::error(
        "You must provide a FermionObservable (with key fermion-observable) as "
        "initialization input for the RDM Purification Decorator.");
  }

  fermionObservable =
      params.get<std::shared_ptr<Observable>>("fermion-observable");
}

void RDMPurificationDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> function) {

  if (!decoratedAccelerator) {
    xacc::error("Null Decorated Accelerator Error");
  }

  return;
}

void RDMPurificationDecorator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {

  std::vector<std::shared_ptr<AcceleratorBuffer>> buffers;
  if (!decoratedAccelerator) {
    xacc::error("RDMPurificationDecorator - Null Decorated Accelerator Error");
  }

  // Here I expect the ansatz to be functions[0]->getInstruction(0);
  auto ansatz = std::dynamic_pointer_cast<CompositeInstruction>(
      functions[0]->getInstruction(0));
  if (!ansatz)
    xacc::error("RDMPurificationDecorator - Ansatz was null.");

  auto nQubits = buffer->size();
  //   std::vector<int> qubitMap(nQubits);
  //   std::iota (std::begin(qubitMap), std::end(qubitMap), 0);

  //   // optionally map the ansatz to a
  //   // different set of physical qubits
  //   if (xacc::optionExists("rdm-qubit-map")) {
  //     auto provider = xacc::getService<IRProvider>("gate");
  //     auto mapStr = xacc::getOption("rdm-qubit-map");

  //     std::vector<std::string> split = xacc::split(mapStr, ',');
  //     // boost::split(split, mapStr, boost::is_any_of(","));

  //     qubitMap.clear();
  //     for (auto s : split) {
  //       auto idx = std::stoi(s);
  //       qubitMap.push_back(idx);
  //     }
  //     // ansatz = ansatz->enabledView();
  //     auto tmp = provider->createFunction(ansatz->name(), ansatz->bits());
  //     for (auto& i : ansatz->getInstructions()) {
  //         auto cloned = provider->createInstruction(i->name(), i->bits(),
  //         i->getParameters()); tmp->addInstruction(cloned);
  //     }
  //     tmp->mapBits(qubitMap);
  //     ansatz = tmp;
  //   }

  Eigen::Tensor<double, 2> hpq(nQubits, nQubits);
  hpq.setZero();
  Eigen::Tensor<double, 4> hpqrs(nQubits, nQubits, nQubits, nQubits);
  hpqrs.setZero();

  double enuc = 0.0;
  auto terms =
      std::dynamic_pointer_cast<quantum::FermionOperator>(fermionObservable)
          ->getTerms();
  for (auto &kv : terms) {
    auto ops = kv.second.ops();
    if (ops.size() == 4) {
      hpqrs(ops[0].first, ops[1].first, ops[2].first, ops[3].first) =
          std::real(kv.second.coeff());
    } else if (ops.size() == 2) {
      hpq(ops[0].first, ops[1].first) = std::real(kv.second.coeff());
    } else {
      enuc = std::real(kv.second.coeff());
    }
  }

  double energy = enuc;

  auto rdmGen = xacc::getAlgorithm(
      "rdm", {std::make_pair("accelerator", decoratedAccelerator),
              std::make_pair("ansatz", ansatz)});
  rdmGen->execute(buffer);
  buffers = buffer->getChildren();

  auto data = buffer->getInformation("2-rdm").as<std::vector<double>>();

  Eigen::TensorMap<Eigen::Tensor<double, 4>> rho_pqrs(
      data.data(), nQubits, nQubits, nQubits, nQubits);

  Eigen::Tensor<double, 4> realpqrs = hpqrs.real();
  auto pqrstmp = realpqrs.data();
  std::vector<double> hpqrs_vec(pqrstmp, pqrstmp + hpqrs.size());

  Eigen::Tensor<double, 2> realpq = hpq.real();
  auto pqtmp = realpq.data();
  std::vector<double> hpq_vec(pqtmp, pqtmp + hpq.size());

  //   //   auto rho_pq = generator.rho_pq;
  //   //   T4 rho_pqrs = generator.rho_pqrs;
  Eigen::Tensor<double, 4> realt = rho_pqrs.real();
  auto real = realt.data();
  std::vector<double> rho_pqrs_data(real, real + rho_pqrs.size());

  double bad_energy = energy;
  for (int p = 0; p < nQubits; p++) {
    for (int q = 0; q < nQubits; q++) {
      for (int r = 0; r < nQubits; r++) {
        for (int s = 0; s < nQubits; s++) {
          bad_energy +=
              0.5 * std::real(hpqrs(p, q, r, s) *
                              (rho_pqrs(p, q, s, r) + rho_pqrs(r, s, q, p)));
        }
      }
    }
  }

  Eigen::array<int, 2> cc2({1, 3});
  Eigen::DynamicSGroup rho_pqrs_Sym;
  rho_pqrs_Sym.addAntiSymmetry(0, 1);
  rho_pqrs_Sym.addAntiSymmetry(2, 3);

  T2 bad_rhopq_tensor = rho_pqrs.trace(cc2);
  for (int p = 0; p < nQubits; p++) {
    for (int q = 0; q < nQubits; q++) {
      bad_energy += 0.5 * std::real(hpq(p, q) * (bad_rhopq_tensor(p, q) +
                                                 bad_rhopq_tensor(q, p)));
    }
  }

  xacc::info("Non-purified Energy: " + std::to_string(bad_energy));

  //   xacc::info("Filtering 2-RDM");
  Eigen::Tensor<double, 4> filtered_rhopqrs(nQubits, nQubits, nQubits, nQubits);
  filtered_rhopqrs.setZero();

  for (int p = 0; p < nQubits; p++) {
    for (int q = 0; q < nQubits; q++) {
      for (int r = 0; r < nQubits; r++) {
        for (int s = 0; s < nQubits; s++) {
          if ((p <= q) && (r <= s)) {
            filtered_rhopqrs(p, q, r, s) = rho_pqrs(p, q, r, s);
            // filtered_rhopqrs(r, s, p, q) = rho_pqrs(p, q, r, s);
          }
        }
      }
    }
  }

  T4 rdm = filtered_rhopqrs;
  Eigen::array<IP, 2> sq_indices{IP(2, 0), IP(3, 1)};

  T4 rdmSq = rdm.contract(rdm, sq_indices);
  T4 diff = rdmSq - rdm;
  T4 diffSq = diff.contract(diff, sq_indices);

  double tr_diff_sq = 0.0;
  for (int p = 0; p < nQubits; p++) {
    for (int q = 0; q < nQubits; q++) {
      tr_diff_sq += std::real(diffSq(p, q, p, q));
    }
  }

  int count = 0;
  while (tr_diff_sq > 1e-8) {
    auto tr_rdm = 0.0;
    for (int p = 0; p < nQubits; p++) {
      for (int q = 0; q < nQubits; q++) {
        tr_rdm += std::real(rdm(p, q, p, q));
      }
    }

    std::stringstream sss;
    sss << "diffsq_tr: " << std::setprecision(8) << tr_diff_sq
        << ", rdm_tr: " << tr_rdm;
    xacc::info("Iter: " + std::to_string(count) + ", diffsq_tr: " + sss.str());
    rdm = rdm * rdm.constant(1. / tr_rdm);

    rdmSq = rdm.contract(rdm, sq_indices);
    diff = rdmSq - rdm;
    diffSq = diff.contract(diff, sq_indices);

    rdm = rdm.constant(3.) * rdmSq -
          rdm.constant(2.) * rdm.contract(rdmSq, sq_indices);

    tr_diff_sq = 0.0;
    for (int p = 0; p < nQubits; p++) {
      for (int q = 0; q < nQubits; q++) {
        tr_diff_sq += std::real(diffSq(p, q, p, q));
      }
    }

    count++;
  }

  // reconstruct rhopqrs using symmetry rules
  for (int p = 0; p < nQubits; p++) {
    for (int q = 0; q < nQubits; q++) {
      for (int r = 0; r < nQubits; r++) {
        for (int s = 0; s < nQubits; s++) {
          rho_pqrs_Sym(rdm, p, q, r, s) = rdm(p, q, r, s);
          rho_pqrs_Sym(rdm, r, s, p, q) = rdm(p, q, r, s);
        }
      }
    }
  }

  realt = rdm.real();
  real = realt.data();
  std::vector<double> fixed_rho_pqrs_data(real, real + rho_pqrs.size());

  T2 rhopq_tensor = rdm.trace(cc2);
  T0 tmp = rhopq_tensor.trace();
  auto rhopq_trace = std::real(tmp(0));
  xacc::info("Tr(rhopq): " + std::to_string(rhopq_trace));

  for (int p = 0; p < nQubits; p++) {
    for (int q = 0; q < nQubits; q++) {
      for (int r = 0; r < nQubits; r++) {
        for (int s = 0; s < nQubits; s++) {
          energy += 0.5 * std::real(hpqrs(p, q, r, s) *
                                    (rdm(p, q, s, r) + rdm(r, s, q, p)));
        }
      }
    }
  }
  //   xacc::info("Energy after 2-rdm: " + std::to_string(energy));

  for (int p = 0; p < nQubits; p++) {
    for (int q = 0; q < nQubits; q++) {
      energy += 0.5 * std::real(hpq(p, q) *
                                (rhopq_tensor(p, q) + rhopq_tensor(q, p)));
    }
  }

  xacc::info("Purified energy " + std::to_string(energy));

  //   auto ir = xacc::getIRProvider("quantum")->createIR();
  //   for (auto k : functions) {std::cout << k->toString() << "\n\n";
  //   ir->addComposite(k);}

  //   xacc::quantum::PauliOperator op;
  //   op.fromXACCIR(ir);

  //   std::cout << "HAM: " << op.toString() << "\n";
  for (auto &b : buffers) {
    b->addExtraInfo("purified-energy", ExtraInfo(energy));
    b->addExtraInfo("non-purified-energy", ExtraInfo(bad_energy));
    b->addExtraInfo("exp-val-z", ExtraInfo(b->getExpectationValueZ()));
    buffer->appendChild(b->name(), b);
  }

  buffers[0]->addExtraInfo("noisy-rdm", ExtraInfo(rho_pqrs_data));
  buffers[0]->addExtraInfo("fixed-rdm", ExtraInfo(fixed_rho_pqrs_data));
  buffers[0]->addExtraInfo("hpqrs", ExtraInfo(hpqrs_vec));
  buffers[0]->addExtraInfo("hpq", ExtraInfo(hpq_vec));

  buffer->addExtraInfo("__internal__decorator_aggregate_vqe__", energy);
  return;
}

} // namespace quantum
} // namespace xacc
