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
#ifndef QUANTUM_GATE_IR_MEASURE_HPP_
#define QUANTUM_GATE_IR_MEASURE_HPP_

#include "GateInstruction.hpp"

namespace xacc {
namespace quantum {

/**
 *
 */
class Measure : public GateInstruction {
public:
	Measure() :GateInstruction("Measure", std::vector<InstructionParameter> {
		InstructionParameter(0) }) {}


	Measure(std::vector<int> qbit) :
			GateInstruction("Measure", qbit, std::vector<InstructionParameter> {
					InstructionParameter(0) }) {
	}

	Measure(int qbit, int classicalIdx) :
			GateInstruction("Measure", std::vector<int> { qbit },
					std::vector<InstructionParameter> { InstructionParameter(
							classicalIdx) }) {
	}

	const std::string toString(const std::string& bufferVarName) {
		return gateName + " " + bufferVarName + std::to_string(bits()[0]);
	}


	int getClassicalBitIndex() {
		return boost::get<int>(parameters[0]);
	}
	virtual std::shared_ptr<GateInstruction> clone() {
		return std::make_shared<Measure>();
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
