#include "hwe.hpp"
#include "XACC.hpp"
#include "IRProvider.hpp"
#include "xacc_service.hpp"
#include <regex>

using namespace xacc;

namespace xacc {
namespace generators {
bool HWE::validateOptions() {
  if (options.count("n-qubits")) {
    if (!options["n-qubits"].isNumeric()) {
      return false;
    }
  }
  return true;
}

std::shared_ptr<xacc::Function>
HWE::generate(std::map<std::string, xacc::InstructionParameter> &parameters) {

  if (!parameters.empty()) {
    options = parameters;
  }

  int nQubits = 0, layers = 1;
  std::vector<std::pair<int, int>> connectivity;
  try {
    nQubits = options["n-qubits"].as<int>();
    layers = options.count("layers") ? options["layers"].as<int>() : layers;
    if (options.count("coupling")) {
      connectivity = options["coupling"].as<std::vector<std::pair<int, int>>>();
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
  if (options.count("param_id")) {
    paramLetter = options["param_id"].toString();
  }

  std::vector<InstructionParameter> fParams;
  for (int nP = 0; nP < (2 * nQubits + 3 * nQubits * layers); nP++)
    fParams.push_back(InstructionParameter(paramLetter + std::to_string(nP)));

  auto provider = xacc::getService<IRProvider>("gate");
  auto f = provider->createFunction("hwe", {}, fParams);
  int angleCounter = 0;

  // Zeroth layer, start with X and Z rotations
  for (int q = 0; q < nQubits; q++) {
    auto rx = provider->createInstruction(
        "Rx", {q},
        {InstructionParameter(paramLetter + std::to_string(angleCounter))});
    auto rz = provider->createInstruction(
        "Rz", {q},
        {InstructionParameter(paramLetter + std::to_string(angleCounter + 1))});
    f->addInstruction(rx);
    f->addInstruction(rz);
    angleCounter += 2;
  }

  for (int d = 0; d < layers; d++) {
    for (auto &p : connectivity) {
      auto cnot = provider->createInstruction("CNOT", {p.first, p.second});
      f->addInstruction(cnot);
    }
    for (int q = 0; q < nQubits; q++) {
      auto rz1 = provider->createInstruction(
          "Rz", {q},
          {InstructionParameter(paramLetter + std::to_string(angleCounter))});
      f->addInstruction(rz1);

      auto rx = provider->createInstruction(
          "Rx", {q},
          {InstructionParameter(paramLetter +
                                std::to_string(angleCounter + 1))});
      f->addInstruction(rx);

      auto rz2 = provider->createInstruction(
          "Rz", {q},
          {InstructionParameter(paramLetter +
                                std::to_string(angleCounter + 2))});
      f->addInstruction(rz2);

      angleCounter += 3;
    }
  }

  return f;
}

} // namespace instructions
} // namespace qcor