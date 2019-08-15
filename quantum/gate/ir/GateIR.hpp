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
#ifndef QUANTUM_GATE_GATEIR_HPP_
#define QUANTUM_GATE_GATEIR_HPP_

#include "CompositeInstruction.hpp"
#include "InstructionIterator.hpp"
#include "IR.hpp"
#include "Utils.hpp"

namespace xacc {
namespace quantum {

class GateIR : public xacc::IR {

public:
  GateIR() {}

  const std::size_t maxBit() override {
    int maxBit = 0;
    for (auto k : kernels) {
      for (auto inst : k->getInstructions()) {
        for (auto b : inst->bits()) {
          if (b > maxBit) {
            maxBit = b;
          }
        }
      }
    }
    return maxBit;
  }

  void addComposite(std::shared_ptr<CompositeInstruction> kernel) override {
    kernels.push_back(kernel);
  }

  std::shared_ptr<CompositeInstruction> getComposite(const std::string &name) override {
    std::shared_ptr<CompositeInstruction> ret;
    for (auto f : kernels) {
      if (f->name() == name) {
        ret = f;
      }
    }
    if (!ret) {
      xacc::XACCLogger::instance()->error("GateIR: Invalid kernel name " + name + ".");
      print_backtrace();
      exit(0);
    }
    return ret;
  }

  bool compositeExists(const std::string &name) override {
    return std::any_of(
        kernels.cbegin(), kernels.cend(),
        [=](std::shared_ptr<CompositeInstruction> i) { return i->name() == name; });
  }

  void mapBits(std::vector<std::size_t> bitMap) override {
    for (auto k : kernels) {
      k->mapBits(bitMap);
    }
  }

  void persist(std::ostream &outStream) override;

  void load(std::istream &inStream) override;

  std::vector<std::shared_ptr<CompositeInstruction>> getComposites() override {
    return kernels;
  }

  virtual ~GateIR() {}

protected:
  std::vector<std::shared_ptr<CompositeInstruction>> kernels;
};
} // namespace quantum
} // namespace xacc
#endif
