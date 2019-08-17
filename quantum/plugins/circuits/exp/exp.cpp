#include "exp.hpp"
#include "FermionOperator.hpp"
#include "IRProvider.hpp"
#include "ObservableTransform.hpp"
#include "PauliOperator.hpp"

#include "XACC.hpp"
#include "xacc_service.hpp"
#include <memory>
#include <regex>

using namespace xacc;
using namespace xacc::quantum;

namespace xacc {
namespace circuits {
const std::vector<std::string> Exp::requiredKeys() {
  return {"pauli", "fermion"};
}

bool Exp::expand(const HeterogeneousMap &parameters) {

  std::string pauli_or_fermion = "pauli";
  std::cout << "FOUND IT: " << parameters.get<const char *>("pauli") << "\n";
  if (!parameters.stringExists("pauli")) {
    if (!parameters.stringExists("fermion")) {
      return false;
    }
    pauli_or_fermion = "fermion";
  }

  std::string paramLetter = "t";
  if (parameters.stringExists("param_id")) {
    paramLetter = parameters.getString("param_id");
  }

  std::unordered_map<std::string, xacc::quantum::Term> terms;
  if (pauli_or_fermion == "fermion") {
    auto fermionStr = parameters.getString("fermion");
    auto op = std::make_shared<FermionOperator>(fermionStr);
    terms = std::dynamic_pointer_cast<PauliOperator>(
                xacc::getService<ObservableTransform>("jw")->transform(op))
                ->getTerms();
  } else {
    auto pauliStr = parameters.getString("pauli");
    PauliOperator op(pauliStr);
    terms = op.getTerms();
  }

  double pi = xacc::constants::pi;
  auto gateRegistry = xacc::getService<IRProvider>("quantum");
  addVariable(paramLetter);
  for (auto spinInst : terms) {
    // Get the individual pauli terms
    auto termsMap = std::get<2>(spinInst.second);

    std::vector<std::pair<int, std::string>> terms;
    for (auto &kv : termsMap) {
      if (kv.second != "I" && !kv.second.empty()) {
        terms.push_back({kv.first, kv.second});
      }
    }

    if (!terms.empty()) {
      // The largest qubit index is on the last term
      int largestQbitIdx = terms[terms.size() - 1].first;

      for (int i = 0; i < terms.size(); i++) {

        std::size_t qbitIdx = terms[i].first;
        std::string gateName = terms[i].second;

        if (i < terms.size() - 1) {
          std::size_t tmp = terms[i + 1].first;
          auto cnot = gateRegistry->createInstruction(
              "CNOT", std::vector<std::size_t>{qbitIdx, tmp});
          addInstruction(cnot);
        }

        if (gateName == "X") {
          auto hadamard = gateRegistry->createInstruction(
              "H", std::vector<std::size_t>{qbitIdx});
          insertInstruction(0, hadamard);
        } else if (gateName == "Y") {
          auto rx = gateRegistry->createInstruction(
              "Rx", std::vector<std::size_t>{qbitIdx});
          InstructionParameter p(pi / 2.0);
          rx->setParameter(0, p);
          insertInstruction(0, rx);
        }

        // Add the Rotation for the last term
        if (i == terms.size() - 1) {
          // FIXME DONT FORGET DIVIDE BY 2
          // std::stringstream ss;
          // ss << 2 * std::imag(std::get<0>(spinInst.second)) << " * "
          //    << std::get<1>(spinInst.second);

          auto rz = gateRegistry->createInstruction(
              "Rz", std::vector<std::size_t>{qbitIdx},
              {std::to_string(std::real(spinInst.second.coeff())) + " * " +
               paramLetter});

          // InstructionParameter p(ss.str());
          // rz->setParameter(0, p);
          addInstruction(rz);
        }
      }
    }

    int counter = nInstructions();
    // Add the instruction on the backend of the circuit
    for (int i = terms.size() - 1; i >= 0; i--) {

      std::size_t qbitIdx = terms[i].first;
      auto gateName = terms[i].second;

      if (i < terms.size() - 1) {
        std::size_t tmp = terms[i + 1].first;
        auto cnot = gateRegistry->createInstruction(
            "CNOT", std::vector<std::size_t>{qbitIdx, tmp});
        insertInstruction(counter, cnot);
        counter++;
      }

      if (gateName == "X") {
        auto hadamard = gateRegistry->createInstruction(
            "H", std::vector<std::size_t>{qbitIdx});
        addInstruction(hadamard);
      } else if (gateName == "Y") {
        auto rx = gateRegistry->createInstruction(
            "Rx", std::vector<std::size_t>{qbitIdx});
        InstructionParameter p(-1.0 * (pi / 2.0));
        rx->setParameter(0, p);
        addInstruction(rx);
      }
    }
  }

  return true;
} // namespace instructions

} // namespace generators
} // namespace xacc