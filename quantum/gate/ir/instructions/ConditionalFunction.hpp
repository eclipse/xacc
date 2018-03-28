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

	ConditionalFunction(int qbit);

	virtual void addInstruction(InstPtr instruction);

	const int getConditionalQubit();
	void evaluate(const int accBitState);

	virtual const std::string toString(const std::string& bufferVarName);

//	virtual std::shared_ptr<GateFunction> clone() {
//		return std::make_shared<ConditionalFunction>();
//	}

	DEFINE_VISITABLE()

};
}
}

#endif
