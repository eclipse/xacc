/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
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

	/**
	 * The destructor
	 */
	virtual ~IR() {
	}

};

}
#endif
