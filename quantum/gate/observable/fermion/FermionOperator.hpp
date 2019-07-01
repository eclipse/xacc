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

using FermionTermTuple = std::tuple<std::complex<double>, Operators>;
class FermionTerm : public FermionTermTuple,
             public tao::operators::commutative_multipliable<FermionTerm>,
             public tao::operators::equality_comparable<FermionTerm> {

public:
  FermionTerm() {
    std::get<0>(*this) = std::complex<double>(1.0, 0.0);
    std::get<1>(*this) = {};
  }

  FermionTerm(const FermionTerm &t) {
    std::get<0>(*this) = std::get<0>(t);
    std::get<1>(*this) = std::get<1>(t);
  }

  FermionTerm(std::complex<double> c) {
    std::get<0>(*this) = c;
    std::get<1>(*this) = {};
  }

 FermionTerm(double c) {
    std::get<0>(*this) = std::complex<double>(c,0.0);
    std::get<1>(*this) = {};
  }

  FermionTerm(std::complex<double> c, Operators ops) {
    std::get<0>(*this) = c;
    std::get<1>(*this) = ops;
  }

  FermionTerm(Operators ops) {
    std::get<0>(*this) = std::complex<double>(1.0, 0.0);
    std::get<1>(*this) = ops;
  }

  static const std::string id(const Operators &ops) {
    std::vector<int> creations, annhilations;
    for (auto &t : ops) {
      if (t.second) {
        creations.push_back(t.first);
      } else {
        annhilations.push_back(t.first);
      }
    }

    // std::sort(creations.rbegin(), creations.rend());
    // std::sort(annhilations.rbegin(), annhilations.rend());

    std::stringstream s;
    for (auto &t : creations) {
      s << t << "^" << std::string(" ");
    }
    for (auto &t : annhilations) {
      s << t << std::string(" ");
    }

    if (s.str().empty()) {
      return "I";
    }

    return s.str();
  }

  const std::string id() {
    std::vector<int> creations, annhilations;
    for (auto &t : ops()) {
      if (t.second) {
        creations.push_back(t.first);
      } else {
        annhilations.push_back(t.first);
      }
    }

    // std::sort(creations.rbegin(), creations.rend());
    // std::sort(annhilations.rbegin(), annhilations.rend());

    std::stringstream s;
    for (auto &t : creations) {
      s << t << "^" << std::string(" ");
    }

    for (auto &t : annhilations) {
      s << t << std::string(" ");
    }

    if (s.str().empty()) {
      return "I";
    }

    return s.str();
  }

  Operators &ops() { return std::get<1>(*this); }

  std::complex<double> &coeff() { return std::get<0>(*this); }

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

  std::vector<std::shared_ptr<Function>>
  observe(std::shared_ptr<Function> function) override;
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

};

} // namespace quantum
} // namespace xacc
#endif