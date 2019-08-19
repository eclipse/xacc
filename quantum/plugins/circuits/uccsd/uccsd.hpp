
#ifndef XACC_GENERATORS_UCCSD_HPP_
#define XACC_GENERATORS_UCCSD_HPP_

#include "Circuit.hpp"

namespace xacc {

namespace circuits {

class UCCSD : public xacc::quantum::Circuit {

public:
  UCCSD() : Circuit("uccsd") {}
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  DEFINE_CLONE(UCCSD);
};

} // namespace circuits

} // namespace xacc

#endif
