/***********************************************************************************
 * Copyright (c) 2018, UT-Battelle
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
#ifndef QUANTUM_GATE_ACCELERATORS_QCSACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_QCSACCELERATOR_HPP_

#include "InstructionIterator.hpp"
#include "QuilVisitor.hpp"
#include "CLIParser.hpp"

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
using namespace rapidjson;

using namespace xacc;

namespace xacc {
namespace quantum {

/**
 *
 */
class QCSAccelerator : virtual public Accelerator {
public:
  QCSAccelerator() : Accelerator() {}

  /**
   * Create, store, and return an AcceleratorBuffer with the given
   * variable id string and of the given number of bits.
   * The string id serves as a unique identifier
   * for future lookups and reuse of the AcceleratorBuffer.
   *
   * @param varId
   * @param size
   * @return
   */
  std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string &varId,
                                                  const int size);

  virtual std::shared_ptr<AcceleratorBuffer>
  createBuffer(const std::string &varId);

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::shared_ptr<Function> function) override;
  std::vector<std::shared_ptr<AcceleratorBuffer>>
  execute(std::shared_ptr<AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<Function>> functions) override;

  virtual void initialize() {}

  /**
   * Return true if this Accelerator can allocated
   * NBits number of bits.
   * @param NBits
   * @return
   */
  virtual bool isValidBufferSize(const int NBits);


  /**
   * This Accelerator models QPU Gate accelerators.
   * @return
   */
  virtual AcceleratorType getType() { return AcceleratorType::qpu_gate; }

  /**
   * We have no need to transform the IR for this Accelerator,
   * so return an empty list, for now.
   * @return
   */
  virtual std::vector<std::shared_ptr<IRTransformation>>
  getIRTransformations() {
    std::vector<std::shared_ptr<IRTransformation>> v;
    return v;
  }

  /**
   * Return all relevant RigettiAccelerator runtime options.
   * Users can set the api-key, execution type, and number of triels
   * from the command line with these options.
   */
  virtual OptionPairs getOptions() {
    OptionPairs desc {{"qcs-shots",
        "Provide the number of trials to execute."},{
        "qcs-backend", ""}};
    return desc;
  }

  virtual const std::string name() const { return "qcs"; }

  virtual const std::string description() const {
    return "";
  }

  /**
   * The destructor
   */
  virtual ~QCSAccelerator() {}

};

} // namespace quantum
} // namespace xacc

#endif
