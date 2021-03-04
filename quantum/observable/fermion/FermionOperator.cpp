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
#include "FermionOperator.hpp"

#include <Utils.hpp>

#include "FermionListenerImpl.hpp"
#include "FermionOperatorLexer.h"
#include "ObservableTransform.hpp"
#include "xacc_service.hpp"

namespace xacc {
namespace quantum {

FermionTerm &FermionTerm::operator*=(const FermionTerm &v) noexcept {
  coeff() *= std::get<0>(v);
  auto otherOps = std::get<1>(v);
  for (auto &kv : otherOps) {
    auto site = kv.first;
    auto c_or_a = kv.second;
    Operators o = ops();
    for (auto oo : o ) {
      if (oo.first == site) {
        if (oo.second == c_or_a) {
          ops().clear();
        } else {
          ops().push_back({site, c_or_a});
        }
        break;
      }
      else {
        ops().push_back({site, c_or_a});
        break;
      }
    }
    // This means, we have a op on same qubit in both
  }

  return *this;
}

FermionOperator::FermionOperator() {}

FermionOperator::FermionOperator(std::complex<double> c) {
  terms.emplace(std::make_pair("I", c));
}

FermionOperator::FermionOperator(double c) {
  terms.emplace(std::make_pair("I", c));
}

FermionOperator::FermionOperator(std::string fromStr) { fromString(fromStr); }

FermionOperator::FermionOperator(const FermionOperator &i) : terms(i.terms) {}

FermionOperator::FermionOperator(Operators operators) {
  terms.emplace(std::make_pair(FermionTerm::id(operators), operators));
}

FermionOperator::FermionOperator(Operators operators,
                                 std::complex<double> coeff) {
  terms.emplace(std::piecewise_construct,
                std::forward_as_tuple(FermionTerm::id(operators)),
                std::forward_as_tuple(coeff, operators));
}

FermionOperator::FermionOperator(Operators operators, double coeff)
    : FermionOperator(operators, std::complex<double>(coeff, 0)) {}

FermionOperator::FermionOperator(Operators operators, double coeff,
                                 std::string var) {
  terms.emplace(
      std::piecewise_construct,
      std::forward_as_tuple(FermionTerm::id(operators)),
      std::forward_as_tuple(std::complex<double>(coeff, 0.0), operators, var));
}

void FermionOperator::clear() { terms.clear(); }

std::vector<std::shared_ptr<CompositeInstruction>> FermionOperator::observe(
    std::shared_ptr<CompositeInstruction> function) {
  auto transform = xacc::getService<ObservableTransform>("jw");
  return transform->transform(xacc::as_shared_ptr(this))->observe(function);
}

const std::string FermionOperator::toString() {
  std::stringstream s;
  for (auto &kv : terms) {
    std::complex<double> c = std::get<0>(kv.second);
    s << c << " ";
    if (kv.first != "I") {
      s << kv.first;
    }
    s << "+ ";
  }

  auto r = s.str().substr(0, s.str().size() - 2);
  xacc::trim(r);
  return r;
}

void FermionOperator::fromString(const std::string str) {
  using namespace antlr4;
  using namespace fermion;

  ANTLRInputStream input(str);
  FermionOperatorLexer lexer(&input);
  lexer.removeErrorListeners();
  lexer.addErrorListener(new FermionOperatorErrorListener());

  CommonTokenStream tokens(&lexer);
  FermionOperatorParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(new FermionOperatorErrorListener());

  // Walk the Abstract Syntax Tree
  tree::ParseTree *tree = parser.fermionSrc();

  FermionListenerImpl listener;
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  clear();

  operator+=(listener.getOperator());
}
const int FermionOperator::nBits() {
  auto maxInt = 0;
  if (terms.empty()) return 0;

  for (auto &kv : terms) {
    auto ops = kv.second.ops();
    for (auto &kv2 : ops) {
      if (maxInt < kv2.first) {
        maxInt = kv2.first;
      }
    }
  }
  return maxInt + 1;
}

FermionOperator &FermionOperator::operator+=(
    const FermionOperator &v) noexcept {
  FermionOperator vv = v;
  for (auto &kv : v.terms) {
    auto termId = kv.first;
    auto otherTerm = kv.second;

    if (terms.count(termId)) {
      terms.at(termId).coeff() += otherTerm.coeff();
    } else {
      terms.insert({termId, otherTerm});
    }

    if (std::abs(terms[termId].coeff()) < 1e-12) {
      terms.erase(termId);
    }
  }

  //   std::cout << "result: " << toString() << "\n";

  return *this;
}

FermionOperator &FermionOperator::operator-=(
    const FermionOperator &v) noexcept {
  return operator+=(-1.0 * v);
}

FermionOperator &FermionOperator::operator*=(
    const FermionOperator &v) noexcept {
  std::unordered_map<std::string, FermionTerm> newTerms;
  for (auto &kv : terms) {
    for (auto &vkv : v.terms) {
      FermionTerm multTerm;
      if (kv.first == "I") {
        multTerm = vkv.second;
      } else {
        multTerm = kv.second * vkv.second;
      }

      if (!multTerm.ops().empty()) {
        auto id = multTerm.id();
        if (!newTerms.insert({id, multTerm}).second && kv.first != "I") {
          newTerms.at(id).coeff() += multTerm.coeff();
        }

        if (!newTerms.insert({id, multTerm}).second && kv.first == "I") {
          newTerms.at(id).coeff() =
              std::get<0>(kv.second) * std::get<0>(vkv.second);
        }

        if (std::abs(newTerms.at(id).coeff()) < 1e-12) {
          newTerms.erase(id);
        }
      }
    }
  }

  terms = newTerms;
  return *this;
}

bool FermionOperator::operator==(const FermionOperator &v) noexcept {
  if (terms.size() != v.terms.size()) {
    return false;
  }

  for (auto &kv : terms) {
    bool found = false;
    for (auto &vkv : v.terms) {
      if (kv.second.operator==(vkv.second)) {
        found = true;
        break;
      }
    }

    if (!found) {
      return false;
    }
  }

  return true;
}

FermionOperator &FermionOperator::operator*=(const double v) noexcept {
  return operator*=(std::complex<double>(v, 0));
}

FermionOperator &FermionOperator::operator*=(
    const std::complex<double> v) noexcept {
  for (auto &kv : terms) {
    std::get<0>(kv.second) *= v;
  }
  return *this;
}

std::shared_ptr<Observable> FermionOperator::commutator(
    std::shared_ptr<Observable> op) {
  FermionOperator &A = *std::dynamic_pointer_cast<FermionOperator>(op);
  std::shared_ptr<FermionOperator> commutatorHA =
      std::make_shared<FermionOperator>((*this) * A - A * (*this));
  return commutatorHA;
}

// We obtain the hermitian conjugate of FermionOperator
// by walking back on a string of FermionTerm and changing the boolean
FermionOperator FermionOperator::hermitianConjugate() const {
  FermionOperator conjugate;

  for (auto &kv : terms) {
    auto c = std::get<0>(kv.second);
    Operators ops = std::get<1>(kv.second), hcOps;
    for (int i = ops.size() - 1; i >= 0; i--) {
      if (ops[i].second) {
        hcOps.push_back({ops[i].first, 0});
      } else {
        hcOps.push_back({ops[i].first, 1});
      }
    }

    conjugate += FermionOperator(hcOps, std::conj(c));
  }

  return conjugate;
}

void FermionOperator::normalize() {

  double norm = 0.0;

  for (auto &kv : terms) {
    norm += std::pow(std::norm(std::get<0>(kv.second)), 2);
  }
  norm = std::sqrt(norm);

  for (auto &kv : terms) {
    std::get<0>(kv.second) = std::get<0>(kv.second) / norm;
  }

  return;
}

double FermionOperator::postProcess(std::shared_ptr<AcceleratorBuffer> buffer,
                                    const std::string &postProcessTask,
                                    const HeterogeneousMap &extra_data) {
  auto transform = xacc::getService<ObservableTransform>("jw");
  return transform->transform(xacc::as_shared_ptr(this))->postProcess(buffer, postProcessTask, extra_data);
}

} // namespace quantum
}  // namespace xacc

bool operator==(const xacc::quantum::FermionOperator &lhs,
                const xacc::quantum::FermionOperator &rhs) {
  if (lhs.getTerms().size() != rhs.getTerms().size()) {
    return false;
  }
  for (auto &kv : lhs.getTerms()) {
    bool found = false;
    for (auto &vkv : rhs.getTerms()) {
      if (kv.second.operator==(vkv.second)) {
        found = true;
        break;
      }
    }

    if (!found) {
      return false;
    }
  }

  return true;
}