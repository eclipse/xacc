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
#include "qpt.hpp"

#include "CompositeInstruction.hpp"
#include "Observable.hpp"
#include "xacc.hpp"
#include "PauliOperator.hpp"
#include <Eigen/Dense>
#include <unsupported/Eigen/KroneckerProduct>
#include <unsupported/Eigen/CXX11/Tensor>

#include <memory>
#include <iomanip>

using namespace xacc;
using namespace xacc::quantum;
using namespace std::complex_literals;

namespace xacc {
namespace algorithm {

bool QPT::initialize(const HeterogeneousMap &parameters) {

  qpu = parameters.getPointerLike<Accelerator>("accelerator");
  circuit = parameters.getPointerLike<CompositeInstruction>("circuit");

  return true;
}

const std::vector<std::string> QPT::requiredParameters() const { return {}; }

void QPT::execute(const std::shared_ptr<AcceleratorBuffer> buffer) const {

  int nQ = circuit->nLogicalBits();

  auto VStack =
      [](const std::vector<Eigen::MatrixXcd> &mat_vec) -> Eigen::MatrixXcd {
    assert(!mat_vec.empty());
    long num_cols = mat_vec[0].cols();
    size_t num_rows = 0;
    for (size_t mat_idx = 0; mat_idx < mat_vec.size(); ++mat_idx) {
      assert(mat_vec[mat_idx].cols() == num_cols);
      num_rows += mat_vec[mat_idx].rows();
    }
    Eigen::MatrixXcd vstacked_mat(num_rows, num_cols);
    size_t row_offset = 0;
    for (size_t mat_idx = 0; mat_idx < mat_vec.size(); ++mat_idx) {
      long cur_rows = mat_vec[mat_idx].rows();
      vstacked_mat.middleRows(row_offset, cur_rows) = mat_vec[mat_idx];
      row_offset += cur_rows;
    }
    return vstacked_mat;
  };

  // Paulis
  Eigen::MatrixXcd im = Eigen::MatrixXcd::Identity(2, 2), px(2, 2), py(2, 2),
                   pz(2, 2);
  px << 0, 1, 1, 0;
  py << 0, -1i, 1i, 0;
  pz << 1, 0, 0, -1;

  std::vector<Eigen::MatrixXcd> rhos{0.5 * (im + pz), 0.5 * (im - pz),
                                     0.5 * (im + px), 0.5 * (im + py)};
  std::map<std::string, Eigen::MatrixXcd> prep_label_2_rho{
      {"zp", rhos[0]}, {"zm", rhos[1]}, {"xp", rhos[2]}, {"yp", rhos[3]}};

  // Measurement operators
  Eigen::MatrixXcd x0(2, 2), x1(2, 2), y0(2, 2), y1(2, 2);
  x0 << .5, .5, .5, .5;
  x1 << .5, -.5, -.5, .5;
  y0 << .5, -.5i, .5i, .5;
  y1 << .5, .5i, -.5i, .5;

  std::vector<Eigen::MatrixXcd> pauli_measure_mats{
      x0, x1, y0, y1, 0.5 * (im + pz), 0.5 * (im - pz)},
      blocks;

  std::map<std::string, std::vector<Eigen::MatrixXcd>> pauli_measure_mats_map{
      {"X", {x0, x1}},
      {"Y", {y0, y1}},
      {"Z", {0.5 * (im + pz), 0.5 * (im - pz)}}};

  // Generate all nQubit Pauli Strings
  std::vector<std::string> XYZ{"X", "Y", "Z"};
  std::vector<std::string> collected_strings;
  std::function<void(std::vector<std::string> &, std::string, const int,
                     const int)>
      y;
  y = [&](std::vector<std::string> &set, std::string prefix, const int n,
          const int k) {
    if (k == 0) {
      collected_strings.push_back(prefix);
      return;
    }

    for (int i = 0; i < n; i++) {
      auto newPrefix = prefix + " " + set[i];
      y(set, newPrefix, n, k - 1);
    }
  };

  auto x = [&](std::vector<std::string> &set, const int k) {
    auto n = set.size();
    y(set, std::string(""), n, k);
  };

  // Get all Permutations of XYZ
  x(XYZ, nQ);

  // Goal 1, create all nqubit pauli strings / observables
  std::vector<PauliOperator> basis_observables;
  for (auto s : collected_strings) {
    xacc::trim(s);
    auto paulis = xacc::split(s, ' ');
    std::string pauli_with_idxs = "";
    int idx = 0;
    for (auto p : paulis) {
      pauli_with_idxs += p + std::to_string(idx) + " ";
      idx++;
    }
    basis_observables.emplace_back(pauli_with_idxs);
  }

  // Goal 2, create states zplus, zmins, xplus, yplus
  // PLUS circuit on all qubits
  auto provider = xacc::getIRProvider("quantum");
  auto circuit_as_shared = std::shared_ptr<CompositeInstruction>(
      circuit, xacc::empty_delete<CompositeInstruction>());
  std::map<int, std::vector<std::shared_ptr<CompositeInstruction>>>
      prep_circuits;

  // template composite instructions, call setBits to change qbit idx
  auto template_generator =
      [&](const std::string &name,
          std::size_t qbit) -> std::shared_ptr<CompositeInstruction> {
    if (name == "zp") {
      return provider->createComposite("zp");
    } else if (name == "zm") {
      auto zm_i = provider->createComposite("zm");
      zm_i->addInstruction(
          provider->createInstruction("X", std::vector<std::size_t>{qbit}));
      return zm_i;
    } else if (name == "xp") {
      auto xp_i = provider->createComposite("xp");
      xp_i->addInstruction(
          provider->createInstruction("H", std::vector<std::size_t>{qbit}));
      return xp_i;
    } else if (name == "yp") {
      auto yp_i = provider->createComposite("yp");
      yp_i->addInstruction(
          provider->createInstruction("H", std::vector<std::size_t>{qbit}));
      yp_i->addInstruction(
          provider->createInstruction("S", std::vector<std::size_t>{qbit}));
      return yp_i;
    } else {
      return nullptr;
    }
  };

  std::vector<std::string> prep_names{"zp", "zm", "xp", "yp"};
  collected_strings.clear();
  x(prep_names, nQ);

  std::vector<std::vector<std::string>> prep_labels;
  for (auto c : collected_strings) {
    xacc::trim(c);
    auto preps = xacc::split(c, ' ');
    prep_labels.push_back(preps);
  }

  // Get all possible bit strings
  collected_strings.clear();
  std::vector<std::string> bits{"0", "1"}, all_bit_strings;
  x(bits, nQ);
  for (auto &bs : collected_strings) {
    xacc::trim(bs);
    auto bbits = xacc::split(bs, ' ');
    std::string bbs = "";
    for (auto bb : bbits)
      bbs += bb;
    all_bit_strings.push_back(bbs);
  }

  std::vector<std::string> names;
  std::vector<std::shared_ptr<CompositeInstruction>> all_circuits;
  for (auto &prep_names_on_qbits : prep_labels) {
    std::string comp_name = "";
    for (auto &p : prep_names_on_qbits) {
      comp_name += p + "_";
    }
    auto prep_on_all_qbits = provider->createComposite(comp_name + "all");
    for (std::size_t i = 0; i < nQ; i++) {
      auto prep_name = prep_names_on_qbits[i];
      auto prep = template_generator(prep_name, i);
      if (i > 0) {
        prep->setBits({i});
      }
      prep_on_all_qbits->addInstructions(prep->getInstructions());
    }

    prep_on_all_qbits->addInstruction(circuit_as_shared);

    // Create Prep Operator
    Eigen::MatrixXcd prep;
    for (int k = 0; k < prep_names_on_qbits.size(); k++) {
      auto p = prep_names_on_qbits[k];
      //   std::cout << p << ", " << k << ", " << prep_label_2_rho[p] << "\n";
      if (k == 0) {
        prep = prep_label_2_rho[p];
      } else {
        prep = Eigen::kroneckerProduct(prep_label_2_rho[p], prep).eval();
      }
    }

    // std::cout << "PREP:\n" << prep << "\n";
    for (auto &basis_element : basis_observables) {
      auto prep_on_all_and_meas = basis_element.observe(prep_on_all_qbits)[0];

      // Observable::observe will change basis to {X,Y,Z}, we want to
      // measure Y.
      for (int i = 0; i < prep_on_all_and_meas->nInstructions(); i++) {
        auto inst = prep_on_all_and_meas->getInstruction(i);
        if (inst->name() == "Rx") {
          auto sdg = provider->createInstruction("Sdg", inst->bits());
          auto h = provider->createInstruction("H", inst->bits());
          auto c = provider->createComposite("__tmp_sdg_h");
          c->addInstructions({sdg, h});
          prep_on_all_and_meas->replaceInstruction(i, c);
        }
      }

      std::stringstream ss;
      std::vector<std::string> measure_label;
      std::vector<int> idxs;
      auto pauli_as_term = *basis_element.begin();
      for (auto &kv : pauli_as_term.second.ops()) {
        // std::cout << "OP: " << kv.second << " ";
        measure_label.push_back(kv.second);
        idxs.push_back(kv.first);
      }
      //   std::cout << "\n";
      ss << measure_label;

      names.push_back(comp_name + "_" + ss.str());

      all_circuits.push_back(prep_on_all_and_meas);

      std::vector<Eigen::MatrixXcd> meas_mats;
      for (auto iter : all_bit_strings) {
        // std::cout << iter << "\n";
        // Direction of loop here is dependent on bit ordering
        // from backend accelerator. Default is MSB like IBM
        Eigen::MatrixXcd meas_mat;
        int counter = 0;
        for (int k = measure_label.size() - 1; k >= 0; k--) {
          int bit = iter[counter] == '0' ? 0 : 1;
          //   std::cout << "\t kron " << measure_label[k] << ", " << bit <<
          //   "\n";
          auto mat = pauli_measure_mats_map[measure_label[k]][bit];
          if (k == measure_label.size() - 1) {
            meas_mat = mat;
          } else {
            meas_mat = Eigen::kroneckerProduct(meas_mat, mat).eval();
          }
          counter++;
        }
        meas_mats.push_back(meas_mat);
      }

      std::vector<Eigen::MatrixXcd> prep_meas_mats;
      for (auto &m : meas_mats) {
        // std::cout << "Meas:\n" << m << "\n";
        prep_meas_mats.push_back(
            Eigen::kroneckerProduct(prep.transpose(), m).eval());
      }

      Eigen::MatrixXcd ret(prep_meas_mats.size(), prep_meas_mats[0].size());
      for (int k = 0; k < prep_meas_mats.size(); k++) {
        ret.row(k) = Eigen::Map<Eigen::VectorXcd>(prep_meas_mats[k].data(),
                                                  prep_meas_mats[k].size())
                         .conjugate();
      }

      blocks.push_back(ret);
    }
  }

  Eigen::MatrixXcd basis_matrix = VStack(blocks);

  // Execute and get data vector
  qpu->execute(buffer, all_circuits);
  auto children = buffer->getChildren();

  std::vector<std::complex<double>> data_vec;
  int i = 0;
  for (auto &child : children) {
    for (auto bit_string : all_bit_strings) {
      // FIXME NEEDED TO REVERSE HERE TO REPRODUCE RESULTS
      std::reverse(bit_string.begin(), bit_string.end());
      auto prob = child->computeMeasurementProbability(bit_string);
      data_vec.push_back(prob);
    }
    i++;
  }

  Eigen::VectorXcd data =
      Eigen::Map<Eigen::VectorXcd>(data_vec.data(), data_vec.size());

  Eigen::VectorXcd choi_vec =
      basis_matrix.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV)
          .solve(data);
  Eigen::MatrixXcd choi = Eigen::Map<Eigen::MatrixXcd>(
      choi_vec.data(), std::pow(2, 2 * nQ), std::pow(2, 2 * nQ));

  std::vector<double> choi_real_vec, choi_imag_vec;
  for (int i = 0; i < choi.rows(); i++) {
    for (int j = 0; j < choi.cols(); j++) {
      double real = choi(i, j).real(), imag = choi(i, j).imag();
      if (std::fabs(real) < 1e-12) {
        real = 0.0;
      }
      if (std::fabs(imag) < 1e-12) {
        imag = 0.0;
      }
      choi_real_vec.push_back(real);
      choi_imag_vec.push_back(imag);
    }
  }

  buffer->addExtraInfo("choi-real", choi_real_vec);
  buffer->addExtraInfo("choi-imag", choi_imag_vec);

  // Convert to Chi process matrix
  Eigen::MatrixXcd basis_mat(4, 4);
  basis_mat << 1, 0, 0, 1, 0, 1, 1, 0, 0, -1i, 1i, 0, 1, 0, 0, -1;
  Eigen::MatrixXcd cob = basis_mat;
  for (int i = 0; i < nQ - 1; i++) {
    int dim = (int)std::sqrt(cob.rows());
    Eigen::MatrixXcd tmp = Eigen::kroneckerProduct(basis_mat, cob);
    Eigen::Tensor<std::complex<double>, 6> tmp_tensor =
        Eigen::TensorMap<Eigen::Tensor<std::complex<double>, 6>>(
            tmp.data(), 4, dim * dim, 2, 2, dim, dim);
    Eigen::Tensor<std::complex<double>, 6> tmp2 =
        tmp_tensor.shuffle(std::array<int, 6>{0, 1, 2, 4, 3, 5});
    cob =
        Eigen::Map<Eigen::MatrixXcd>(tmp2.data(), 4 * dim * dim, 4 * dim * dim);
  }

  Eigen::MatrixXcd chi = (1 / std::pow(2, nQ)) * (cob * choi * cob.adjoint());
//   std::cout << "QPT Chi:\n" << chi << "\n\n";
  std::vector<double> chi_real_vec, chi_imag_vec;
  for (int i = 0; i < chi.rows(); i++) {
    for (int j = 0; j < chi.cols(); j++) {
      double real = chi(i, j).real(), imag = chi(i, j).imag();
      if (std::fabs(real) < 1e-12) {
        real = 0.0;
      }
      if (std::fabs(imag) < 1e-12) {
        imag = 0.0;
      }
      chi_real_vec.push_back(real);
      chi_imag_vec.push_back(imag);
    }
  }

  buffer->addExtraInfo("chi-real", chi_real_vec);
  buffer->addExtraInfo("chi-imag", chi_imag_vec);
}

} // namespace algorithm
} // namespace xacc