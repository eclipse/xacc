#pragma once

#include "CompositeInstruction.hpp"
#include "Identifiable.hpp"

namespace xacc {

class IRVerifier : public Identifiable {
public:
  virtual std::pair<bool, std::string>
  verify(std::shared_ptr<CompositeInstruction> original,
         std::shared_ptr<CompositeInstruction> transformed,
         const HeterogeneousMap &options = {}) = 0;

  virtual ~IRVerifier() = default;
};

} // namespace xacc
