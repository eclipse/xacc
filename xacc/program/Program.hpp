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
 *   * Neither the name of the <organization> nor the
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
#include "Compiler.hpp"
#include "Kernel.hpp"
#include "Accelerator.hpp"
#include <memory>
#include <queue>
#include <boost/program_options.hpp>
#include <algorithm>
#include "QCIError.hpp"
#include "XaccUtils.hpp"
#include "XACC.hpp"

using namespace boost::program_options;
using namespace qci::common;

namespace xacc {

/**
 *
 */
class Program {

protected:

	std::string src;

	std::shared_ptr<Accelerator> accelerator;

	std::shared_ptr<options_description> compilerOptions;

public:

	Program(std::shared_ptr<Accelerator> acc, const std::string& sourceFile) :
			src(sourceFile) {
		accelerator = std::move(acc);
		compilerOptions = std::make_shared<options_description>(
				"XACC Compiler Options");
		compilerOptions->add_options()("help", "Help Message")("compiler",
				value<std::string>()->default_value("scaffold"),
				"Indicate the compiler to be used.");
	}

	template<typename ... RuntimeArgs>
	void build(const std::string& compilerArgStr, RuntimeArgs ... runtimeArgs) {

		// Get the user-specified compiler parameters as a map
		variables_map compileParameters;
		store(
				command_line_parser(tokenize(compilerArgStr)).options(
						*compilerOptions.get()).run(), compileParameters);

		// Get the compiler name
		auto compilerToRun = compileParameters["compiler"].as<std::string>();

		// Create the appropriate compiler
		auto compiler = std::shared_ptr<ICompiler>(
				qci::common::AbstractFactory::createAndCast<ICompiler>(
						"compiler", compilerToRun));

		// Make sure we got a valid
		if (!compiler) {
			QCIError("Invalid Compiler.\n");
		}

		// Execute the compilation
		auto ir = compiler->compile(src);

		// Validate the compilation
		if (!ir) {
			QCIError("Bad source string or something.\n");
		}

		// Execute IR Translations and Optimizations
		// FIXME GET LIST OF TRANSFORMATION FROM
		auto acceleratorType = accelerator->getType();
		auto defaultTransforms = getAcceleratorIndependentTransformations(acceleratorType);
		auto accDepTransforms = accelerator->getIRTransformations();

		for (IRTransformation& t : defaultTransforms) {
			t.transform(*ir.get());
		}

		for (IRTransformation& t : accDepTransforms) {
			t.transform(*ir.get());
		}

		// Create Kernel from IR

		return;
	}

	void getKernel(std::string& name) {}
};

}
#endif
