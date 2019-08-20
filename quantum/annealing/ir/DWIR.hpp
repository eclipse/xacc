/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#ifndef QUANTUM_AQC_IR_DWIR_HPP_
#define QUANTUM_AQC_IR_DWIR_HPP_

#include "IR.hpp"

namespace xacc {
namespace quantum {

class DWIR : public IR {

public:

  void persist(std::ostream &outStream) override{ return; }

  const std::size_t maxBit() override { return 0; }

  void load(std::istream &inStream) override {}

  void addComposite(std::shared_ptr<CompositeInstruction> kernel) override{
    kernels.push_back(kernel);
  }

  bool compositeExists(const std::string &name) override {
    return std::any_of(
        kernels.cbegin(), kernels.cend(),
        [=](std::shared_ptr<CompositeInstruction> i) { return i->name() == name; });
  }

  std::shared_ptr<CompositeInstruction> getComposite(const std::string &name) override {
    for (auto f : kernels) {
      if (f->name() == name) {
        return f;
      }
    }
    XACCLogger::instance()->error("Invalid kernel name - " + name);
    return nullptr;
  }

  void mapBits(std::vector<std::size_t> bitMap) override {}

  std::vector<std::shared_ptr<CompositeInstruction>> getComposites() override {
    return kernels;
  }

protected:
  /**
   * Reference to this QIR's list of quantum functions
   */
  std::vector<std::shared_ptr<CompositeInstruction>> kernels;
};

} // namespace quantum
} // namespace xacc

#endif /* QUANTUM_AQC_IR_DWIR_HPP_ */
