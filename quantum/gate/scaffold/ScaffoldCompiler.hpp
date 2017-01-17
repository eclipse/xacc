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
#ifndef QUANTUM_SCAFFOLDCOMPILER_HPP_
#define QUANTUM_SCAFFOLDCOMPILER_HPP_

#include <iostream>
#include <memory>
#include "AbstractFactory.hpp"
#include "Compiler.hpp"
#include "QCIError.hpp"

using namespace qci::common;

namespace xacc {

namespace quantum {

/**
 * The Scaffold compiler is a subclass of the XACC
 * Compiler that implements the compile() and modifySource() methods
 * to handle generation of quantum assembly language (or QASM)
 * using an installed Scaffold compiler.
 */
class ScaffoldCompiler : public Compiler<ScaffoldCompiler> {

public:

	/**
	 * Execute the Scaffold compiler to generate an
	 * XACC intermediate representation instance.
	 * @return ir XACC intermediate representation
	 */
	virtual std::shared_ptr<IR> compile();

	/**
	 * This method is intended to modify the incoming
	 * source code to be compiled to be amenable to the
	 * Scaffold compiler.
	 */
	virtual void modifySource();

	/**
	 * The destructor
	 */
	virtual ~ScaffoldCompiler() {}

};

}

}
#endif
