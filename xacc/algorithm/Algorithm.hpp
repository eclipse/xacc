#ifndef XACC_ALGORITHM_HPP_
#define XACC_ALGORITHM_HPP_

#include "Identifiable.hpp"
#include "heterogeneous.hpp"
#include "Observable.hpp"
#include "Accelerator.hpp"
#include "Optimizer.hpp"

#include <memory>

namespace xacc {

class Algorithm : public xacc::Identifiable {

public:
  bool initialize(const HeterogeneousMap &&parameters) { return initialize(parameters); }
  virtual bool initialize(const HeterogeneousMap &parameters) = 0;
  virtual const std::vector<std::string> requiredParameters() const = 0;

  virtual void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const = 0;
};

} // namespace xacc

#endif