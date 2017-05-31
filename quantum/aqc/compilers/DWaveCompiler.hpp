/*
 * DWaveCompiler.hpp
 *
 *  Created on: May 30, 2017
 *      Author: aqw
 */

#ifndef QUANTUM_AQC_COMPILERS_DWAVECOMPILER_HPP_
#define QUANTUM_AQC_COMPILERS_DWAVECOMPILER_HPP_

#include "Compiler.hpp"
#include "Utils.hpp"
#include "EmbeddingAlgorithm.hpp"
#include "DWaveIR.hpp"

namespace xacc {

namespace quantum {

/**
 */
class DWaveCompiler: public xacc::Compiler {

public:

	DWaveCompiler();

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
		return "DWave";
	}

	/**
	 * Register this Compiler with the framework.
	 */
	static void registerCompiler() {
		xacc::RegisterCompiler<xacc::quantum::DWaveCompiler> Scaffold(
				"DWave");
	}

	/**
	 * The destructor
	 */
	virtual ~DWaveCompiler() {}

};

RegisterCompiler(xacc::quantum::DWaveCompiler)

}

}

#endif
