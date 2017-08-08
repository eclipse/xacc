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

namespace xacc {

/**
 * Placeholder, soon we will have Acc Independent transformations...
 * @param accType
 * @return
 */
std::vector<IRTransformation> getAcceleratorIndependentTransformations(
		AcceleratorType& accType) {
	std::vector<IRTransformation> transformations;
	return transformations;
}

std::vector<std::shared_ptr<Preprocessor>> getDefaultPreprocessors(
		AcceleratorType accType) {
	std::vector<std::shared_ptr<Preprocessor>> preprocessors;
	auto preprocessor = ServiceRegistry::instance()->getService<Preprocessor>(
			"kernel-replacement");
	if (accType == AcceleratorType::qpu_gate) {
		preprocessors.push_back(preprocessor);
	}

	return preprocessors;
}

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
	}

	/**
	 * Execute the compilation mechanism on the provided program
	 * source kernel code to produce XACC IR that can be executed
	 * on the attached Accelerator.
	 */
	void build() {

		// Get reference to the runtime options
		auto runtimeOptions = RuntimeOptions::instance();

		// Get the compiler that has been requested.
		auto compilerToRun = (*runtimeOptions)["compiler"];

		// Create the appropriate compiler
		compiler = xacc::ServiceRegistry::instance()->getService<Compiler>(
				compilerToRun);

		// Make sure we got a valid
		if (!compiler) {
			XACCError("Invalid Compiler.\n");
		}

		// Before compiling, run preprocessors
		auto defaultPPs = getDefaultPreprocessors(accelerator->getType());
		for (auto preprocessor : defaultPPs) {
			src = preprocessor->process(src, compiler, accelerator);
		}

		XACCInfo("Executing "+ compiler->getName() + " compiler.");

		// Execute the compilation
		xaccIR = compiler->compile(src, accelerator);

		// Validate the compilation
		if (!xaccIR) {
			XACCError("Bad source string or something.\n");
		}

		// Execute hardware dependent IR Transformations
		auto accTransforms = accelerator->getIRTransformations();
		for (auto t : accTransforms) {
			xaccIR = t->transform(xaccIR);
		}

		// Write the IR to file if the user requests it
		if (runtimeOptions->exists("persist-ir")) {
			auto fileStr = (*runtimeOptions)["persist-ir"];
			std::ofstream ostr(fileStr);
			xaccIR->persist(ostr);
		}

		return;
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
	auto getKernels() -> std::vector<Kernel<RuntimeArgs...>> {
		std::vector<Kernel<RuntimeArgs...>> kernels;
		if (!xaccIR) {
			build();
		}

		for (auto k : xaccIR->getKernels()) {
			if (k->nParameters() == (sizeof...(RuntimeArgs))) {
				kernels.push_back(Kernel<RuntimeArgs...>(accelerator, k));
			}
		}

		return kernels;
	}
};

}
#endif
