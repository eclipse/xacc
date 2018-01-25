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
#ifndef XACC_COMPILER_IR_HPP_
#define XACC_COMPILER_IR_HPP_
#include <iostream>
#include "AcceleratorBuffer.hpp"
#include "Function.hpp"

namespace xacc {

/**
 * The IR interface is the base interface for
 * derived accelerator-specific intermediate representations.
 * At this level, an intermediate representation can be
 * persisted to an assembly-like string, can be read in from
 * file, and can be persisted to file. Since all XACC
 * intermediate representations operate on an Accelerator Buffer,
 * the IR interface also provides a setter for such a buffer.
 *
 */
class IR {

public:

	/**
	 * Return a assembly-like string representation of this
	 * intermediate representation
	 *
	 * @param kernelName The name of hte kernel to persist to string
	 * @param accBufferVarName The name of the AcceleratorBuffer
	 * @return
	 */
	virtual std::string toAssemblyString(const std::string& kernelName,
			const std::string& accBufferVarName) = 0;

	/**
	 * Persist this IR instance to the given
	 * output stream.
	 *
	 * @param outStream The output stream to persist to.
	 */
	virtual void persist(std::ostream& outStream) = 0;

	/**
	 * Create this IR instance from the given input
	 * stream.
	 *
	 * @param inStream The input stream to read from.
	 */
	virtual void load(std::istream& inStream) = 0;

	/**
	 * Add a new kernel to this IR instance.
	 *
	 * @param kernel The Function instance to add as a new kernel
	 */
	virtual void addKernel(std::shared_ptr<Function> kernel) = 0;

	/**
	 * Return true if the kernel with given name exists in this IR.
	 *
	 * @param name The name of the kernel to return.
	 * @return exists True if kernel exists.
	 */
	virtual bool kernelExists(const std::string& name) = 0;

	/**
	 * Return the kernel with the given name.
	 *
	 * @param name The name of the kernel to return.
	 * @return kernel The kernel with given name.
	 */
	virtual std::shared_ptr<Function> getKernel(const std::string& name) = 0;

	/**
	 * Return all of this IR instance's kernels.
	 *
	 * @return kernels The kernels this IR contains.
	 */
	virtual std::vector<std::shared_ptr<Function>> getKernels() = 0;

	virtual void mapBits(std::vector<int> bitMap) = 0;

	virtual const int maxBit() = 0;

	/**
	 * The destructor
	 */
	virtual ~IR() {
	}

};

}
#endif
