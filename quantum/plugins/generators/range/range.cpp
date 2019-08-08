#include "range.hpp"
#include "XACC.hpp"
#include "xacc_service.hpp"

using namespace xacc;
namespace xacc {
namespace generators {
bool Range::validateOptions() {
  if (options.count("nqubits") && options.count("gate")) {
    return true;
  }
  return false;
}
std::shared_ptr<xacc::Function>
Range::generate(std::map<std::string, xacc::InstructionParameter> &parameters) {

  if (!parameters.empty()) {
    options = parameters;
  }

  auto gate = options["gate"].as<std::string>();
  int nqubits;
  try {
     nqubits = options["nqubits"].as_no_error<int>();
  } catch (std::exception& e) {
      nqubits = (int) options["nqubits"].as<double>();
  }

  auto provider = xacc::getIRProvider("quantum");
  auto f = provider->createFunction("range" + gate, {});
  for (int i = 0; i < nqubits; i++) {
    auto g = provider->createInstruction(gate, {i});
    f->addInstruction(g);
  }

  return f;
} // namespace instructions

} // namespace generators
} // namespace xacc