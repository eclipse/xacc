#include "xacc.hpp"
#include "qcec.hpp"
#include "CompilationFlowEquivalenceChecker.hpp"
#include "EquivalenceChecker.hpp"
#include "IRVerifier.hpp"
#include "ImprovedDDEquivalenceChecker.hpp"
#include "PowerOfSimulationEquivalenceChecker.hpp"
#include "xacc_plugin.hpp"
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <fstream>

namespace xacc {
namespace quantum {

const std::unordered_map<std::string, ec::Method> strToMethod = {
    {"reference", ec::Reference},
    {"naive", ec::Naive},
    {"proportional", ec::Proportional},
    {"lookahead", ec::Lookahead},
    {"simulation", ec::PowerOfSimulation},
    {"compilationflow", ec::CompilationFlow}};

const std::unordered_map<std::string, ec::StimuliType> strToStimuli = {
    {"classical", ec::Classical},
    {"localquantum", ec::LocalQuantum},
    {"globalquantum", ec::GlobalQuantum}};

std::pair<bool, std::string>
getResults(ec::EquivalenceCheckingResults &result) {
  auto &equivalence = result.equivalence;
  if (equivalence == ec::NoInformation || result.error())
    return {false, "{\"error\":\"No equivalence information avalible\""};

  result.name1 = "original";
  result.name2 = "transformed";

  if (equivalence == ec::Equivalent || equivalence == ec::ProbablyEquivalent ||
      equivalence == ec::EquivalentUpToGlobalPhase)
    return {true, result.produceJSON(true).dump()};

  return {false, result.produceJSON(true).dump()};
}

static std::string writeTempFile(const std::string &str) {
  auto &&hash = std::hash<std::string>{};
  auto filename = "/tmp/xacc-verifier-" + std::to_string(hash(str)) + ".qasm";
  std::ofstream(filename) << str;
  return filename;
}

std::pair<bool, std::string>
qcec::verify(std::shared_ptr<CompositeInstruction> original,
             std::shared_ptr<CompositeInstruction> transformed,
             const HeterogeneousMap &options) {
  auto staq = xacc::getCompiler("staq");

  auto originalFile = writeTempFile(staq->translate(original));
  auto transformedFile = writeTempFile(staq->translate(transformed));

  qc::QuantumComputation originalQC(originalFile);
  qc::QuantumComputation transformedQC(transformedFile);

  std::remove(originalFile.c_str());
  std::remove(transformedFile.c_str());

  ec::Method method = ec::Proportional;
  ec::Configuration config{};

  if (options.stringExists("method"))
    method = strToMethod.at(options.getString("method"));
  if (options.stringExists("stimuli"))
    config.stimuliType = strToStimuli.at(options.getString("stimuli"));
  if (options.stringExists("numsims"))
    config.max_sims = options.get<unsigned long>("numsims");
  if (options.stringExists("tolerance"))
    config.tolerance = options.get<double>("tolerance");
  if (options.stringExists("fidelity"))
    config.fidelity_limit =
        std::clamp(options.get<double>("fidelity"), 0.0, 1.0);

  if (method == ec::CompilationFlow) {
    ec::CompilationFlowEquivalenceChecker ec(originalQC, transformedQC);
    ec.expectNothing();
    ec.check(config);
    return getResults(ec.results);
  } else if (method == ec::PowerOfSimulation) {
    ec::PowerOfSimulationEquivalenceChecker ec(originalQC, transformedQC);
    ec.expectNothing();
    ec.check(config);
    return getResults(ec.results);
  } else {
    ec::ImprovedDDEquivalenceChecker ec(originalQC, transformedQC, method);
    ec.expectNothing();
    ec.check(config);
    return getResults(ec.results);
  }

  return {false, "{\"error\":\"No verification method used\""};
}

} // namespace quantum
} // namespace xacc

REGISTER_PLUGIN(xacc::quantum::qcec, xacc::IRVerifier)
