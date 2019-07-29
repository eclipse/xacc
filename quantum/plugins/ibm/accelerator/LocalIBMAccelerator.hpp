/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef QUANTUM_GATE_ACCELERATORS_LOCALIBMACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_LOCALIBMACCELERATOR_HPP_

#include "Accelerator.hpp"
#include "InstructionIterator.hpp"
#include "OpenQasmVisitor.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {

/**
 */
class LocalIBMAccelerator : public Accelerator {
public:
  std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string &varId,
                                                  const int size) override;

  std::shared_ptr<AcceleratorBuffer>
  createBuffer(const std::string &varId) override;

  bool isValidBufferSize(const int NBits) override;

  AcceleratorType getType() override { return AcceleratorType::qpu_gate; }

  void initialize() override { return; }

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::shared_ptr<xacc::Function> kernel) override;

  std::vector<std::shared_ptr<AcceleratorBuffer>>
  execute(std::shared_ptr<AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<Function>> functions) override;

  std::vector<std::shared_ptr<IRTransformation>> getIRTransformations() override;

  OptionPairs getOptions() override {
    OptionPairs desc {{"local-ibm-ro-error",
                        "Provide the p(1|0), p(0|1)."},{
                        "u-p-depol", ""},{
                        "cx-p-depol", ""}};
    return desc;
  }

  /**
   * Return the name of this instance.
   *
   * @return name The string name
   */
  const std::string name() const override { return "local-ibm"; }

  /**
   * Return the description of this instance
   * @return description The description of this object.
   */
  const std::string description() const override { return ""; }

  LocalIBMAccelerator() : Accelerator() {}

  /**
   * The destructor
   */
  virtual ~LocalIBMAccelerator() {}

  std::map<int, std::vector<int>> measurementSupports;
};

} // namespace quantum
} // namespace xacc

#endif
