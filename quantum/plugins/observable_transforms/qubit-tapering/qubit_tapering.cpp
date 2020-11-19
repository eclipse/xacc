#include "qubit_tapering.hpp"

#include <armadillo>
#include <iomanip>

#include "FermionOperator.hpp"
#include "xacc_observable.hpp"
#include "xacc_plugin.hpp"
#include "xacc_service.hpp"
#include "xacc.hpp"

namespace xacc {

template <typename T>
bool ptr_is_a(std::shared_ptr<Observable> ptr) {
  return std::dynamic_pointer_cast<T>(ptr) != nullptr;
}
std::shared_ptr<xacc::Observable> QubitTapering::transform(
    std::shared_ptr<xacc::Observable> Hptr_input) {

  // First we pre-process the observable to a PauliOperator
  auto obs_str = Hptr_input->toString();
  auto fermi_to_pauli = xacc::getService<xacc::ObservableTransform>("jw");
  std::shared_ptr<xacc::Observable> Hptr;
  if (ptr_is_a<FermionOperator>(Hptr_input)) {  
    Hptr = fermi_to_pauli->transform(Hptr_input);
  } else if (obs_str.find("^") != std::string::npos) {
    auto fermionObservable = xacc::quantum::getObservable("fermion", obs_str);
    Hptr = fermi_to_pauli->transform(fermionObservable);
  } else if (ptr_is_a<PauliOperator>(Hptr_input)) {
    Hptr = Hptr_input;
  } else if (obs_str.find("X") != std::string::npos ||
             obs_str.find("Y") != std::string::npos ||
             obs_str.find("Z") != std::string::npos) {
    Hptr = xacc::quantum::getObservable("pauli", obs_str);
  } else {
    xacc::error("[Qubit Tapering] Error, cannot cast incoming Observable ptr to something we can process.");
  }

  // Convert the IR into a Hamiltonian
  PauliOperator &H = dynamic_cast<PauliOperator &>(*Hptr.get());

  auto n = H.nQubits();
  int counter = 0;

  // Compute Tableaux of the hamiltonian,
  // This is the E matrix from arxiv:1701.08213
  Eigen::MatrixXi tableaux = computeTableaux(H, n);
  std::vector<int> pivotCols;

  // Convert tableaux to rref
  Eigen::MatrixXi b = gauss(tableaux, pivotCols);

  // Get linear independent vectors from rref
  int ker_dim = 2 * n - pivotCols.size();
  Eigen::MatrixXi linIndVecs(pivotCols.size(), 2 * n);
  linIndVecs.setZero();
  std::vector<std::vector<int>> linIndVecsVector;
  for (int i = 0; i < pivotCols.size(); i++) {
    linIndVecs.row(i) = b.row(i);
  }

  // Build up the symmetry group generators
  std::vector<int> zero(2 * n);
  std::set<std::vector<int>> generators, generated;
  generated.insert(zero);

  // Get all g_z (combinations of nqubit 1s and 0s)
  auto combinations = generateCombinations(n);

  // Lambda to compute the union of 2 sets
  auto getUnion = [](const std::set<std::vector<int>> &a,
                     const std::set<std::vector<int>> &b) {
    std::set<std::vector<int>> result = a;
    result.insert(b.begin(), b.end());
    return result;
  };

  // Loop over the g_z 0s and 1s combinations
  for (auto &gz : combinations) {
    std::vector<int> tau(n);
    tau.insert(tau.end(), gz.begin(), gz.end());

    if (generated.find(tau) != generated.end()) {
      continue;
    } else {
      int sum = 0;
      for (int i = 0; i < linIndVecs.rows(); i++) {
        Eigen::VectorXi row = linIndVecs.row(i);
        std::vector<int> h(2 * n);
        Eigen::VectorXi::Map(&h[0], 2 * n) = row;
        sum += binaryVectorInnerProduct(tau, h);
      }

      if (sum == 0) {
        generators.insert(tau);
        counter++;

        if (counter == ker_dim) break;

        for (auto &g : generators) {
          std::vector<int> s(2 * n);
          for (int i = 0; i < 2 * n; i++) {
            s[i] = (tau[i] + g[i]) % 2;
          }

          std::set<std::vector<int>> unionSet = getUnion(generators, generated);
          if (unionSet.find(s) == unionSet.end()) {
            generated.insert(s);
          }
        }
      }
    }
  }

  // Create the tau generators
  std::vector<PauliOperator> taus;
  for (auto &g : generators) {
    std::map<int, std::string> terms;
    for (int i = 0; i < 2 * n; i++) {
      if (g[i] == 1) {
        if (i < n) {
          terms.insert({i, "X"});
        } else {
          terms.insert({i - n, "Z"});
        }
      }
    }
    taus.emplace_back(terms);
  }

  //   for (auto t : taus) std::cout << "GEN: " << t.toString() << "\n";

  // Lambda to compute hermitian conjugate of PauliOperator
  auto hc = [](PauliOperator &op) {
    PauliOperator newOp = op;
    for (auto &kv : newOp) {
      kv.second.coeff() = std::conj(kv.second.coeff());
    }
    return newOp;
  };

  // Compute U = U1 U2 U3 ...
  double invSqrt2 = 1.0 / std::sqrt(2.0);
  PauliOperator U(1.0);
  for (auto &t : taus) {
    int i = t.getTerms().begin()->second.ops().begin()->first;
    U *= (t + PauliOperator({{i, "X"}})) * invSqrt2;
    U *= (PauliOperator({{i, "X"}}) + PauliOperator({{i, "Z"}})) *
         (1.0 / std::sqrt(2));
  }

  // Use U to compute HPrime
  PauliOperator HPrime = hc(U) * H * U;

  // Compute rref of HPrime
  Eigen::MatrixXi hPrimeTableaux = computeTableaux(HPrime, n);
  std::vector<int> hPrimePivotCols;
  Eigen::MatrixXi b2 = gauss(hPrimeTableaux, hPrimePivotCols);

  // Convert to hPrimePivotCols vector to a set
  std::set<int> hPrimePivotColsSet(hPrimePivotCols.begin(),
                                   hPrimePivotCols.end()),
      doubleNSet, nSet;

  // Generate range(2*n)
  for (int i = 0; i < 2 * n; i++) doubleNSet.insert(i);

  // Generate range(n)
  for (int i = 0; i < n; i++) nSet.insert(i);

  // Create the phase_sites set which is the difference between
  // range(2*nQ) and the hPrimePivotSet
  std::set<int> phase_sites, keep_sites;
  std::set_difference(doubleNSet.begin(), doubleNSet.end(),
                      hPrimePivotColsSet.begin(), hPrimePivotColsSet.end(),
                      std::inserter(phase_sites, phase_sites.end()));

  // Create the keep_sites set, i.e. the qubit sites we
  // are keeping in the reduction
  std::set_difference(nSet.begin(), nSet.end(), phase_sites.begin(),
                      phase_sites.end(),
                      std::inserter(keep_sites, keep_sites.end()));

  // Generate all 1s and 0s of list size phase_sites.size(), map
  // all 0s to -1s.
  std::vector<std::vector<int>> phase_configs =
      generateCombinations(phase_sites.size(), [&](std::vector<int> &tmp) {
        for (int i = 0; i < phase_sites.size(); i++) {
          if (tmp[i] == 0) tmp[i] = -1;
        }
        return;
      });

  // Create the reduced hamiltonian
  // by mapping operators on unused qubits to
  // +-1 subspace
  auto actualReduced = std::make_shared<PauliOperator>();
  //   PauliOperator& actualReduced = *actualReduced_ptr.get();
  double energy = 0.0;
  for (auto &phases : phase_configs) {
    counter = 0;
    std::map<int, int> Z2_dict;
    for (auto &z : phase_sites) {
      Z2_dict.insert({z, phases[counter]});
      counter++;
    }

    auto reduced_ptr = std::make_shared<PauliOperator>();
    PauliOperator &reduced = *reduced_ptr.get();

    for (auto &kv : HPrime) {
      auto term = kv.second;
      if (term.ops().empty()) {
        reduced += PauliOperator(term.coeff());
      } else {
        std::map<int, std::string> newTerm;
        double phase = 1.0;

        for (auto &termKv : term.ops()) {
          if (keep_sites.find(termKv.first) != keep_sites.end()) {
            newTerm.insert({termKv.first, termKv.second});
          }

          if (Z2_dict.count(termKv.first)) {
            phase *= Z2_dict[termKv.first];
          }
        }
        reduced += PauliOperator(newTerm, phase * term.coeff());
      }
    }

    // Get the ground state energy to check if this is the
    // minimizing sector
    auto reducedEnergy = computeGroundStateEnergy(reduced, n);
    if (reducedEnergy < energy) {
      energy = reducedEnergy;
      actualReduced = reduced_ptr;
    }
  }

  counter = 0;
  std::map<int, int> keepSites2Logical;
  for (auto &i : keep_sites) {
    keepSites2Logical.insert({i, counter});
    counter++;
  }

  actualReduced->mapQubitSites(keepSites2Logical);

  std::stringstream s;
  s << std::setprecision(12) << energy;
  std::cout << "Reduced Hamiltonian:" << actualReduced->toString()
            << ", with energy = " << s.str() << "\n";

  return actualReduced;

  //   auto newIR = actualReduced.toXACCIR();

  //   // See if we have an ansatz and if so grab
  //   // it and add it to the reduced IR
  //   auto ansatz = std::dynamic_pointer_cast<Function>(
  //       ir->getKernels()[0]->getInstruction(0));
  //   if (ansatz) {
  //     for (auto &k : newIR->getKernels()) {
  //       k->insertInstruction(0, ansatz);
  //     }
  //   }

  //   return newIR;
}

const double QubitTapering::computeGroundStateEnergy(PauliOperator &op,
                                                     const int n) {
  auto n_qubits = op.nQubits();
  auto n_hilbert = std::pow(2, n_qubits);
  using SparseMatrix = arma::SpMat<std::complex<double>>;

  SparseMatrix x(2, 2), y(2, 2), z(2, 2);
  x(0, 1) = 1.0;
  x(1, 0) = 1.0;
  y(0, 1) = std::complex<double>(0, -1);
  y(1, 0) = std::complex<double>(0, 1);
  z(0, 0) = 1.;
  z(1, 1) = -1.;

  SparseMatrix i = arma::speye<SparseMatrix>(2, 2);

  std::map<std::string, SparseMatrix> mat_map{
      {"I", i}, {"X", x}, {"Y", y}, {"Z", z}};

  auto kron_ops = [](std::vector<SparseMatrix> &ops) {
    auto first = ops[0];
    for (int i = 1; i < ops.size(); i++) {
      first = arma::kron(first, ops[i]);
    }
    return first;
  };

  SparseMatrix total(n_hilbert, n_hilbert);
  for (auto &term : op.getTerms()) {
    auto tensor_factor = 0;
    auto coeff = term.second.coeff();

    std::vector<SparseMatrix> sparse_mats;

    if (term.second.ops().empty()) {
      // this was I term
      auto id = arma::speye<SparseMatrix>(n_hilbert, n_hilbert);
      sparse_mats.push_back(id);
    } else {
      for (auto &pauli : term.second.ops()) {
        if (pauli.first > tensor_factor) {
          auto id_qbits = pauli.first - tensor_factor;
          auto id = arma::speye<SparseMatrix>((int)std::pow(2, id_qbits),
                                              (int)std::pow(2, id_qbits));
          sparse_mats.push_back(id);
        }

        sparse_mats.push_back(mat_map[pauli.second]);
        tensor_factor = pauli.first + 1;
      }

      for (int i = tensor_factor; i < n_qubits; i++) {
        auto id = arma::speye<SparseMatrix>(2, 2);
        sparse_mats.push_back(id);
      }
    }

    auto sp_matrix = kron_ops(sparse_mats);
    sp_matrix *= coeff;
    total += sp_matrix;
  }

  arma::vec eigval;
  arma::mat eigvec;

  arma::sp_mat test(total.n_rows, total.n_cols);
  for (auto i = total.begin(); i != total.end(); ++i) {
    test(i.row(), i.col()) = (*i).real();
  }

  arma::eigs_sym(eigval, eigvec, test, 1);

  double reducedEnergy = eigval(0);
  return reducedEnergy;
}

Eigen::MatrixXi QubitTapering::computeTableaux(PauliOperator &H, const int n) {
  int nRows = 0;
  Eigen::MatrixXi tableaux;
  for (auto &term : H) {
    Eigen::VectorXi x(n), z(n), row(2 * n);
    auto p = term.second.toBinaryVector(n);
    int c = 0;
    for (auto &i : p.first) {
      z(c) = i;
      c++;
    }
    c = 0;
    for (auto &i : p.second) {
      x(c) = i;
      c++;
    }

    row << x, z;
    if (row != Eigen::VectorXi::Zero(2 * n)) {
      nRows++;
      tableaux.conservativeResize(nRows, 2 * n);
      tableaux.row(nRows - 1) = row;
    }
  }
  return tableaux;
}

std::vector<std::vector<int>> QubitTapering::generateCombinations(
    const int n, std::function<void(std::vector<int> &)> &&f) {
  std::vector<std::vector<int>> combinations;

  for (int nOnes = 0; nOnes <= n; nOnes++) {
    std::vector<int> test(n);

    for (int k = 0; k < nOnes; k++) test[n - k - 1] = 1;

    do {
      // Perform custom mapping on the vector before adding
      // By default this does nothing
      f(test);
      combinations.push_back(test);
    } while (std::next_permutation(test.begin(), test.end()));
  }

  return combinations;
}

int QubitTapering::binaryVectorInnerProduct(std::vector<int> &bv1,
                                            std::vector<int> &bv2) {
  std::vector<int> ax(bv1.begin(), bv1.begin() + bv1.size() / 2);
  std::vector<int> az(bv1.begin() + bv1.size() / 2, bv1.begin() + bv1.size());
  std::vector<int> bx(bv2.begin(), bv2.begin() + bv2.size() / 2);
  std::vector<int> bz(bv2.begin() + bv2.size() / 2, bv2.begin() + bv2.size());
  Eigen::VectorXi axv = Eigen::Map<Eigen::VectorXi>(ax.data(), ax.size());
  Eigen::VectorXi azv = Eigen::Map<Eigen::VectorXi>(az.data(), az.size());
  Eigen::VectorXi bxv = Eigen::Map<Eigen::VectorXi>(bx.data(), bx.size());
  Eigen::VectorXi bzv = Eigen::Map<Eigen::VectorXi>(bz.data(), bz.size());
  return (axv.dot(bzv) + azv.dot(bxv)) % 2;
}

Eigen::MatrixXi QubitTapering::gauss(Eigen::MatrixXi &A,
                                     std::vector<int> &pivotCols) {
  int n = A.rows();
  int m = A.cols();

  int sc = 0;

  for (int i = 0; i < m; i++) {
    bool found_row = false;
    int ip = i - sc;
    for (int k = ip; k < n; k++) {
      if (A(k, i) == 1) {
        found_row = true;

        if (k > ip) {
          for (int j = i; j < m; j++) {
            auto tmp = A(k, j);
            A(k, j) = A(ip, j);
            A(ip, j) = tmp;
          }
        }

        for (int l = ip + 1; l < n; l++) {
          if (A(l, i) != 0) {
            for (int j = ip; j < m; j++) {
              A(l, j) = (A(l, j) + A(ip, j)) % 2;
            }
          }
        }
        break;
      }
    }
    if (!found_row) sc += 1;
  }

  unsigned row2 = 0;
  for (unsigned col = 0; col < A.cols() && row2 < A.rows(); col++) {
    if (std::fabs(A(row2, col)) < 1e-12) continue;

    pivotCols.push_back(col);
    row2++;
  }

  return A;
}

}  // namespace xacc

REGISTER_PLUGIN(xacc::QubitTapering, xacc::ObservableTransform)