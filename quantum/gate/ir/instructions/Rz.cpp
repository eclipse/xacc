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
#include "Rz.hpp"

namespace xacc {
namespace quantum {

Rz::Rz(int qbit, double theta) :
		GateInstruction("Rz", std::vector<int> { qbit },
				std::vector<InstructionParameter> { InstructionParameter(theta) }) {
}

Rz::Rz(std::vector<int> qbits) :
		GateInstruction("Rz", qbits, std::vector<InstructionParameter> {
				InstructionParameter(0.0) }) {

}

}
}
