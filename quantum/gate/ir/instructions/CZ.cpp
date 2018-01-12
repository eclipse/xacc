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
#include "CZ.hpp"

namespace xacc {
namespace quantum {

CZ::CZ(std::vector<int> qbits) :
		GateInstruction("CZ", qbits) {
}

CZ::CZ(int srcqbit, int tgtqbit) :
		CZ(std::vector<int> { srcqbit, tgtqbit }) {
}
//
//void CZ::accept(std::shared_ptr<InstructionVisitor> visitor) {
//	auto v = std::dynamic_pointer_cast<GateInstructionVisitor>(visitor);
//	if (v) {
//		v->visit(*this);
//	} else {
//		visitor->visit(*this);
//	}
//}

RegisterGateInstruction<CZ> CZTEMP("CZ");

}
}
