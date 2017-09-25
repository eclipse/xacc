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
#include "ConditionalFunction.hpp"
namespace xacc {
namespace quantum {

ConditionalFunction::ConditionalFunction(int qbit) :
		GateFunction("conditional_" + std::to_string(qbit)), qbitIdx(qbit) {
}

void ConditionalFunction::addInstruction(InstPtr instruction) {
	instruction->disable();
	instructions.push_back(instruction);
}

const int ConditionalFunction::getConditionalQubit() {
	return qbitIdx;
}

void ConditionalFunction::evaluate(const int accBitState) {
	if (accBitState == 1) {
		for (auto i : instructions) {
			i->enable();
		}
	}
}

const std::string ConditionalFunction::toString(
		const std::string& bufferVarName) {
	return "";
}

//RegisterGateFunction<ConditionalFunction, int> CONDTEMP("Conditional");

}
}
