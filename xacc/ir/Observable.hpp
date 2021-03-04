/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_IR_OBSERVABLE_HPP_
#define XACC_IR_OBSERVABLE_HPP_
#include "CompositeInstruction.hpp"
#include "Utils.hpp"

namespace xacc {
class AcceleratorBuffer;
class SparseTriplet
    : std::tuple<std::uint64_t, std::uint64_t, std::complex<double>> {
public:
  SparseTriplet(std::uint64_t r, std::uint64_t c, std::complex<double> coeff) {
    std::get<0>(*this) = r;
    std::get<1>(*this) = c;
    std::get<2>(*this) = coeff;
  }
  const std::uint64_t row() { return std::get<0>(*this); }
  const std::uint64_t col() { return std::get<1>(*this); }
  const std::complex<double> coeff() { return std::get<2>(*this); }
};

class Observable : public Identifiable {
public:
  virtual std::vector<std::shared_ptr<CompositeInstruction>>
  observe(std::shared_ptr<CompositeInstruction> CompositeInstruction) = 0;

  virtual const std::string toString() = 0;
  virtual void fromString(const std::string str) = 0;
  virtual const int nBits() = 0;
  virtual void fromOptions(const HeterogeneousMap &&options) {
    fromOptions(options);
    return;
  }
  virtual void fromOptions(const HeterogeneousMap &options) = 0;

  virtual std::vector<std::shared_ptr<Observable>> getSubTerms() { return {}; }

  virtual std::vector<std::shared_ptr<Observable>> getNonIdentitySubTerms() {
    return {};
  }

  virtual std::shared_ptr<Observable> getIdentitySubTerm() { return nullptr; }

  virtual std::complex<double> coefficient() {
    return std::complex<double>(1.0, 0.0);
  }

  virtual std::vector<SparseTriplet>
  to_sparse_matrix() {
    return {};
  }

   virtual std::shared_ptr<Observable> commutator(std::shared_ptr<Observable>){
    return nullptr;
  }

  virtual void normalize() { return; }
  // Some pre-defined tasks (Observable sub-classes can have custom tasks)
  struct PostProcessingTask {
    static inline const std::string EXP_VAL_CALC = "exp-val";
    static inline const std::string VARIANCE_CALC = "variance";
  };
  // Post process the execution data (stored on the AcceleratorBuffer)
  virtual double
  postProcess(
      std::shared_ptr<AcceleratorBuffer> buffer,
      const std::string &postProcessTask = PostProcessingTask::EXP_VAL_CALC,
      const HeterogeneousMap &extra_data = {}) = 0;
};

template Observable *
HeterogeneousMap::getPointerLike<Observable>(const std::string key) const;
template bool
HeterogeneousMap::pointerLikeExists<Observable>(const std::string key) const;

} // namespace xacc
#endif