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
#ifndef IR_IRGENERATOR_HPP_
#define IR_IRGENERATOR_HPP_

#include "AcceleratorBuffer.hpp"
#include "Function.hpp"
#include "Identifiable.hpp"
#include <vector>
#include <memory>

namespace xacc {

/**
 * The IRGenerator interface provides a mechanism for
 * generating common algorithms modeled as an XACC Function instance.
 *
 * @author Alex McCaskey
 */
class IRGenerator : public Identifiable {

public:

	/**
	 * Implementations of this method generate a Function IR
	 * instance corresponding to the implementation's modeled
	 * algorithm. The algorithm is specified to operate over the
	 * provided AcceleratorBuffer and can take an optional
	 * vector of InstructionParameters.
	 *
	 * @param bits The bits this algorithm operates on
	 * @return function The algorithm represented as an IR Function
	 */
	virtual std::shared_ptr<Function> generate(
			std::shared_ptr<AcceleratorBuffer> buffer,
			std::vector<InstructionParameter> parameters = std::vector<
					InstructionParameter> { }) = 0;

	/**
	 * The destructor
	 */
	virtual ~IRGenerator() {}
};

}
#endif
