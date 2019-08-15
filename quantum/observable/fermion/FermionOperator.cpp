#include "FermionOperator.hpp"

#include "FermionOperatorLexer.h"
#include "FermionListenerImpl.hpp"

#include "ObservableTransform.hpp"
#include "xacc_service.hpp"

namespace xacc {
namespace quantum {

FermionTerm &FermionTerm::operator*=(const FermionTerm &v) noexcept {
  coeff() *= std::get<0>(v);

//   std::cout << "FermionTerm: " << id() << ", " << FermionTerm::id(std::get<1>(v)) << "\n";
  auto otherOps = std::get<1>(v);
  for (auto &kv : otherOps) {
    auto site = kv.first;
    auto c_or_a = kv.second;
    // std::cout << "HELLO: " << site << ", " << std::boolalpha << c_or_a << "\n";
    Operators o = ops();
    if (!o.empty()) {
    auto it = std::find_if(o.begin(), o.end(),
                           [&](const std::pair<int, bool> &element) {
                             return element.first == site;
                           });
    // std::cout << it->first << ", " << std::boolalpha << it->second << "\n";
    if (it->first == site) {
      if (it->second && c_or_a) {
        // zero out this FermionTerm
        ops().clear();
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

    std::vector<int> creations, annhilations;
    for (auto &t : ops) {
        // std::cout << "tostring " << t.first << ", " << t.second << "\n";
      if (t.second) {
        creations.push_back(t.first);
      } else {
        annhilations.push_back(t.first);
      }
    }

    std::sort(creations.rbegin(), creations.rend());
    std::sort(annhilations.rbegin(), annhilations.rend());

    for (auto &t : creations) {
      s << t << "^" << std::string(" ");
    }
    for (auto &t : annhilations) {
      s << t << std::string(" ");
    }

    s << "+ ";
  }

//   std::cout << "tostring " << s.str() << "\n";
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
      auto multTerm = kv.second * vkv.second;
      if (!multTerm.ops().empty()) {
        auto id = multTerm.id();

        if (!newTerms.insert({id, multTerm}).second) {
          newTerms.at(id).coeff() += multTerm.coeff();
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