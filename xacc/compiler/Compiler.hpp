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
#include "Registry.hpp"
#include "IR.hpp"
#include "Accelerator.hpp"

namespace xacc {

/**
 *
 */
class Compiler {

public:

	/**
	 * The Compiler.compile method is in charge of modifying
	 * the source code to be amenable to compilation by derived
	 * types.
	 *
	 * @param src The kernel source string.
	 * @return ir Intermediate representation for provided source kernel code.
	 */
	virtual std::shared_ptr<IR> compile(const std::string& src,
			std::shared_ptr<Accelerator> acc) = 0;

	virtual std::shared_ptr<IR> compile(const std::string& src) = 0;

	virtual ~Compiler() {}

protected:

	/**
	 *
	 */
	std::string kernelSource;

	/**
	 *
	 */
	std::shared_ptr<Accelerator> accelerator;
};

/**
 * Compiler Registry is just an alias for a
 * Registry of Compilers.
 */
using CompilerRegistry = Registry<Compiler>;

/**
 * The RegisterCompiler class simply provides
 * a convenience constructor that adds the provided template
 * parameter type to the CompilerRegistry.
 */
template<typename T>
class RegisterCompiler {
public:
	RegisterCompiler(const std::string& name) {
		CompilerRegistry::instance()->add(name,
				(std::function<std::shared_ptr<xacc::Compiler>()>) ([]() {
					return std::make_shared<T>();
				}));
	}
};
}
#endif
