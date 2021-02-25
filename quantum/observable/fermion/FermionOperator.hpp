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
#ifndef XACC_FERMIONOPERATOR_HPP_
#define XACC_FERMIONOPERATOR_HPP_

#include "Observable.hpp"
#include "operators.hpp"
#include <memory>
#include <unordered_map>
#include "Cloneable.hpp"

namespace xacc {
namespace quantum {
class FermionOperator;
}
} // namespace xacc

bool operator==(const xacc::quantum::FermionOperator &lhs,
                const xacc::quantum::FermionOperator &rhs);

namespace xacc {
namespace quantum {
using Operator = std::pair<int, bool>;
using Operators = std::vector<Operator>;
// using SiteMap = std::map<Operators, std::complex<double>>;

using FermionTermTuple = std::tuple<std::complex<double>, Operators, std::string>;
class FermionTerm : public FermionTermTuple,
             public tao::operators::commutative_multipliable<FermionTerm>,
             public tao::operators::equality_comparable<FermionTerm> {

public:
  FermionTerm() {
    std::get<0>(*this) = std::complex<double>(1.0, 0.0);
    std::get<1>(*this) = {};
    std::get<2>(*this) = "";
  }

  FermionTerm(const FermionTerm &t) {
    std::get<0>(*this) = std::get<0>(t);
    std::get<1>(*this) = std::get<1>(t);
    std::get<2>(*this) = std::get<2>(t);
  }

  FermionTerm(std::complex<double> c) {
    std::get<0>(*this) = c;
    std::get<1>(*this) = {};
    std::get<2>(*this) = "";
  }

 FermionTerm(double c) {
    std::get<0>(*this) = std::complex<double>(c,0.0);
    std::get<1>(*this) = {};
    std::get<2>(*this) = "";

  }

  FermionTerm(std::complex<double> c, Operators ops) {
    std::get<0>(*this) = c;
    std::get<1>(*this) = ops;
    std::get<2>(*this) = "";
  }

 FermionTerm(std::complex<double> c, Operators ops, std::string var) {
    std::get<0>(*this) = c;
    std::get<1>(*this) = ops;
    std::get<2>(*this) = var;
  }
  FermionTerm(Operators ops) {
    std::get<0>(*this) = std::complex<double>(1.0, 0.0);
    std::get<1>(*this) = ops;
    std::get<2>(*this) = "";
  }

  static const std::string id(const Operators &ops) {

    std::stringstream s;
    for (auto &t : ops) {
      if (t.second) {
        s << t.first << "^" << std::string(" ");
      } else {
        s << t.first << std::string(" ");
      }
    }

    if (s.str().empty()) {
      return "I";
    }

    return s.str();
  }

  const std::string id() {

    std::stringstream s;
    for (auto &t : ops()) {
      if (t.second) {
        s << t.first << "^" << std::string(" ");
      } else {
        s << t.first << std::string(" ");
      }
    }

    if (s.str().empty()) {
      return "I";
    }

    return s.str();
  }

  Operators &ops() { return std::get<1>(*this); }

  std::complex<double> &coeff() { return std::get<0>(*this); }
  std::string var() {return std::get<2>(*this);}

  FermionTerm &operator*=(const FermionTerm &v) noexcept;

  bool operator==(const FermionTerm &v) noexcept {
    return (std::get<1>(*this) == std::get<1>(v));
  }
};

class FermionOperator
    : public Observable, public Cloneable<Observable>,
      public std::enable_shared_from_this<FermionOperator>,
      public tao::operators::commutative_ring<FermionOperator>,
      public tao::operators::equality_comparable<FermionOperator>,
      public tao::operators::commutative_multipliable<FermionOperator, double>,
      public tao::operators::commutative_multipliable<FermionOperator,
                                                      std::complex<double>> {

protected:
  std::unordered_map<std::string, FermionTerm> terms;

public:
  std::shared_ptr<Observable> clone() override {
      return std::make_shared<FermionOperator>();
  }
  FermionOperator();
  FermionOperator(std::complex<double> c);
  FermionOperator(double c);
  FermionOperator(std::string fromString);
  FermionOperator(const FermionOperator &i);
  FermionOperator(Operators operators);
  FermionOperator(Operators operators, std::complex<double> coeff);
  FermionOperator(Operators operators, double coeff);
  FermionOperator(Operators operators, double coeff, std::string var);

  std::vector<std::shared_ptr<CompositeInstruction>>
  observe(std::shared_ptr<CompositeInstruction> function) override;
  const std::string toString() override;
  void fromString(const std::string str) override;
  const int nBits() override;

  void clear();
  std::unordered_map<std::string, FermionTerm> getTerms() const { return terms; }

  FermionOperator &operator+=(const FermionOperator &v) noexcept;
  FermionOperator &operator-=(const FermionOperator &v) noexcept;
  FermionOperator &operator*=(const FermionOperator &v) noexcept;
  bool operator==(const FermionOperator &v) noexcept;
  FermionOperator &operator*=(const double v) noexcept;
  FermionOperator &operator*=(const std::complex<double> v) noexcept;

  const std::string name() const override {
      return "fermion";
  }

  const std::string description() const override {
      return "";
  }
  void fromOptions(const HeterogeneousMap& options) override {
      return;
  }

  std::shared_ptr<Observable>
  commutator(std::shared_ptr<Observable> obs) override;

  FermionOperator hermitianConjugate() const;

  void normalize() override;
  virtual double postProcess(std::shared_ptr<AcceleratorBuffer> buffer,
                             const std::string &postProcessTask,
                             const HeterogeneousMap &extra_data) override;
};

} // namespace quantum
} // namespace xacc
#endif