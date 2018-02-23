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
#ifndef XACC_PROGRAM_HPP_
#define XACC_PROGRAM_HPP_

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <queue>
#include <boost/algorithm/string.hpp>
#include <algorithm>

#include "Utils.hpp"
#include "Compiler.hpp"
#include "Accelerator.hpp"
#include "RuntimeOptions.hpp"
#include "Preprocessor.hpp"
#include "ServiceRegistry.hpp"
#include "Kernel.hpp"
#include "IRPreprocessor.hpp"
#include "AcceleratorBufferPostprocessor.hpp"

namespace xacc {

/**
 * Placeholder, soon we will have Acc Independent transformations...
 * @param accType
 * @return
 */
std::vector<IRTransformation> getAcceleratorIndependentTransformations(
		AcceleratorType& accType);

std::vector<std::shared_ptr<Preprocessor>> getDefaultPreprocessors(
		AcceleratorType accType);

/**
 * The Program is the main entrypoint for the XACC
 * API. Users with accelerator kernels must construct a
 * valid Program to be compiled and executed on the
 * attached accelerator. Programs must be given the
 * Accelerator reference to be used and kernel source
 * code at construction time.
 */
class Program {

protected:

	/**
	 * Reference to the source accelerator
	 * kernel code to be compiled and executed
	 */
	std::string src;

	/**
	 * Reference to the attached Accelerator to
	 * use in this compilation and execution
	 */
	std::shared_ptr<Accelerator> accelerator;

	/**
	 * Reference to the XACC IR instance that is
	 * created by the Compiler
	 */
	std::shared_ptr<IR> xaccIR;

	/**
	 * Reference to the compiler
	 */
	std::shared_ptr<Compiler> compiler;

	/**
	 * Reference to user-provided Preprocessors
	 * to be run before Compiler execution.
	 */
	std::vector<std::shared_ptr<Preprocessor>> preprocessors;
	std::vector<std::shared_ptr<IRPreprocessor>> irpreprocessors;
	std::vector<std::shared_ptr<AcceleratorBufferPostprocessor>> bufferPostprocessors;

public:

	/**
	 * The Constructor, takes the Accelerator
	 * to execute on, and the source to compile and execute
	 *
	 * @param acc Attached Accelerator to execute
	 * @param sourceFile The kernel source code
	 */
	Program(std::shared_ptr<Accelerator> acc, const std::string& sourceFile) :
			src(sourceFile), accelerator(std::move(acc)) {
		addIRPreprocessor("qubit-map-preprocessor");
	}

	Program(std::shared_ptr<Accelerator> acc, std::shared_ptr<xacc::IR> ir) :
			xaccIR(ir), accelerator(std::move(acc)) {
		addIRPreprocessor("qubit-map-preprocessor");
	}

	/**
	 * The Constructor, takes the Accelerator to execute on,
	 * and the source kernel string as a stream.
	 * @param acc Attached Accelerator to execute
	 * @param stream The file stream containing kernel source code
	 */
	Program(std::shared_ptr<Accelerator> acc, std::istream& stream) :
			accelerator(std::move(acc)), src(
					std::istreambuf_iterator<char>(stream), { }) {
		addIRPreprocessor("qubit-map-preprocessor");
	}

	void addPreprocessor(std::shared_ptr<Preprocessor> preprocessor) {
		preprocessors.push_back(preprocessor);
	}

	void addPreprocessor(const std::string& preProcessorName) {
        if(ServiceRegistry::instance()->hasService<Preprocessor>(preProcessorName)) {
			auto preprocessor = ServiceRegistry::instance()->getService<Preprocessor>(preProcessorName);
			preprocessors.push_back(preprocessor);
		}
	}

	void addIRPreprocessor(const std::string& name) {
		if (ServiceRegistry::instance()->hasService<IRPreprocessor>(
				name)) {
			auto p = ServiceRegistry::instance()->getService<
					IRPreprocessor>(name);
			irpreprocessors.push_back(p);
		}
	}

	std::vector<std::shared_ptr<AcceleratorBufferPostprocessor>> getBufferPostprocessors() {
		return bufferPostprocessors;
	}

	std::shared_ptr<IR> getIR() {
		return xaccIR;
	}

	/**
	 * Execute the compilation mechanism on the provided program
	 * source kernel code to produce XACC IR that can be executed
	 * on the attached Accelerator.
	 */
	virtual void build();

	/**
	 * Return the number of Kernels in this Program's XACC IR.
	 *
	 * @return nKernels The number of kernels.
	 */
	const int nKernels() {
		if (!xaccIR) build();

		return xaccIR->getKernels().size();
	}

	/**
	 * Return an executable version of the kernel
	 * referenced by the kernelName string.
	 *
	 * @param name The name of the kernel
	 * @return kernel The Kernel represented by kernelName
	 */
	template<typename ... RuntimeArgs>
	auto getKernel(
			const std::string& kernelName) -> Kernel<RuntimeArgs...> {

		// Build the kernel with the appropriate compiler
		if (!xaccIR) {
			build();
		}

		return Kernel<RuntimeArgs...>(accelerator, xaccIR->getKernel(kernelName));
	}

	/**
	 * Return all Kernels that have sizeof...(RuntimeArgs)
	 * InstructionParameters.
	 * @return kernels Kernels with sizeof...(RuntimeArgs) Parameters.
	 */
	template<typename ... RuntimeArgs>
	auto getKernels() -> KernelList<RuntimeArgs...> {
		if (!xaccIR) {
			build();
		}

		KernelList<RuntimeArgs...> kernels(accelerator, bufferPostprocessors);
		for (auto k : xaccIR->getKernels()) {
			if (k->nParameters() == (sizeof...(RuntimeArgs))) {
				kernels.push_back(Kernel<RuntimeArgs...>(accelerator, k));
			}
		}

		return kernels;
	}

	/**
	 * Return the Kernel range given by the provided indices.
	 * @param beginIdx The beginning index for the range
	 * @param endIdx The endindex for the range
	 * @return kernels The kernels in the given range.
	 */
	template<typename ... RuntimeArgs>
	auto getKernels(const int beginIdx, const int endIdx) -> KernelList<RuntimeArgs...> {
		KernelList<RuntimeArgs...> kernels(accelerator, bufferPostprocessors);
		if (!xaccIR) {
			build();
		}

		for (int i = beginIdx; i < endIdx; i++) {
			if (xaccIR->getKernels()[i]->nParameters() == (sizeof...(RuntimeArgs))) {
				kernels.push_back(Kernel<RuntimeArgs...>(accelerator, xaccIR->getKernels()[i]));
			}
		}

		return kernels;
	}

	/**
	 * Return a list of Kernels with now runtime type information
	 * for the arguments. Any arguments must be provided
	 * as InstructionParameters.
	 */
	auto getRuntimeKernels() -> KernelList<> {
		if (!xaccIR) {
			build();
		}

		KernelList<> kernels(accelerator, bufferPostprocessors);
		for (auto k : xaccIR->getKernels()) {
			kernels.push_back(Kernel<>(accelerator, k));
		}

		return kernels;
	}
};

}
#endif
