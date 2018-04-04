/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef QUANTUM_GATE_GATEQIR_INSTRUCTIONS_CNOT_HPP_
#define QUANTUM_GATE_GATEQIR_INSTRUCTIONS_CNOT_HPP_

#include "GateInstruction.hpp"

namespace xacc {
namespace quantum {

/**
 *
 */
class CNOT: public virtual GateInstruction {
public:
	CNOT() :GateInstruction("CNOT") {}
	CNOT(std::vector<int> qbits) : GateInstruction("CNOT", qbits) {}
	CNOT(int srcqbit, int tgtqbit) : CNOT(std::vector<int> { srcqbit, tgtqbit }) {}

	virtual std::shared_ptr<GateInstruction> clone() {
		return std::make_shared<CNOT>();
	}

	/**
	 * Return the description of this instance
	 * @return description The description of this object.
	 */
	virtual const std::string description() const {
		return "";
	}


	DEFINE_VISITABLE()
};

}
}
#endif
