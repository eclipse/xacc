#include "xacc_quantum_gate_api.hpp"
#include "PauliOperator.hpp"
#include "FermionOperator.hpp"
#include "xacc.hpp"

namespace xacc {
namespace quantum {

std::shared_ptr<Observable> getObservable(const std::string type,
                                          const std::string representation) {
  if (type == "pauli") {
    return representation.empty()
               ? std::make_shared<PauliOperator>()
               : std::make_shared<PauliOperator>(representation);
  } else if (type == "fermion") {
    return representation.empty()
               ? std::make_shared<FermionOperator>()
               : std::make_shared<FermionOperator>(representation);
  } else {
    xacc::error("[xacc::getObservable()] Invalid observable type: " + type);
    return std::make_shared<PauliOperator>();
  }
}

std::shared_ptr<Observable> getObservable() {
  return getObservable("pauli", std::string(""));
}
std::shared_ptr<Observable> getObservable(const std::string type) {
  return getObservable(type, std::string(""));
}

std::shared_ptr<Observable> getObservable(const std::string type,
                                          const HeterogeneousMap &options) {
  auto observable = getObservable(type);
  observable->fromOptions(options);
  return observable;
}
std::shared_ptr<Observable> getObservable(const std::string type,
                                          const HeterogeneousMap &&options) {
  return getObservable(type, options);
}

} // namespace quantum
} // namespace xacc