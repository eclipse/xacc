
#ifndef XACC_GENERATORS_UCCSD_HPP_
#define XACC_GENERATORS_UCCSD_HPP_

#include "IRGenerator.hpp"

namespace xacc {

namespace generators {

class UCCSD : public xacc::IRGenerator {

public:
  std::shared_ptr<Function>
  generate(std::map<std::string, InstructionParameter> &parameters) override;

  const std::string name() const override { return "uccsd"; }

  const std::string description() const override { return ""; }
};

} // namespace generators

} // namespace xacc

#endif
