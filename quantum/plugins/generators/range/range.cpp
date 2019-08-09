#include "range.hpp"
#include "XACC.hpp"
#include "xacc_service.hpp"

using namespace xacc;
namespace xacc {
namespace generators {
bool Range::validateOptions() {
  if (!options.count("gate")) {
    return false;
  }

  if (!options.count("nqubits")) {
    if (!options.count("start") && !options.count("end")) {
      return false;
    }
  }

  return true;
}

std::shared_ptr<xacc::Function>
Range::generate(std::map<std::string, xacc::InstructionParameter> &parameters) {

  if (!parameters.empty()) {
    options = parameters;
  }

  auto gate = options["gate"].as<std::string>();
  int start = 0, end, nqubits;
  if (options.count("nqubits")) {
    try {
      nqubits = options["nqubits"].as_no_error<int>();
    } catch (std::exception &e) {
      nqubits = (int)options["nqubits"].as<double>();
    }
    end = nqubits;
  } else if (options.count("start")) {
    try {
      start = options["start"].as_no_error<int>();
    } catch (std::exception &e) {
      start = (int)options["start"].as<double>();
    }

    try {
      end = options["end"].as_no_error<int>();
    } catch (std::exception &e) {
      end = (int)options["end"].as<double>();
    }

  }

  auto provider = xacc::getIRProvider("quantum");
  auto f = provider->createFunction("range" + gate, {});
  for (int i = start; i < end; i++) {
    auto g = provider->createInstruction(gate, {i});
    f->addInstruction(g);
  }

  return f;
} // namespace instructions

} // namespace generators
} // namespace xacc