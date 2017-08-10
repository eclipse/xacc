/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
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
#ifndef XACC_COMPILER_HPP_
#define XACC_COMPILER_HPP_

#include <memory>
#include <iostream>
#include "IR.hpp"
#include "Accelerator.hpp"
#include "Identifiable.hpp"

namespace xacc {

/**
 * The Compiler class provides an extensible interface
 * for injecting custom compilation mechanisms into the
 * XACC framework. Implementations provide a compile method
 * that takes the kernel source code string, performs
 * compiler-specific compilation mechanism, and returns a valid
 * XACC IR instance modeling the result of the compilation.
 */
class __attribute__((visibility("default"))) Compiler : public OptionsProvider, public Identifiable {

public:

	/**
	 * This method is to be implemented by derived Compilers
	 * and is in charge of executing the compilation mechanism
	 * on the provided source string. Implementations also are
	 * given access to the Accelerator that this source code is
	 * intended for.
	 *
	 * @param src The kernel source string.
	 * @param acc The Accelerator this code will be executed on
	 * @return ir Intermediate representation for provided source kernel code.
	 */
	virtual std::shared_ptr<IR> compile(const std::string& src,
			std::shared_ptr<Accelerator> acc) = 0;

	/**
	 * This method is to be implemented by derived Compilers
	 * and is in charge of executing the compilation mechanism
	 * on the provided source string.
	 * @param src
	 * @return
	 */
	virtual std::shared_ptr<IR> compile(const std::string& src) = 0;

	/**
	 * This method is to be implemented by derived Compilers and
	 * is in charge of taking the provided Function IR and converting
	 * it to source code in this Compiler's language.
	 *
	 * @param function The XACC IR Function to translate
	 * @return src The source code as a string
	 */
	virtual const std::string translate(const std::string& bufferVariable,
			std::shared_ptr<Function> function) = 0;

	/**
	 * Return the name of this Compiler
	 * @return name Compiler name
	 */
	virtual const std::string getName() = 0;

	/**
	 * Return an empty options_description, this is for
	 * subclasses to implement.
	 */
	virtual std::shared_ptr<options_description> getOptions() {
		return std::make_shared<options_description>();
	}

	virtual bool handleOptions(variables_map& map){
		return false;
	}

	/**
	 * The destructor
	 */
	virtual ~Compiler() {}

protected:

	/**
	 * Reference to the provided kernel source code string
	 */
	std::string kernelSource;

	/**
	 * Reference to the Accelerator that this compiler is
	 * targeting.
	 */
	std::shared_ptr<Accelerator> accelerator;
};

}
#endif
