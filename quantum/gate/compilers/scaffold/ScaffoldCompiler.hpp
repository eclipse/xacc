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

#include "Compiler.hpp"
#include "XACCError.hpp"
#include <boost/algorithm/string.hpp>
#include "Accelerator.hpp"
#include "GraphIR.hpp"
#include "ScaffCCAPI.hpp"
#include "QasmToGraph.hpp"
#include "QuantumCircuit.hpp"

namespace xacc {

namespace quantum {

/**
 * The Scaffold compiler is a subclass of the XACC
 * Compiler that implements the compile() and modifySource() methods
 * to handle generation of quantum assembly language (or QASM)
 * using an installed Scaffold compiler.
 */
class ScaffoldCompiler : public xacc::Compiler {

public:

	/**
	 * Execute the Scaffold compiler to generate an
	 * XACC intermediate representation instance.
	 * @return ir XACC intermediate representation
	 */
	virtual std::shared_ptr<xacc::IR> compile(const std::string& src,
			std::shared_ptr<Accelerator> acc);

	virtual std::shared_ptr<xacc::IR> compile(const std::string& src);

	/**
	 *
	 * @return
	 */
	virtual std::string getBitType() {
		return "qbit";
	}

	/**
	 * The destructor
	 */
	virtual ~ScaffoldCompiler() {}

protected:

	/**
	 * This method is intended to modify the incoming
	 * source code to be compiled to be amenable to the
	 * Scaffold compiler.
	 */
	virtual void modifySource();

	/**
	 * Reference to potential conditional code
	 */
	std::vector<std::string> conditionalCodeSegments;

	/**
	 *
	 */
	std::vector<int> conditionalCodeSegmentActingQubits;

	std::map<int, std::vector<std::string>> gateIdToParameterMap;

	/**
	 *
	 */
	std::map<std::string, std::string> typeToVarKernelArgs;

	std::vector<std::string> orderOfArgs;

	std::vector<std::string> orderedVarNames;

	virtual std::vector<std::string> getKernelArgumentVariableNames() {
		return orderedVarNames;
	}

	void kernelArgsToMap() {

			auto firstParen = kernelSource.find_first_of('(');
			auto secondParen = kernelSource.find_first_of(')', firstParen);
			auto functionArguments = kernelSource.substr(firstParen+1, (secondParen-firstParen)-1);
			int counter = 0;

			if (!functionArguments.empty()) {
				// First search the prototype to see if it has
				// and argument that declares the accelerator bit buffer
				// to use in the kernel
				std::vector<std::string> splitArgs, splitTypeVar;
				boost::split(splitArgs, functionArguments, boost::is_any_of(","));
				std::string varName;
				for (int i = 0; i < splitArgs.size(); i++) {
					// split type from var name
					auto s = splitArgs[i];
					boost::trim(s);
					boost::split(splitTypeVar, s, boost::is_any_of(" "));
					auto type = splitTypeVar[0];
					auto var = splitTypeVar[1];
					boost::trim(type);
					boost::trim(var);
					typeToVarKernelArgs.insert(std::make_pair(type, var));
					orderOfArgs.push_back(type);
					orderedVarNames.push_back(var);
				}

			}
		}
};

}

}
#endif
