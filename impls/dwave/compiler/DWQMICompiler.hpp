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

#include "ServiceRegistry.hpp"

namespace xacc {

namespace quantum {

/**
 * The DWQMICompiler is an XACC Compiler that compiles
 * D-Wave quantum machine instructions to produce an
 * appropriate Ising form for execution on the D-Wave QPU.
 *
 * This compilation leverages XACC EmbeddingAlgorithms to
 * compute the minor graph embedding represented by the
 * input source kernel code to output the embedded Ising
 * graph for D-Wave execution.
 */
class DWQMICompiler: public xacc::Compiler {

public:

	/**
	 * The Compiler.
	 */
	DWQMICompiler() {}

	/**
	 * Compile the given kernel code for the
	 * given D-Wave Accelerator.
	 *
	 * @param src The QMI source code
	 * @param acc Reference to the D-Wave Accelerator
	 * @return
	 */
	virtual std::shared_ptr<xacc::IR> compile(const std::string& src,
			std::shared_ptr<Accelerator> acc);

	/**
	 * This method is not implemented - we must always have
	 * D-Wave Accelerator connectivity information for compilation.
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

	/**
	 * Return the command line options for this compiler
	 *
	 * @return options Description of command line options.
	 */
	virtual std::shared_ptr<options_description> getOptions() {
		auto desc = std::make_shared<options_description>(
				"D-Wave QMI Compiler Options");
		desc->add_options()("dwave-embedding", value<std::string>(),
				"Provide the name of the Embedding Algorithm to use during compilation.")(
				"dwave-parameter-setter", value<std::string>(),
				"Provide the name of the "
						"ParameterSetter to map logical parameters to physical parameters.")
						("dwave-load-embedding", value<std::string>(), "Use the embedding in the given file.")
						("dwave-persist-embedding", value<std::string>(), "Persist the computed embedding to the given file name.")
						("dwave-list-embedding-algorithms", "List all available embedding algorithms.");
		return desc;
	}

	virtual bool handleOptions(variables_map& map) {
		if (map.count("dwave-list-embedding-algorithms")) {
			auto ids = ServiceRegistry::instance()->getRegisteredIds<EmbeddingAlgorithm>();
			for (auto i : ids) {
				XACCInfo("Registered Embedding Algorithm: " + i);
			}
			return true;
		}
		return false;
	}

	/**
	 * We don't allow translations for the DW Compiler.
	 * @param bufferVariable
	 * @param function
	 * @return
	 */
	virtual const std::string translate(const std::string& bufferVariable,
			std::shared_ptr<Function> function) {
		XACCError("DWQMICompiler::translate - Method not implemented");
	};

	virtual const std::string name() const {
		return "dwave-qmi";
	}

	virtual const std::string description() const {
		return "The D-Wave QMI Compiler takes quantum machine instructions "
				"and performs minor graph embedding and parameter setting.";
	}

	/**
	 * The destructor
	 */
	virtual ~DWQMICompiler() {}

};

}

}

#endif
