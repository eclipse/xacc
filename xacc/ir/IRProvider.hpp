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
#ifndef XACC_COMPILER_IRPROVIDER_HPP_
#define XACC_COMPILER_IRPROVIDER_HPP_
#include "Instruction.hpp"
#include "IR.hpp"
#include "Function.hpp"

namespace xacc {

class __attribute__((visibility("default"))) IRProvider : public Identifiable {
public:
	virtual std::shared_ptr<Instruction> createInstruction(const std::string name,
			std::vector<int> bits,
			std::vector<InstructionParameter> parameters = std::vector<
					InstructionParameter> { }) = 0;
	virtual std::shared_ptr<Function> createFunction(const std::string name,
			std::vector<int> bits,
			std::vector<InstructionParameter> parameters = std::vector<
					InstructionParameter> { }) = 0;
	virtual std::shared_ptr<IR> createIR() = 0;
	virtual std::vector<std::string> getInstructions() = 0;
	virtual ~IRProvider() {}
};

}
#endif
