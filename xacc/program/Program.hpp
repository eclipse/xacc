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

std::vector<std::shared_ptr<Preprocessor>> getDefaultPreprocessors(AcceleratorType accType) {
	std::vector<std::shared_ptr<Preprocessor>> preprocessors;
	auto ppRegistry = PreprocessorRegistry::instance();
	if (accType == AcceleratorType::qpu_gate) {
		preprocessors.push_back(
				ppRegistry->create("kernel-replacement"));
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

	/**
	 * Execute the compilation mechanism on the provided program
	 * source kernel code to produce XACC IR that can be executed
	 * on the attached Accelerator.
	 *
	 */
	void build() {

		// Get reference to the runtime options
		auto runtimeOptions = RuntimeOptions::instance();

		// Get the compiler that has been requested.
		auto compilerToRun = (*runtimeOptions)["compiler"];

		// Create the appropriate compiler
		compiler = xacc::CompilerRegistry::instance()->create(compilerToRun);

		// Make sure we got a valid
		if (!compiler) {
			XACCError("Invalid Compiler.\n");
		}

		// Before compiling, run preprocessors
		auto defaultPPs = getDefaultPreprocessors(accelerator->getType());
		for (auto preprocessor : defaultPPs) {
			src = preprocessor->process(src, compiler, accelerator);
//			std::cout << "Src after process:\n" << src << "\n";
		}

		XACCInfo("Executing "+ compiler->getName() + " compiler.");

		// Execute the compilation
		xaccIR = compiler->compile(src, accelerator);

		// Validate the compilation
		if (!xaccIR) {
			XACCError("Bad source string or something.\n");
		}

		// Write the IR to file if the user requests it
		if (runtimeOptions->exists("persist-ir")) {
			auto fileStr = (*runtimeOptions)["persist-ir"];
			std::ofstream ostr(fileStr);
			xaccIR->persist(ostr);
		}

		return;
	}

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
	 * Return an executable version of the quantum kernel
	 * referenced by the kernelName string.
	 *
	 * @param name
	 * @param args
	 * @return
	 */
	template<typename ... RuntimeArgs>
	std::function<void(std::shared_ptr<AcceleratorBuffer>, RuntimeArgs...)> getKernel(
			const std::string& kernelName) {

		// Build the kernel with the appropriate compiler
		build();

		// Create a lambda that executes the kernel on the Accelerator.
		return [=](std::shared_ptr<AcceleratorBuffer> buffer, RuntimeArgs... args) {

			// Get the Function for the Kernel from the IR
			auto kernel = xaccIR->getKernel(kernelName);

			if (sizeof...(RuntimeArgs) > 0) {
				// Store the runtime parameters in a tuple
				auto argsTuple = std::make_tuple(args...);

				// Loop through the tuple, and add InstructionParameters
				// to the parameters vector.
				std::vector<InstructionParameter> parameters;
				xacc::tuple_for_each(argsTuple, [&](auto value) {
					parameters.push_back(InstructionParameter(value));
				});

				// Evaluate all Variable Parameters
				kernel->evaluateVariableParameters(parameters);
			}

			// Execute the Kernel on the Accelerator
			accelerator->execute(buffer, kernel);

			return;
		};
	}
};

}
#endif
