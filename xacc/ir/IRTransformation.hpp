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
#ifndef XACC_IR_IRTRANSFORMATION_HPP_
#define XACC_IR_IRTRANSFORMATION_HPP_

#include "IR.hpp"
#include "Identifiable.hpp"
#include "Accelerator.hpp"

namespace xacc {

enum IRTransformationType { Optimization, Placement, ErrorCorrection };

class IRTransformation : public Identifiable {
public:
  virtual void apply(std::shared_ptr<CompositeInstruction> program,
                     const std::shared_ptr<Accelerator> accelerator,
                     const HeterogeneousMap &options = {}) = 0;
  virtual const IRTransformationType type() const = 0;

  virtual ~IRTransformation() {}
};

} // namespace xacc
#endif
