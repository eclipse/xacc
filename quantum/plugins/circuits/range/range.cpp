#include "range.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

using namespace xacc;
namespace xacc {
namespace circuits {

const std::vector<std::string> Range::requiredKeys() {
  return {"nq", "gate", "start", "end"};
}

bool Range::expand(const xacc::HeterogeneousMap &runtimeOptions) {
  if (!runtimeOptions.keyExists<std::string>("gate")) {
    return false;
  }

  std::stringstream sss;
  runtimeOptions.print<int, std::string>(sss);
  int start = 0, end = -1, nqubits;
  if (!runtimeOptions.keyExists<int>("nq")) {
    if (!runtimeOptions.keyExists<int>("start") &&
        !runtimeOptions.keyExists<int>("end")) {
      return false;
    } else {
      if (runtimeOptions.keyExists<int>("start")) {
        start = runtimeOptions.get<int>("start");
      }

      if (runtimeOptions.keyExists<int>("end")) {
        end = runtimeOptions.get<int>("end");
      }
    }
  } else {
    end = runtimeOptions.get<int>("nq");
  }

  if (end == -1) {
    std::stringstream ss;
    runtimeOptions.print<int, std::string>(ss);
    xacc::error("Invalid Range runtimeOptions:\n"+ss.str());
  }

  auto gate = runtimeOptions.get<std::string>("gate");
  auto provider = xacc::getIRProvider("quantum");
  for (std::size_t i = start; i < end; i++) {
    auto g = provider->createInstruction(gate, std::vector<std::size_t>{i});
    addInstruction(g);
  }

  return true;
} // namespace instructions

} // namespace circuits
} // namespace xacc