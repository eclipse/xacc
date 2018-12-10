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
#ifndef QUANTUM_GATE_GATEIR_INSTRUCTIONS_CZ_HPP_
#define QUANTUM_GATE_GATEIR_INSTRUCTIONS_CZ_HPP_

#include "GateInstruction.hpp"

namespace xacc {
namespace quantum {

class CZ : public virtual GateInstruction {
public:
  CZ() : GateInstruction("CZ") {}
  CZ(std::vector<int> qbits) : GateInstruction("CZ", qbits) {}
  CZ(int srcqbit, int tgtqbit) : CZ(std::vector<int>{srcqbit, tgtqbit}) {}

  DEFINE_CLONE(CZ)
  DEFINE_VISITABLE()
};

} // namespace quantum
} // namespace xacc
#endif
