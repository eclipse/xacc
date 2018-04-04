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
#ifndef QUANTUM_GATE_GATEQIR_INSTRUCTIONS_CPHASE_HPP_
#define QUANTUM_GATE_GATEQIR_INSTRUCTIONS_CPHASE_HPP_

#include "GateInstruction.hpp"

namespace xacc {
namespace quantum {
class CPhase: public virtual GateInstruction {
public:
	CPhase() :
			GateInstruction("CPhase", std::vector<InstructionParameter> {
					InstructionParameter(0.0) }) {
	}

	CPhase(int controlQubit, int targetQubit, double theta) :
			GateInstruction("CPhase", std::vector<int> { controlQubit,
					targetQubit }, std::vector<InstructionParameter> {
					InstructionParameter(theta) }) {
	}

	CPhase(std::vector<int> qbits) :
			GateInstruction("CPhase", qbits, std::vector<InstructionParameter> {
					InstructionParameter(0.0) }) {

	}

	virtual std::shared_ptr<GateInstruction> clone() {
		return std::make_shared<CPhase>();
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
