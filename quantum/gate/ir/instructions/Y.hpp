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
#ifndef QUANTUM_GATE_IR_Y_HPP_
#define QUANTUM_GATE_IR_Y_HPP_

#include "GateInstruction.hpp"

namespace xacc {
namespace quantum {

class Y : public virtual GateInstruction {
public:
  Y() : GateInstruction("Y") {}
  Y(std::vector<int> qbit) : GateInstruction("Y", qbit) {}
  Y(int qbit) : Y(std::vector<int>{qbit}) {}

  DEFINE_CLONE(Y)
  DEFINE_VISITABLE()
};

} // namespace quantum
} // namespace xacc

#endif
