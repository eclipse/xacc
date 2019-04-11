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
#ifndef QUANTUM_GATE_ACCELERATORS_QVMACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_QVMACCELERATOR_HPP_

#include "RemoteAccelerator.hpp"
#include "InstructionIterator.hpp"
#include "QuilVisitor.hpp"
#include "XACC.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {

/**
 * The QVMAccelerator is a QPUGate Accelerator that
 * provides an execute implementation that maps XACC IR
 * to an equivalent Quil string, and executes it on the
 * Rigetti QVM at 127.0.0.1:5000/qvm
 *
 *
 */
class QVMAccelerator : virtual public RemoteAccelerator {
public:
  QVMAccelerator() : RemoteAccelerator() {}

  QVMAccelerator(std::shared_ptr<Client> client)
      : RemoteAccelerator(client) {}
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

  virtual std::vector<std::shared_ptr<AcceleratorBuffer>>
  execute(std::shared_ptr<AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<Function>> functions) {
    int counter = 0;
    std::vector<std::shared_ptr<AcceleratorBuffer>> tmpBuffers;
    for (auto f : functions) {
      xacc::info("QVM Executing kernel = " + f->name());
      auto tmpBuffer = createBuffer(buffer->name() + std::to_string(counter),
                                    buffer->size());
      RemoteAccelerator::execute(tmpBuffer, f);
      tmpBuffers.push_back(tmpBuffer);
      counter++;
    }

    return tmpBuffers;
  }

  virtual void initialize() {}

  /**
   * Return true if this Accelerator can allocated
   * NBits number of bits.
   * @param NBits
   * @return
   */
  virtual bool isValidBufferSize(const int NBits);

  virtual const std::string
  processInput(std::shared_ptr<AcceleratorBuffer> buffer,
               std::vector<std::shared_ptr<Function>> functions);

  /**
   * take response and create
   */
  virtual std::vector<std::shared_ptr<AcceleratorBuffer>>
  processResponse(std::shared_ptr<AcceleratorBuffer> buffer,
                  const std::string &response);
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
   * Return all relevant QVMAccelerator runtime options.
   * Users can set the api-key, execution type, and number of triels
   * from the command line with these options.
   */
  virtual OptionPairs getOptions() {
    OptionPairs desc {{"rigetti-shots",
        "Provide the number of shots to execute on the QVM."}};
    return desc;
  }

  virtual const std::string name() const { return "rigetti-qvm"; }

  virtual const std::string description() const {
    return "The Rigetti QVM Accelerator interacts with "
           "the Forest QVM to execute XACC quantum IR.";
  }

  /**
   * The destructor
   */
  virtual ~QVMAccelerator() {}

private:
  std::vector<int> currentMeasurementSupports;

};

} // namespace quantum
} // namespace xacc

#endif
