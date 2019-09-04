#include "rbm.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

#include "DWQMI.hpp"

using namespace xacc;
namespace xacc {
namespace dwave {

const std::vector<std::string> RBM::requiredKeys() { return {"nh", "nv"}; }

bool RBM::expand(const xacc::HeterogeneousMap &runtimeOptions) {
  if (!runtimeOptions.keyExists<int>("nh") &&
      !runtimeOptions.keyExists<int>("nv")) {
    return false;
  }

  int n_visible = runtimeOptions.get<int>("nv");
  int n_hidden = runtimeOptions.get<int>("nh");

  for (int i = 0; i < n_visible; i++) {
    std::string paramName = "v" + std::to_string(i);
    addVariable(paramName);
    xacc::InstructionParameter visParam(paramName);
    auto qmi = std::make_shared<xacc::quantum::DWQMI>(i, i, visParam);
    addInstruction(qmi);
  }

  for (int i = n_visible; i < n_visible + n_hidden; i++) {
    std::string paramName = "h" + std::to_string(i);
    addVariable(paramName);
    xacc::InstructionParameter hidParam(paramName);
    auto qmi = std::make_shared<xacc::quantum::DWQMI>(i, i, hidParam);
    addInstruction(qmi);
  }
  for (int i = 0; i < n_visible; i++) {
    for (int j = n_visible; j < n_visible + n_hidden; j++) {
      std::string paramName = "w" + std::to_string(i) + std::to_string(j);
      addVariable(paramName);
      xacc::InstructionParameter wParam(paramName);
      auto qmi = std::make_shared<xacc::quantum::DWQMI>(i, j, wParam);
      addInstruction(qmi);
    }
  }

  return true;
} // namespace instructions

} // namespace dwave
} // namespace xacc