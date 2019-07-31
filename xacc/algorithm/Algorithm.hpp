#ifndef XACC_ALGORITHM_HPP_
#define XACC_ALGORITHM_HPP_

#include "Identifiable.hpp"
#include "InstructionParameter.hpp"
#include "Observable.hpp"
#include "Accelerator.hpp"
#include "Optimizer.hpp"

#include <memory>

namespace xacc {

using AlgorithmParameter =
    Variant<int, double, std::string, std::vector<int>, std::vector<double>,
            std::vector<std::string>, std::shared_ptr<Observable>, Observable*, 
            std::shared_ptr<Accelerator>, std::shared_ptr<Function>,
            std::shared_ptr<Optimizer>>;
using AlgorithmParameters = std::map<std::string, AlgorithmParameter>;

class Algorithm : public xacc::Identifiable {

public:
  bool initialize(const AlgorithmParameters &&parameters) { return initialize(parameters); }
  virtual bool initialize(const AlgorithmParameters &parameters) = 0;
  virtual const std::vector<std::string> requiredParameters() const = 0;

  virtual void execute(const std::shared_ptr<AcceleratorBuffer> buffer) const = 0;
};

} // namespace xacc

#endif