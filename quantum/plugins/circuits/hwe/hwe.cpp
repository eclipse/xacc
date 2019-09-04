#include "hwe.hpp"
#include "xacc.hpp"
#include "IRProvider.hpp"
#include "xacc_service.hpp"
#include <regex>

using namespace xacc;

namespace xacc {
namespace circuits {
const std::vector<std::string> HWE::requiredKeys() {
  return {"nq", "layers", "coupling"};
}
bool HWE::expand(const HeterogeneousMap &parameters) {

  if (!parameters.keyExists<int>("nq")) {
      return false;
  }

  int nQubits = 0, layers = 1;
  std::vector<std::pair<int, int>> connectivity;
  try {
    nQubits = parameters.get_with_throw<int>("nq");
    layers = parameters.keyExists<int>("layers") ? parameters.get<int>("layers") : layers;
    if (parameters.keyExists<std::vector<std::pair<int,int>>>("coupling")) {
      connectivity = parameters.get<std::vector<std::pair<int,int>>>("coupling");
    } else {
      for (int i = 0; i < nQubits - 1; i++) {
        connectivity.push_back({i, i + 1});
      }
    }

  } catch (std::exception &e) {
    xacc::error("Could not cast HWE parameter to correct type: " +
                std::string(e.what()));
  }

  std::string paramLetter = "t";
  if (parameters.stringExists("param_id")) {
    paramLetter = parameters.getString("param_id");
  }

  std::vector<std::string> fParams;
  for (int nP = 0; nP < (2 * nQubits + 3 * nQubits * layers); nP++)
    fParams.push_back(paramLetter + std::to_string(nP));

  addVariables(fParams);
  
  auto provider = xacc::getService<IRProvider>("quantum");
  int angleCounter = 0;

  // Zeroth layer, start with X and Z rotations
  for (std::size_t q = 0; q < nQubits; q++) {
    auto rx = provider->createInstruction(
        "Rx", {q},
        {InstructionParameter(paramLetter + std::to_string(angleCounter))});
    auto rz = provider->createInstruction(
        "Rz", {q},
        {InstructionParameter(paramLetter + std::to_string(angleCounter + 1))});
    addInstruction(rx);
    addInstruction(rz);
    angleCounter += 2;
  }

  for (int d = 0; d < layers; d++) {
    for (auto &p : connectivity) {
      std::size_t tmp1 = p.first;
      std::size_t tmp2 = p.second;
      auto cnot = provider->createInstruction("CNOT", {tmp1,tmp2});
      addInstruction(cnot);
    }
    for (std::size_t q = 0; q < nQubits; q++) {
      auto rz1 = provider->createInstruction(
          "Rz", {q},
          {InstructionParameter(paramLetter + std::to_string(angleCounter))});
      addInstruction(rz1);

      auto rx = provider->createInstruction(
          "Rx", {q},
          {InstructionParameter(paramLetter +
                                std::to_string(angleCounter + 1))});
      addInstruction(rx);

      auto rz2 = provider->createInstruction(
          "Rz", {q},
          {InstructionParameter(paramLetter +
                                std::to_string(angleCounter + 2))});
      addInstruction(rz2);

      angleCounter += 3;
    }
  }

  return true;
}

} // namespace instructions
} // namespace qcor