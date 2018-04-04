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
#ifndef QUANTUM_GATE_GATEQIR_FUNCTIONS_CONDITIONALFUNCTION_HPP_
#define QUANTUM_GATE_GATEQIR_FUNCTIONS_CONDITIONALFUNCTION_HPP_

#include "GateFunction.hpp"

namespace xacc {
namespace quantum {

class ConditionalFunction: public virtual GateFunction {

protected:

	int qbitIdx;

public:
	ConditionalFunction(int qbit) :
			GateFunction("conditional_" + std::to_string(qbit)), qbitIdx(qbit) {
	}

	void addInstruction(InstPtr instruction) {
		instruction->disable();
		instructions.push_back(instruction);
	}

	const int getConditionalQubit() {
		return qbitIdx;
	}

	void evaluate(const int accBitState) {
		if (accBitState == 1) {
			for (auto i : instructions) {
				i->enable();
			}
		}
	}

	const std::string toString(
			const std::string& bufferVarName) {
		return "";
	}

	DEFINE_VISITABLE()

};
}
}

#endif
