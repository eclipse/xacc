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
#include "PauliOperator.hpp"
#include "IRProvider.hpp"
#include <regex>
#include <set>
#include <iostream>
#include "xacc.hpp"
#include "xacc_service.hpp"

#include <Eigen/Core>

#include "PauliOperatorLexer.h"
#include "PauliListenerImpl.hpp"

namespace xacc {
namespace quantum {
// const std::map<std::string, std::pair<c, std::string>> Term:: pauliProducts =
// Term::create_map();

PauliOperator::PauliOperator() {}

PauliOperator::PauliOperator(std::complex<double> c) {
  terms.emplace(std::make_pair("I", c));
}

PauliOperator::PauliOperator(double c) {
  terms.emplace(std::make_pair("I", c));
}

PauliOperator::PauliOperator(std::string fromStr) {
  fromString(fromStr);
}

PauliOperator::PauliOperator(std::complex<double> c, std::string var) {
  terms.emplace(std::piecewise_construct, std::forward_as_tuple("I"),
                std::forward_as_tuple(c, var));
}

PauliOperator::PauliOperator(const PauliOperator &i) : terms(i.terms) {}

/**
 * The Constructor, takes a vector of
 * qubit-gatename pairs. Initializes coefficient to 1
 *
 * @param operators The pauli operators making up this SpinInstruction
 */
PauliOperator::PauliOperator(std::map<int, std::string> operators) {
  terms.emplace(std::make_pair(Term::id(operators), operators));
}

PauliOperator::PauliOperator(std::map<int, std::string> operators,
                             std::string var) {
  terms.emplace(std::piecewise_construct,
                std::forward_as_tuple(Term::id(operators, var)),
                std::forward_as_tuple(var, operators));
}

/**
 * The Constructor, takes a vector of
 * qubit-gatename pairs and this instruction's coefficient
 *
 * @param operators
 * @param coeff
 */
PauliOperator::PauliOperator(std::map<int, std::string> operators,
                             std::complex<double> coeff) {
  terms.emplace(std::piecewise_construct,
                std::forward_as_tuple(Term::id(operators)),
                std::forward_as_tuple(coeff, operators));
}

PauliOperator::PauliOperator(std::map<int, std::string> operators, double coeff)
    : PauliOperator(operators, std::complex<double>(coeff, 0)) {}

PauliOperator::PauliOperator(std::map<int, std::string> operators,
                             std::complex<double> coeff, std::string var) {
  terms.emplace(std::piecewise_construct,
                std::forward_as_tuple(Term::id(operators, var)),
                std::forward_as_tuple(coeff, var, operators));
}

std::vector<std::shared_ptr<CompositeInstruction>>
PauliOperator::observe(std::shared_ptr<CompositeInstruction> function) {

    // Create a new GateQIR to hold the spin based terms
  auto gateRegistry = xacc::getService<IRProvider>("quantum");
  std::vector<std::shared_ptr<CompositeInstruction>> observed;
  int counter = 0;
  auto pi = 3.141592653589793238;

  // Populate GateQIR now...
  for (auto &inst : terms) {

    Term spinInst = inst.second;

    auto gateFunction = gateRegistry->createComposite(
        inst.first, function->getVariables());

    gateFunction->setCoefficient(spinInst.coeff());

    if (function->hasChildren()) {
      gateFunction->addInstruction(function);
    }

    // Loop over all terms in the Spin Instruction
    // and create instructions to run on the Gate QPU.
    std::vector<std::shared_ptr<xacc::Instruction>> measurements;
    auto termsMap = spinInst.ops();

    std::vector<std::pair<int, std::string>> terms;
    for (auto &kv : termsMap) {
      if (kv.second != "I" && !kv.second.empty()) {
        terms.push_back({kv.first, kv.second});
      }
    }

    for (int i = terms.size() - 1; i >= 0; i--) {
      auto qbit = terms[i].first;
      int t = qbit;
      std::size_t tt = t;
      auto gateName = terms[i].second;
      auto meas =
          gateRegistry->createInstruction("Measure", std::vector<std::size_t>{tt});
      xacc::InstructionParameter classicalIdx(qbit);
      meas->setParameter(0, classicalIdx);
      measurements.push_back(meas);

      if (gateName == "X") {
        auto hadamard =
            gateRegistry->createInstruction("H", std::vector<std::size_t>{tt});
        gateFunction->addInstruction(hadamard);
      } else if (gateName == "Y") {
        auto rx = gateRegistry->createInstruction("Rx", std::vector<std::size_t>{tt});
        InstructionParameter p(pi / 2.0);
        rx->setParameter(0, p);
        gateFunction->addInstruction(rx);
      }
    }

    if (!spinInst.isIdentity()) {
      for (auto m : measurements) {
        gateFunction->addInstruction(m);
      }
    }

    observed.push_back(gateFunction);
    counter++;
  }
  return observed;
}

std::pair<std::vector<int>, std::vector<int>>
Term::toBinaryVector(const int nQubits) {
  // return v,w
  std::vector<int> v(nQubits), w(nQubits);

  for (auto &kv : ops()) {
    auto site = kv.first;
    auto pauli = kv.second;
    if (pauli == "X") {
      w[site] = 1;
    } else if (pauli == "Z") {
      v[site] = 1;
    } else if (pauli == "Y") {
      v[site] = 1;
      w[site] = 1;
    }
  }

  return {v, w};
}

std::vector<Triplet> PauliOperator::getSparseMatrixElements() {

  // Get number of qubits
  std::set<int> distinctSites;
  for (auto &kv : terms) {
    for (auto &kv2 : kv.second.ops()) {
      distinctSites.insert(kv2.first);
    }
  }

  auto nQubits = distinctSites.size();

  std::vector<Triplet> triplets;

  for (auto &kv : terms) {
    auto termTrips = kv.second.getSparseMatrixElements(nQubits);
    triplets.insert(std::end(triplets), std::begin(termTrips),
                    std::end(termTrips));
  }

  return triplets;
}

ActionResult Term::action(const std::string &bitString, ActionType type) {

  auto _coeff = coeff();
  auto newBits = bitString;
  c i(0, 1);

  for (auto &t : ops()) {
    auto idx = t.first;
    auto gate = t.second;

    if (gate == "Z") {
      _coeff *= newBits[idx] == '1' ? -1 : 1;
    } else if (gate == "X") {
      newBits[idx] = (newBits[idx] == '1' ? '0' : '1');
    } else if (gate == "Y") {
      if (type == ActionType::Bra) {
        _coeff *= newBits[idx] == '1' ? i : -i;
      } else {
        _coeff *= newBits[idx] == '1' ? -i : i;
      }
      newBits[idx] = (newBits[idx] == '1' ? '0' : '1');
    }
  }

  return {newBits, _coeff};
}

const std::vector<std::pair<std::string, std::complex<double>>>
PauliOperator::computeActionOnKet(const std::string &bitString) {

  std::vector<std::pair<std::string, std::complex<double>>> ret;
  std::string newBits = bitString;
  std::complex<double> newCoeff(1, 0), i(0, 1);

  for (auto &kv : terms) {
    ret.push_back(kv.second.action(bitString, ActionType::Ket));
  }
  return ret;
}

const std::vector<std::pair<std::string, std::complex<double>>>
PauliOperator::computeActionOnBra(const std::string &bitString) {
  std::vector<std::pair<std::string, std::complex<double>>> ret;

  for (auto &kv : terms) {
    ret.push_back(kv.second.action(bitString, ActionType::Bra));
  }

  return ret;
}

std::vector<std::complex<double>> PauliOperator::toDenseMatrix(const int n) {
  std::size_t dim = 1;
  std::size_t two = 2;
  for (int i = 0; i < n; i++)
    dim *= two;
  auto getBitStrForIdx = [&](std::uint64_t i) {
    std::stringstream s;
    for (int k = n - 1; k >= 0; k--)
      s << ((i >> k) & 1);
    return s.str();
  };

  Eigen::MatrixXcd A(dim, dim);
  A.setZero();
  for (std::uint64_t myRow = 0; myRow < dim; myRow++) {
    auto rowBitStr = getBitStrForIdx(myRow);
    auto results = computeActionOnBra(rowBitStr);
    for (auto &result : results) {
      std::uint64_t k = std::stol(result.first, nullptr, 2);
      A(myRow, k) += result.second;
    }
  }

  std::vector<std::complex<double>> retv(dim*dim);
  Eigen::MatrixXcd::Map(&retv.data()[0], A.rows(),A.cols()) = A;

  return retv;
}

const int PauliOperator::nTerms() { return terms.size(); }

bool PauliOperator::isClose(PauliOperator &other) {
  if (!operator==(other)) {
    return false;
  }

  // These are equal, check their coeffs
  for (auto &kv : terms) {
    Term otherTerm = other.terms[kv.first];
    if (std::abs(kv.second.coeff() - otherTerm.coeff()) > 1e-6) {
      return false;
    }
  }

  return true;
}

/**
 * Persist this Instruction to an assembly-like
 * string.
 *
 * @param bufferVarName The name of the AcceleratorBuffer
 * @return str The assembly-like string.
 */
const std::string PauliOperator::toString() {
  std::stringstream s;
  for (auto &kv : terms) {
    std::complex<double> c = std::get<0>(kv.second);
    std::string v = std::get<1>(kv.second);
    std::map<int, std::string> ops = std::get<2>(kv.second);

    s << c << " ";
    if (!v.empty()) {
      s << v << " ";
    }

    for (auto &kv2 : ops) {
      if (kv2.second == "I") {
        s << "I ";
      } else {
        s << kv2.second << kv2.first << " ";
      }
    }

    s << "+ ";
  }

  auto r = s.str().substr(0, s.str().size() - 2);
  xacc::trim(r);
  return r;
}

void PauliOperator::fromString(const std::string str) {
  using namespace antlr4;
  using namespace pauli;

  ANTLRInputStream input(str);
  PauliOperatorLexer lexer(&input);
  lexer.removeErrorListeners();
  lexer.addErrorListener(new PauliOperatorErrorListener());

  CommonTokenStream tokens(&lexer);
  PauliOperatorParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(new PauliOperatorErrorListener());

  // Walk the Abstract Syntax Tree
  tree::ParseTree *tree = parser.pauliSrc();

  PauliListenerImpl listener;
  tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  clear();

  operator+=(listener.getOperator());

}

bool PauliOperator::contains(PauliOperator &op) {
  if (op.nTerms() > 1)
    xacc::error("Cannot check PauliOperator.contains for more than 1 term.");
  for (auto &term : getTerms()) {
    if (op.terms.count(term.first)) {
      return true;
    }
  }
  return false;
}

bool PauliOperator::commutes(PauliOperator &op) {
  return (op * (*this) - (*this) * op).nTerms() == 0;
}

void PauliOperator::clear() { terms.clear(); }

PauliOperator &PauliOperator::operator+=(const PauliOperator &v) noexcept {
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

  return *this;
}

PauliOperator &PauliOperator::operator-=(const PauliOperator &v) noexcept {
  return operator+=(-1.0 * v);
}

PauliOperator &PauliOperator::operator*=(const PauliOperator &v) noexcept {

  std::unordered_map<std::string, Term> newTerms;
  for (auto &kv : terms) {
    for (auto &vkv : v.terms) {
      auto multTerm = kv.second * vkv.second;
      auto id = multTerm.id();

      if (!newTerms.insert({id, multTerm}).second) {
        newTerms.at(id).coeff() += multTerm.coeff();
      }

      if (std::abs(newTerms.at(id).coeff()) < 1e-12) {
        newTerms.erase(id);
      }
    }
  }
  terms = newTerms;
  return *this;
}

bool PauliOperator::operator==(const PauliOperator &v) noexcept {
  if (terms.size() != v.terms.size()) {
    return false;
  }

  for (auto &kv : terms) {
    bool found = false;
    for (auto &vkv : v.terms) {

      if (kv.second.operator==(vkv.second) | (kv.second.id() == "I" && vkv.second.id() == "I")) {
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

PauliOperator &PauliOperator::operator*=(const double v) noexcept {
  return operator*=(std::complex<double>(v, 0));
}

PauliOperator &
PauliOperator::operator*=(const std::complex<double> v) noexcept {
  for (auto &kv : terms) {
    std::get<0>(kv.second) *= v;
  }
  return *this;
}

std::vector<Triplet> Term::getSparseMatrixElements(const int nQubits) {

  // X = |1><0| + |0><1|
  // Y = -i|1><0| + i|0><1|
  // Z = |0><0| - |1><1|

  auto comb = [](int N, int K) -> std::vector<std::vector<int>> {
    std::string bitmask(K, 1); // K leading 1's
    bitmask.resize(N, 0);      // N-K trailing 0's

    std::vector<std::vector<int>> allCombinations;
    // print integers and permute bitmask
    do {
      std::vector<int> bits;
      for (int i = 0; i < N; ++i) {
        if (bitmask[i]) {
          bits.push_back(i);
        }

        //					if (bitmask[i]) std::cout << " "
        //<< i;
      }

      bool add = true;
      for (int i = 1; i < bits.size(); i++) {
        if (bits[i] % 2 != 0 && std::abs(bits[i] - bits[i - 1]) == 1) {
          add = false;
          break;
        }
      }

      if (add) {
        allCombinations.push_back(bits);
      }
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));

    // for (auto a : allCombinations) {
    //   for (auto b : a) {
    //     std::cout << b << " ";
    //   }
    //   std::cout << "\n";
    // }

    // std::cout << (3 / 2) << "\n";
    return allCombinations;
  };

  auto nSites = ops().size();
  auto termCombinations = comb(2 * nSites, nSites);

  std::map<std::string, std::vector<std::pair<int, int>>> subterms;
  subterms["X"] = {{1, 0}, {0, 1}};
  subterms["Y"] = {{1, 0}, {0, 1}};
  subterms["Z"] = {{0, 0}, {1, 1}};

  std::string zeroStr = "";
  for (int i = 0; i < nQubits; i++)
    zeroStr += "0";

  auto ket = zeroStr;
  auto bra = zeroStr;

  std::vector<Triplet> triplets;
  for (auto &combo : termCombinations) {

    std::complex<double> coeff(1, 0), i(0, 1);
    for (auto &c : combo) {

      auto iter = ops().begin();
      std::advance(iter, c / 2);
      auto ithOp = iter->second;
      auto ithOpSite = iter->first;
      auto x = (c % 2);

      //			Now I know the Operator and whether this is hte
      // 0 or 1st term for that op

      auto term = subterms[ithOp][x];

      ket[ithOpSite] = term.first ? '1' : '0';
      bra[ithOpSite] = term.second ? '1' : '0';

      if (ithOp == "Y") {
        if (x) {
          coeff *= i;
        } else {
          coeff *= -i;
        }
      } else if (ithOp == "Z") {
        if (x) {
          coeff *= -1;
        }
      }
    }

    // now convert bra and ket into integers
    auto row = std::stol(bra, nullptr, 2);
    auto col = std::stol(ket, nullptr, 2);

    triplets.emplace_back(row, col, coeff);
  }

  return triplets;
}

Term &Term::operator*=(const Term &v) noexcept {

  coeff() *= std::get<0>(v);

  std::string ss;
  std::string myVar = std::get<1>(*this);
  std::string otherVar = std::get<1>(v);
  if (!myVar.empty()) {
    if (!otherVar.empty()) {
      ss = myVar + " " + otherVar;
    } else {
      ss = myVar;
    }
  } else {
    if (!otherVar.empty()) {
      ss = otherVar;
    }
  }

  std::get<1>(*this) = ss;

  auto otherOps = std::get<2>(v);
  for (auto &kv : otherOps) {
    auto qubit = kv.first;
    auto gate = kv.second;
    if (ops().count(qubit)) {
      // This means, we have a op on same qubit in both
      // so we need to check its product
      auto myGate = ops().at(qubit);
      auto gate_coeff = pauliProducts.at(myGate + gate);
      if (gate_coeff.second != "I") {
        ops().at(kv.first) = gate_coeff.second;
      } else {
        ops().erase(kv.first);
      }
      coeff() *= gate_coeff.first;
    } else if (gate != "I") {
      ops().emplace(std::make_pair(qubit, gate));
    }
  }

  return *this;
}

PauliOperator PauliOperator::eval(
    const std::map<std::string, std::complex<double>> varToValMap) {
  PauliOperator ret;

  for (auto &kv : terms) {

    auto id = kv.first;
    auto term = kv.second;

    for (auto &varVal : varToValMap) {
      if (varVal.first == term.var()) {
        term.var() = "";
        term.coeff() *= varVal.second;
      }
    }

    ret.terms.insert({id, term});
  }

  return ret;
}

std::shared_ptr<IR> PauliOperator::toXACCIR() {
  // Create a new GateQIR to hold the spin based terms

  auto gateRegistry = xacc::getService<IRProvider>("quantum");
  auto tmp = gateRegistry->createComposite("tmp");
  auto kernels = observe(tmp);
  auto newIr = gateRegistry->createIR();
  for (auto& k : kernels) newIr->addComposite(k);
  return newIr;


}

int PauliOperator::nQubits() {
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

void PauliOperator::fromXACCIR(std::shared_ptr<IR> ir) {

  terms.clear();

  for (auto &kernel : ir->getComposites()) {
    std::map<int, std::string> pauliTerm;
    for (auto inst : kernel->getInstructions()) {

      bool seen = false;
      if (!inst->isComposite()) {

        if (inst->name() == "H") {
          pauliTerm.insert({inst->bits()[0], "X"});
        } else if (inst->name() == "Rx") {
          pauliTerm.insert({inst->bits()[0], "Y"});
        }

        if (pauliTerm.count(inst->bits()[0]) == 0) {
          pauliTerm.insert({inst->bits()[0], "Z"});
        }
      }
    }
    std::complex<double> c = kernel->getCoefficient();

    if (pauliTerm.empty())
      pauliTerm.insert({0, "I"});

    Term t(c, pauliTerm);
    terms.insert({t.id(), t});
  }
}

} // namespace quantum
} // namespace xacc

bool operator==(const xacc::quantum::PauliOperator &lhs,
                const xacc::quantum::PauliOperator &rhs) {
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
