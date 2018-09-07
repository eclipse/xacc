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

#include "GateInstruction.hpp"
#include "GateFunction.hpp"
#include "InstructionIterator.hpp"
#include "IR.hpp"

namespace xacc {
namespace quantum {

class GateIR : public xacc::IR {

public:
  /**
   * The nullary Constructor
   */
  GateIR() {}

  virtual const int maxBit() {
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

  /**
   * Add a quantum function to this intermediate representation.
   * @param kernel
   */
  virtual void addKernel(std::shared_ptr<Function> kernel) {
    kernels.push_back(kernel);
  }

  virtual const int numberOfKernels() { return kernels.size(); }

  virtual std::shared_ptr<Function> getKernel(const std::string &name) {
    std::shared_ptr<Function> ret;
    for (auto f : kernels) {
      if (f->name() == name) {
        ret = f;
      }
    }
    if (!ret) {
      xacc::error("Invalid kernel name.");
    }
    return ret;
  }

  virtual bool kernelExists(const std::string &name) {
    return std::any_of(
        kernels.cbegin(), kernels.cend(),
        [=](std::shared_ptr<Function> i) { return i->name() == name; });
  }

  virtual void mapBits(std::vector<int> bitMap) {
    for (auto k : kernels) {
      k->mapBits(bitMap);
    }
  }

  /**
   * Return a string representation of this
   * intermediate representation
   * @return
   */
  virtual std::string toAssemblyString(const std::string &kernelName,
                                       const std::string &accBufferVarName);

  /**
   * Persist this IR instance to the given
   * output stream.
   *
   * @param outStream
   */
  virtual void persist(std::ostream &outStream);

  /**
   * Create this IR instance from the given input
   * stream.
   *
   * @param inStream
   */
  virtual void load(std::istream &inStream);

  virtual std::vector<std::shared_ptr<Function>> getKernels() {
    return kernels;
  }

  /**
   * The destructor
   */
  virtual ~GateIR() {}

protected:
  /**
   * Reference to this QIR's list of quantum functions
   */
  std::vector<std::shared_ptr<Function>> kernels;
};
} // namespace quantum
} // namespace xacc
#endif
