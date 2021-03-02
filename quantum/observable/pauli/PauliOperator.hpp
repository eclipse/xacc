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
#ifndef QUANTUM_UTILS_PAULIOPERATOR_HPP_
#define QUANTUM_UTILS_PAULIOPERATOR_HPP_
#include <unordered_map>
#include <complex>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Observable.hpp"
#include "IR.hpp"

#include "Cloneable.hpp"

// Putting this here due to clang error
// not able to find operator!= from operators.hpp
namespace xacc {
namespace quantum {
class PauliOperator;
}
} // namespace xacc

bool operator==(const xacc::quantum::PauliOperator &lhs,
                const xacc::quantum::PauliOperator &rhs);

#include "operators.hpp"

namespace xacc {

namespace quantum {

// A Term can be a coefficient, a variable coefficient, and the terms themselves
using TermTuple =
    std::tuple<std::complex<double>, std::string, std::map<int, std::string>>;
using c = std::complex<double>;
using ActionResult = std::pair<std::string, c>;
enum ActionType { Bra, Ket };

class Term : public TermTuple,
             public tao::operators::commutative_multipliable<Term>,
             public tao::operators::equality_comparable<Term> {

protected:
  // static std::map<std::string, std::pair<c, std::string>>
  void create_map() {
    // static std::map<std::string, std::pair<c, std::string>> m;
    pauliProducts.insert({"II", {c(1.0, 0.0), "I"}});
    pauliProducts.insert({"IX", {c(1.0, 0.0), "X"}});
    pauliProducts.insert({"XI", {c(1.0, 0.0), "X"}});
    pauliProducts.insert({"IY", {c(1.0, 0.0), "Y"}});
    pauliProducts.insert({"YI", {c(1.0, 0.0), "Y"}});
    pauliProducts.insert({"ZI", {c(1.0, 0.0), "Z"}});
    pauliProducts.insert({"IZ", {c(1.0, 0.0), "Z"}});
    pauliProducts.insert({"XX", {c(1.0, 0.0), "I"}});
    pauliProducts.insert({"YY", {c(1.0, 0.0), "I"}});
    pauliProducts.insert({"ZZ", {c(1.0, 0.0), "I"}});
    pauliProducts.insert({"XY", {c(0.0, 1.0), "Z"}});
    pauliProducts.insert({"XZ", {c(0.0, -1.0), "Y"}});
    pauliProducts.insert({"YX", {c(0.0, -1.0), "Z"}});
    pauliProducts.insert({"YZ", {c(0.0, 1.0), "X"}});
    pauliProducts.insert({"ZX", {c(0.0, 1.0), "Y"}});
    pauliProducts.insert({"ZY", {c(0.0, -1.0), "X"}});
    // return m;
  }

  std::map<std::string, std::pair<c, std::string>> pauliProducts;

public:
  Term() {
    std::get<0>(*this) = std::complex<double>(0, 0);
    std::get<1>(*this) = "";
    std::get<2>(*this) = {};
    create_map();
  }

  Term(const Term &t) {
    std::get<0>(*this) = std::get<0>(t);
    std::get<1>(*this) = std::get<1>(t);
    std::get<2>(*this) = std::get<2>(t);
    create_map();
  }

  Term(std::complex<double> c) {
    std::get<0>(*this) = c;
    std::get<1>(*this) = "";
    std::get<2>(*this) = {};
    create_map();
  }

  Term(double c) {
    std::get<0>(*this) = std::complex<double>(c, 0);
    std::get<1>(*this) = "";
    std::get<2>(*this) = {};
    create_map();
  }

  Term(std::complex<double> c, std::map<int, std::string> ops) {
    std::get<0>(*this) = c;
    std::get<1>(*this) = "";
    std::get<2>(*this) = ops;
    create_map();
  }

  Term(std::string var) {
    std::get<0>(*this) = std::complex<double>(1, 0);
    std::get<1>(*this) = var;
    std::get<2>(*this) = {};
    create_map();
  }

  Term(std::complex<double> c, std::string var) {
    std::get<0>(*this) = c;
    std::get<1>(*this) = var;
    std::get<2>(*this) = {};
    create_map();
  }

  Term(std::string var, std::map<int, std::string> ops) {
    std::get<0>(*this) = std::complex<double>(1, 0);
    std::get<1>(*this) = var;
    std::get<2>(*this) = ops;
    create_map();
  }

  Term(std::complex<double> c, std::string var,
       std::map<int, std::string> ops) {
    std::get<0>(*this) = c;
    std::get<1>(*this) = var;
    std::get<2>(*this) = ops;
    create_map();
  }

  Term(std::map<int, std::string> ops) {
    std::get<0>(*this) = std::complex<double>(1, 0);
    std::get<1>(*this) = "";
    std::get<2>(*this) = ops;
    create_map();
  }

  static const std::string id(const std::map<int, std::string> &ops,
                              const std::string &var = "") {
    std::string s;
    s = var;
    for (auto &t : ops) {
      if (t.second != "I") {
        s += t.second + std::to_string(t.first);
      }
    }

    if (s.empty()) {
      return "I";
    }

    return s;
  }

  const std::string id() const {
    std::string s;
    s = std::get<1>(*this);
    for (auto &t : std::get<2>(*this)) {
      if (t.second != "I") {
        s += t.second + std::to_string(t.first);
      }
    }

    if (s.empty()) {
      return "I";
    }

    return s;
  }

  std::map<int, std::string> &ops() { return std::get<2>(*this); }

  bool isIdentity() {
    if (ops().empty() || (ops().size() == 1 && ops().begin()->second == "I")) {
      return true;
    } else {
      return false;
    }
  }

  std::complex<double> &coeff() { return std::get<0>(*this); }

  std::string &var() { return std::get<1>(*this); }

  Term &operator*=(const Term &v) noexcept;

  bool operator==(const Term &v) noexcept {
    return (std::get<1>(*this) == std::get<1>(v) && ops() == std::get<2>(v));
  }

  std::vector<SparseTriplet> getSparseMatrixElements(const int nQubits);

  ActionResult action(const std::string &bitString, ActionType type);

  std::pair<std::vector<int>, std::vector<int>>
  toBinaryVector(const int nQubits);
};

class PauliOperator
    : public xacc::Observable,
      public xacc::Cloneable<Observable>,
      public tao::operators::commutative_ring<PauliOperator>,
      public tao::operators::equality_comparable<PauliOperator>,
      public tao::operators::commutative_multipliable<PauliOperator, double>,
      public tao::operators::commutative_multipliable<PauliOperator,
                                                      std::complex<double>> {
protected:
  std::map<std::string, Term> terms;

public:
  std::shared_ptr<Observable> clone() override {
    return std::make_shared<PauliOperator>();
  }

  std::map<std::string, Term>::iterator begin() {
    return terms.begin();
  }
  std::map<std::string, Term>::iterator end() { return terms.end(); }

  PauliOperator();
  PauliOperator(std::complex<double> c);
  PauliOperator(double c);
  PauliOperator(std::string fromString);
  PauliOperator(std::complex<double> c, std::string var);
  PauliOperator(const PauliOperator &i);
  PauliOperator(std::map<int, std::string> operators);
  PauliOperator(std::map<int, std::string> operators, std::string var);
  PauliOperator(std::map<int, std::string> operators,
                std::complex<double> coeff);
  PauliOperator(std::map<int, std::string> operators, double coeff);
  PauliOperator(std::map<int, std::string> operators,
                std::complex<double> coeff, std::string var);

  std::vector<std::shared_ptr<CompositeInstruction>>
  observe(std::shared_ptr<CompositeInstruction> function) override;

  std::vector<std::shared_ptr<Observable>> getSubTerms() override {
    std::vector<std::shared_ptr<Observable>> ret;
    for (auto &term : getTerms()) {
      ret.emplace_back(
          new PauliOperator(term.second.ops(), term.second.coeff()));
    }
    return ret;
  }

  std::vector<std::shared_ptr<Observable>> getNonIdentitySubTerms() override {
    std::vector<std::shared_ptr<Observable>> ret;
    for (auto &term : getTerms()) {
      if (term.first != "I") {
        ret.emplace_back(
            new PauliOperator(term.second.ops(), term.second.coeff()));
      }
    }
    return ret;
  }

  std::shared_ptr<Observable> getIdentitySubTerm() override {
    for (auto &term : getTerms()) {
      if (term.first == "I") {
        return std::make_shared<PauliOperator>(term.second.ops(),
                                               term.second.coeff());
      }
    }

    return nullptr;
  }

  std::complex<double> coefficient() override;

  const std::vector<std::pair<std::string, std::complex<double>>>
  computeActionOnKet(const std::string &bitString);
  const std::vector<std::pair<std::string, std::complex<double>>>
  computeActionOnBra(const std::string &bitString);

  const int nTerms();
  std::pair<std::vector<int>, std::vector<int>>
  toBinaryVectors(const int nQubits) {
    return getTerms().begin()->second.toBinaryVector(nQubits);
  }

  const std::string toString() override;
  void fromString(const std::string str) override;

  bool contains(PauliOperator &op);
  bool commutes(PauliOperator &op);

  void mapQubitSites(std::map<int, int> &siteMap) {
    PauliOperator op;
    for (auto &termKv : *this) {
      auto ops = termKv.second.ops();
      std::map<int, std::string> nops;
      for (auto &kv : ops) {
        nops.insert({siteMap[kv.first], kv.second});
      }
      op += PauliOperator(nops, termKv.second.coeff());
    }

    *this = op;
  }
  void clear();

  std::map<std::string, Term> getTerms() const { return terms; }

  std::vector<SparseTriplet> getSparseMatrixElements() {return to_sparse_matrix();}
  std::vector<std::complex<double>> toDenseMatrix(const int nQubits);

  std::vector<SparseTriplet>
  to_sparse_matrix() override;

  std::shared_ptr<IR> toXACCIR();
  void fromXACCIR(std::shared_ptr<IR> ir);
  PauliOperator
  eval(const std::map<std::string, std::complex<double>> varToValMap);
  bool isClose(PauliOperator &other);
  int nQubits();
  const int nBits() override { return nQubits(); }

  PauliOperator &operator+=(const PauliOperator &v) noexcept;
  PauliOperator &operator-=(const PauliOperator &v) noexcept;
  PauliOperator &operator*=(const PauliOperator &v) noexcept;
  bool operator==(const PauliOperator &v) noexcept;
  PauliOperator &operator*=(const double v) noexcept;
  PauliOperator &operator*=(const std::complex<double> v) noexcept;

  const std::string name() const override { return "pauli"; }
  const std::string description() const override { return ""; }
  void fromOptions(const HeterogeneousMap &options) override { return; }
  
  std::shared_ptr<Observable>
  commutator(std::shared_ptr<Observable> obs) override;

  PauliOperator hermitianConjugate() const;

  void normalize() override;
  virtual double postProcess(std::shared_ptr<AcceleratorBuffer> buffer,
                             const std::string &postProcessTask,
                             const HeterogeneousMap &extra_data) override;
};
} // namespace quantum

template const quantum::PauliOperator
HeterogeneousMap::get<quantum::PauliOperator>(const std::string key) const;

} // namespace xacc

#endif
