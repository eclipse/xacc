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
#ifndef QUANTUM_AQC_COMPILERS_DWQMICOMPILER_HPP_
#define QUANTUM_AQC_COMPILERS_DWQMICOMPILER_HPP_

#include "Compiler.hpp"
#include "Utils.hpp"
#include "DWIR.hpp"
#include "DWGraph.hpp"
#include "EmbeddingAlgorithm.hpp"

namespace xacc {

namespace quantum {

/**
 */
class DWQMICompiler: public xacc::Compiler {

public:

	DWQMICompiler();

	/**
	 */
	virtual std::shared_ptr<xacc::IR> compile(const std::string& src,
			std::shared_ptr<Accelerator> acc);

	/**
	 *
	 * @return
	 */
	virtual std::shared_ptr<xacc::IR> compile(const std::string& src);

	/**
	 * Return the name of this Compiler
	 * @return name Compiler name
	 */
	virtual const std::string getName() {
		return "dwave-qmi";
	}

	virtual std::shared_ptr<options_description> getOptions() {
		auto desc = std::make_shared<options_description>(
				"D-Wave QMI Compiler Options");
		desc->add_options()("dwave-embedding", value<std::string>(),
				"Provide the name of the Embedding Algorithm to use during compilation.");
		return desc;
	}

	/**
	 * Register this Compiler with the framework.
	 */
	static void registerCompiler() {
		DWQMICompiler c;
		xacc::RegisterCompiler<xacc::quantum::DWQMICompiler> DWQMITEMP(
				"dwave-qmi", c.getOptions());
	}

	virtual const std::string translate(const std::string& bufferVariable,
			std::shared_ptr<Function> function) {
		XACCError("DWQMICompiler::translate - Method not implemented");
	};

	/**
	 * The destructor
	 */
	virtual ~DWQMICompiler() {}

};

RegisterCompiler(xacc::quantum::DWQMICompiler)

}

}

#endif
