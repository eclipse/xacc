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
 *******************************************************************************/
#ifndef XACC_QUBIT_POOL_HPP_
#define XACC_QUBIT_POOL_HPP_

#include "adapt.hpp"
#include "OperatorPool.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "Observable.hpp"
#include "xacc_observable.hpp"
#include "Circuit.hpp"
#include <memory>

using namespace xacc;

namespace xacc{
// namespace algorithm{
namespace quantum{

class QubitPool : public OperatorPool {

protected:

  std::vector<std::shared_ptr<Observable>> pool;

public:

  QubitPool() = default;

  bool optionalParameters(const HeterogeneousMap parameters) override {
    return true;
  }

// Helper function to generate a Pauli string from a pattern and indices
std::string generate_pauli_string(const std::vector<std::pair<char, int>>& pattern) {
    std::string result;
    for (const auto& [pauli, index] : pattern) {
        result += pauli;
        result += std::to_string(index);
        result += " ";
    }
    return result;
}

// Generate all combinations of two unique indices for n qubits
std::vector<std::pair<int, int>> generate_two_index_combinations(int n) {
    std::vector<std::pair<int, int>> combinations;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            combinations.emplace_back(i, j);
        }
    }
    return combinations;
}

// Generate all combinations of four unique indices for n qubits
std::vector<std::tuple<int, int, int, int>> generate_four_index_combinations(int n) {
    std::vector<std::tuple<int, int, int, int>> combinations;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            for (int k = j + 1; k < n; ++k) {
                for (int l = k + 1; l < n; ++l) {
                    combinations.emplace_back(i, j, k, l);
                }
            }
        }
    }
    return combinations;
}

// Generate all permutations of a given pattern of X and Y
std::vector<std::vector<char>> generate_permutations(const std::vector<char>& pattern) {
    std::vector<std::vector<char>> permutations;
    std::vector<char> p = pattern;
    do {
        permutations.push_back(p);
    } while (std::next_permutation(p.begin(), p.end()));
    return permutations;
}

  // generate the pool
  std::vector<std::shared_ptr<Observable>> generate(const int &nQubits) override {

    // The qubit pool vanishes for strings with an even number of Pauli Y's
    // and can have at most 4 operators
    // We loop over the indices for qubits q0-q3 and
    // {X, Y, X} for each qubit and make sure that we have the appropriate 
    // number of Y's and only add unique operators, i.e., those which 
    // survive symmetry operations in the PauliOperator class.
    std::vector<std::string> ops;
    for (const auto& [a, b] : generate_two_index_combinations(nQubits)) {
        std::vector<char> base_pattern = {'X', 'Y'};
        auto permutations = generate_permutations(base_pattern);

        for (const auto& perm : permutations) {
            std::vector<std::pair<char, int>> pattern = {{perm[0], a}, {perm[1], b}};
            ops.push_back(generate_pauli_string(pattern));
        }
    }

    // Case 2: Xa Xb Xc Yd and permutations (XXXY and all permutations)
    for (const auto& [a, b, c, d] : generate_four_index_combinations(nQubits)) {
        std::vector<char> base_pattern = {'X', 'X', 'X', 'Y'};
        auto permutations = generate_permutations(base_pattern);

        for (const auto& perm : permutations) {
            std::vector<std::pair<char, int>> pattern = {{perm[0], a}, {perm[1], b}, {perm[2], c}, {perm[3], d}};
            ops.push_back(generate_pauli_string(pattern));
        }
    }

    // Case 3: Xa Yb Yc Yd and permutations (XYYY and all permutations)
    for (const auto& [a, b, c, d] : generate_four_index_combinations(nQubits)) {
        std::vector<char> base_pattern = {'X', 'Y', 'Y', 'Y'};
        auto permutations = generate_permutations(base_pattern);

        for (const auto& perm : permutations) {
            std::vector<std::pair<char, int>> pattern = {{perm[0], a}, {perm[1], b}, {perm[2], c}, {perm[3], d}};
            ops.push_back(generate_pauli_string(pattern));
        }
    }

    const std::complex<double> i{0, 1};
    for (auto op : ops) {
      pool.push_back(std::dynamic_pointer_cast<Observable>(std::make_shared<PauliOperator>("(0, 1)" + op)));
    }
    return pool;
  }

  std::string operatorString(const int index) override {

    return pool[index]->toString();

  }

  double getNormalizationConstant(const int index) const override {

    if (pool.empty()) {
      xacc::error("You need to call generate() first.");
    }
    auto tmp = *std::dynamic_pointer_cast<PauliOperator>(pool[index]);
    tmp -= tmp.hermitianConjugate();
    return 1.0 / tmp.operatorNorm();
  }

  std::shared_ptr<CompositeInstruction> 
  getOperatorInstructions(const int opIdx, const int varIdx) const override {

    // Instruction service for the operator to be added to the ansatz
    auto gate = std::dynamic_pointer_cast<quantum::Circuit>(
        xacc::getService<Instruction>("exp_i_theta"));

    // Create instruction for new operator
    gate->expand(
        {std::make_pair("pauli", pool[opIdx]->toString()),
        std::make_pair("param_id", "x" + std::to_string(varIdx))});

    return gate;

  }

  const std::string name() const override { return "qubit-pool"; }
  const std::string description() const override { return ""; }
};

} // namespace algorithm
} // namespace xacc

#endif