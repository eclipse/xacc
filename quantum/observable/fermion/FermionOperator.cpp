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

#include "FermionOperatorLexer.h"
#include "FermionListenerImpl.hpp"

#include "ObservableTransform.hpp"
#include "xacc_service.hpp"

namespace xacc {
namespace quantum {

FermionTerm &FermionTerm::operator*=(const FermionTerm &v) noexcept {
  coeff() *= std::get<0>(v);

   //std::cout << "FermionTerm: " << id() << ", " << FermionTerm::id(std::get<1>(v)) << "\n";
  auto otherOps = std::get<1>(v);
  for (auto &kv : otherOps) {
    auto site = kv.first;
    auto c_or_a = kv.second;
    //std::cout << "\n\nHELLO: " << site << ", " << std::boolalpha << c_or_a << "\n";
    Operators o = ops();    
    if (!o.empty()) {

      auto it = std::find_if(o.begin(), o.end(),
                           [&](const std::pair<int, bool> &element) {
                             return element.first == site;
                           });
      //std::cout << it->first << ", " << std::boolalpha << it->second << "\n";
      if (it->first == site) {
        if (it->second == c_or_a) {
        // zero out this FermionTerm
          ops().clear();
        } else {//this adds the adjoint of operators whose sites are already in the product
          ops().push_back({site, c_or_a});
        }

      } else {
        ops().push_back({site, c_or_a});
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


FermionOperator::FermionOperator(Operators operators, double coeff, std::string var)
    {
          terms.emplace(std::piecewise_construct,
                std::forward_as_tuple(FermionTerm::id(operators)),
                std::forward_as_tuple(std::complex<double>(coeff,0.0), operators, var));
    }

void FermionOperator::clear() { terms.clear(); }

std::vector<std::shared_ptr<CompositeInstruction>>
FermionOperator::observe(std::shared_ptr<CompositeInstruction> function) {
    auto transform = xacc::getService<ObservableTransform>("jw");
    return transform->transform(shared_from_this())->observe(function);
}

const std::string FermionOperator::toString() {
  std::stringstream s;
  for (auto &kv : terms) {
    std::complex<double> c = std::get<0>(kv.second);
    s << c << " " << std::get<2>(kv.second) << " ";
    Operators ops = std::get<1>(kv.second);

    // I changed this to get the operators printed in the order they're built
    /*
    std::vector<int> creations, annhilations;
    for (auto &t : ops) {
      if (t.second) {
        creations.push_back(t.first);
      } else {
        annhilations.push_back(t.first);
      }
    }
    for (auto &t : creations) {
      s << t << "^" << std::string(" ");
    }
    for (auto &t : annhilations) {
      s << t << std::string(" ");
    }*/

    for (auto &t : ops) {
      if (t.second) {
        s << t.first << "^" << std::string(" ");
      }
      else{
        s << t.first << std::string(" ");
      }
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
  if (terms.empty())
    return 0;

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

FermionOperator &
FermionOperator::operator+=(const FermionOperator &v) noexcept {
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

FermionOperator &
FermionOperator::operator-=(const FermionOperator &v) noexcept {
  return operator+=(-1.0 * v);
}

FermionOperator &
FermionOperator::operator*=(const FermionOperator &v) noexcept {

  std::unordered_map<std::string, FermionTerm> newTerms;
  for (auto &kv : terms) {
    for (auto &vkv : v.terms) {

      FermionTerm multTerm;
      if (kv.first == "I"){
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
          newTerms.at(id).coeff() = std::get<0>(kv.second) * std::get<0>(vkv.second);
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

FermionOperator &
FermionOperator::operator*=(const std::complex<double> v) noexcept {
  for (auto &kv : terms) {
    std::get<0>(kv.second) *= v;
  }
  return *this;
}

std::shared_ptr<Observable> FermionOperator::commutator(std::shared_ptr<Observable> op) {

  FermionOperator& A = *std::dynamic_pointer_cast<FermionOperator>(op);
  std::shared_ptr<FermionOperator> commutatorHA = std::make_shared<FermionOperator>((*this) * A - A * (*this));
  return commutatorHA;
}

} // namespace quantum
} // namespace xacc

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