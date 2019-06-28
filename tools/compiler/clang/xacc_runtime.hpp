#include "XACC.hpp"
#include "xacc_service.hpp"
#include "IRProvider.hpp"

#define __qpu__ __attribute__((annotate("__qpu__")))
using qbit = std::shared_ptr<xacc::AcceleratorBuffer>;
namespace xacc {
qbit qalloc(const int n) {
  return std::make_shared<xacc::AcceleratorBuffer>(n);
}
std::shared_ptr<Function> loadFromIR(const std::string &ir) {
  auto function =
      xacc::getService<xacc::IRProvider>("gate")->createFunction("f", {}, {});
  std::istringstream iss(ir);
  function->load(iss);
  return function;
}
} // namespace xacc