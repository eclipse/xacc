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
#ifndef IR_ALGORITHMS_ALGORITHMGENERATOR_HPP_
#define IR_ALGORITHMS_ALGORITHMGENERATOR_HPP_

#include "Function.hpp"
#include "Identifiable.hpp"

namespace xacc {

/**
 * The AlgorithmGenerator interface provides a mechanism for
 * generating algorithms modeled as an XACC Function instance.
 *
 * @author Alex McCaskey
 */
class __attribute__((visibility("default"))) AlgorithmGenerator : public Identifiable {

public:

	/**
	 * Implementations of this method generate a Function IR
	 * instance corresponding to the implementation's modeled
	 * algorithm. The algorithm is specified to operate over the
	 * provided bits.
	 *
	 * @param bits The bits this algorithm operates on
	 * @return function The algorithm represented as an IR Function
	 */
	virtual std::shared_ptr<Function> generateAlgorithm(std::vector<int> bits) = 0;

	/**
	 * The destructor
	 */
	virtual ~AlgorithmGenerator() {}
};

}
#endif
