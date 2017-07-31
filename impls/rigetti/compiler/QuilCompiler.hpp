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
#ifndef IMPLS_RIGETTI_QUILCOMPILER_HPP_
#define IMPLS_RIGETTI_QUILCOMPILER_HPP_


#include "Compiler.hpp"
#include "Utils.hpp"
#include <boost/algorithm/string.hpp>

namespace xacc {

namespace quantum {

/**
 */
class QuilCompiler: public xacc::Compiler {

public:

	QuilCompiler();

	/**
	 * Translate Quil to the
	 * XACC intermediate representation.
	 *
	 * @return ir XACC intermediate representation
	 */
	virtual std::shared_ptr<xacc::IR> compile(const std::string& src,
			std::shared_ptr<Accelerator> acc);

	/**
	 *
	 * @param src
	 * @return
	 */
	virtual std::shared_ptr<xacc::IR> compile(const std::string& src);

	/**
	 * Return the name of this Compiler
	 * @return name Compiler name
	 */
	virtual const std::string getName() {
		return "quil";
	}

	/**
	 * This produces a Quil source code representation of the
	 * given IR Function
	 *
	 * @param function The XACC IR Function to translate
	 * @return src The source code as a string
	 */
	virtual const std::string translate(const std::string& bufferVariable,
			std::shared_ptr<Function> function);

	virtual const std::string name() const {
		return "quil";
	}

	virtual const std::string description() const {
		return "The Quil Compiler compiles kernels written in the Quil intermediate language.";
	}

	/**
	 * The destructor
	 */
	virtual ~QuilCompiler() {}

};

}

}

#endif
