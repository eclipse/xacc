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
#ifndef XACC_COMPILER_IR_HPP_
#define XACC_COMPILER_IR_HPP_

#include "CompositeInstruction.hpp"

namespace xacc {

class IR : public Persistable {
protected:
  HeterogeneousMap runtimeVariables;
public:
  void setRuntimeVariables(HeterogeneousMap m) {runtimeVariables = m;}
  HeterogeneousMap getRuntimeVariables() {return runtimeVariables;}

  virtual void addComposite(std::shared_ptr<CompositeInstruction> kernel) = 0;
  virtual bool compositeExists(const std::string &name) = 0;
  virtual std::shared_ptr<CompositeInstruction> getComposite(const std::string &name) = 0;
  virtual std::vector<std::shared_ptr<CompositeInstruction>> getComposites() = 0;

  virtual void mapBits(std::vector<std::size_t> bitMap) = 0;
  virtual const std::size_t maxBit() = 0;
  virtual ~IR() {}
};

} // namespace xacc
#endif
