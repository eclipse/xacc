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
#ifndef XACC_COMPILER_IRTRANSLATION_HPP_
#define XACC_COMPILER_IRTRANSLATION_HPP_

#include "IR.hpp"
#include "Identifiable.hpp"

namespace xacc {

/**
 * The IRTransformation interface provides a method
 * that subclasses may implement to
 * modify, transform, optimize, or translate
 * the provided IR, producing a new IR instance
 * that is isomorphic to the input IR.
 */
class IRTransformation : public Identifiable {
public:

	/**
	 * Transform the given IR into a new isomorphic
	 * IR.
	 *
	 * @param ir Input IR to modify
	 * @return newIr Transformed IR
	 */
	virtual std::shared_ptr<IR> transform(std::shared_ptr<IR> ir) = 0;

	/**
	 * The destructor
	 */
	virtual ~IRTransformation() {}
};

}
#endif
