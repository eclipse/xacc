/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#ifndef XACC_ACCELERATORDECORATOR_HPP_
#define XACC_ACCELERATORDECORATOR_HPP_

#include "Accelerator.hpp"

namespace xacc {

class AcceleratorDecorator : public Accelerator {
protected:

  std::shared_ptr<Accelerator> decoratedAccelerator;
  virtual bool isValidBufferSize(const int NBits) {return true;} 
  
public:

  AcceleratorDecorator() {}
  AcceleratorDecorator(std::shared_ptr<Accelerator> a) :decoratedAccelerator(a) {}
  void setDecorated(std::shared_ptr<Accelerator> a) {
      decoratedAccelerator = a;
  }
  
  virtual void initialize(){ decoratedAccelerator->initialize(); }

  /**
   * Return the type of this Accelerator.
   *
   * @return type The Accelerator type - Gate or AQC QPU, or NPU
   */
  virtual AcceleratorType getType() {decoratedAccelerator->getType();}

  /**
   * Return any IR Transformations that must be applied to ensure
   * the compiled IR is amenable to execution on this Accelerator.
   *
   * @return transformations The IR transformations this Accelerator exposes
   */
  virtual std::vector<std::shared_ptr<IRTransformation>>
  getIRTransformations() {return decoratedAccelerator->getIRTransformations();}

  /**
   * Execute the provided XACC IR Function on the provided AcceleratorBuffer.
   *
   * @param buffer The buffer of bits this Accelerator should operate on.
   * @param function The kernel to execute.
   */
  virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::shared_ptr<Function> function) = 0;

  /**
   * Execute a set of kernels with one remote call. Return
   * a list of AcceleratorBuffers that provide a new view
   * of the given one AcceleratorBuffer. The ith AcceleratorBuffer
   * contains the results of the ith kernel execution.
   *
   * @param buffer The AcceleratorBuffer to execute on
   * @param functions The list of IR Functions to execute
   * @return tempBuffers The list of new AcceleratorBuffers
   */
  virtual std::vector<std::shared_ptr<AcceleratorBuffer>>
  execute(std::shared_ptr<AcceleratorBuffer> buffer,
          const std::vector<std::shared_ptr<Function>> functions) = 0;

  /**
   * Create, store, and return an AcceleratorBuffer with the given
   * variable id string. This method returns all available
   * qubits for this Accelerator. The string id serves as a unique identifier
   * for future lookups and reuse of the AcceleratorBuffer.
   *
   * @param varId The variable name of the created buffer
   * @return buffer The buffer instance created.
   */
  virtual std::shared_ptr<AcceleratorBuffer>
  createBuffer(const std::string &varId) {return decoratedAccelerator->createBuffer(varId);}

  /**
   * Create, store, and return an AcceleratorBuffer with the given
   * variable id string and of the given number of bits.
   * The string id serves as a unique identifier
   * for future lookups and reuse of the AcceleratorBuffer.
   *
   * @param varId The variable name of the created buffer
   * @param size The number of bits in the created buffer
   * @return buffer The buffer instance created.
   */
  virtual std::shared_ptr<AcceleratorBuffer>
  createBuffer(const std::string &varId, const int size) {return decoratedAccelerator->createBuffer(varId,size);}

  
  /**
   * Return true if this Accelerator is a remotely hosted resource.
   *
   * @return remote True if this is a remote Accelerator
   */
  virtual bool isRemote() { return decoratedAccelerator->isRemote(); }

  /**
   * Destructor
   */
  virtual ~AcceleratorDecorator() {}

};

} // namespace xacc
#endif
