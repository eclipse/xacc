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
#ifndef XACC_XACC_HPP_
#define XACC_XACC_HPP_

#include <iostream>
#include <memory>
#include "Utils.hpp"
#include "Compiler.hpp"
#include "Accelerator.hpp"
#include "CLIParser.hpp"
#include "Program.hpp"
#include "Preprocessor.hpp"

namespace xacc {

bool xaccFrameworkInitialized = false;
auto tmpInitConsole = spdlog::stdout_logger_mt("xacc-console");
auto xaccCLParser = std::make_shared<CLIParser>();

/**
 * This method should be called by
 * clients to initialize the XACC framework.
 * It should be called before using any of the
 * XACC API.
 */
void Initialize(int argc, char** argv) {
	XACCInfo("[xacc] Initializing XACC Framework");

	// Get reference to our compiler and accelerator registries
	auto compilerRegistry = xacc::CompilerRegistry::instance();
	auto acceleratorRegistry = xacc::AcceleratorRegistry::instance();
	auto preprocessorRegistry = xacc::PreprocessorRegistry::instance();

	// Parse any user-supplied command line options
//	CLIParser parser(argc, argv);
	xaccCLParser->parse(argc, argv);

	// Check that we have some
	auto s = compilerRegistry->size();
	auto a = acceleratorRegistry->size();
	auto ps = preprocessorRegistry->size();
	if (s == 0) XACCError("There are no Compiler instances available. Exiting.");
	if (a == 0) XACCError("There are no Accelerator instances available. Exiting.");

	XACCInfo(
			"[xacc::compiler] XACC has " + std::to_string(s) + " Compiler"
					+ ((s == 1 || s == 0) ? "" : "s") + " available.");
	XACCInfo(
			"[xacc::accelerator] XACC has " + std::to_string(a) + " Accelerator"
					+ ((s == 0 || s == 1) ? "" : "s") + " available.");
	XACCInfo(
			"[xacc::preprocessor] XACC has " + std::to_string(ps) + " Preprocessor"
					+ ((ps == 0 || ps == 1) ? "" : "s") + " available.");

	// We're good if we make it here, so indicate that we've been
	// initialized
	xacc::xaccFrameworkInitialized = true;
}

void addCommandLineOption(const std::string& optionName, const std::string& optionDescription = "") {
	xaccCLParser->addStringOption(optionName, optionDescription);
}

std::shared_ptr<Accelerator> getAccelerator(const std::string& name) {
	if (!xacc::xaccFrameworkInitialized) {
		XACCError(
				"XACC not initialized before use. Please execute xacc::Initialize() before using API.");
	}
	auto acc = AcceleratorRegistry::instance()->create(name);
	if (acc) {
		acc->initialize();
		return acc;
	} else {
		XACCError(
				"Invalid Accelerator. Could not find " + name
						+ " in Accelerator Registry.");
	}
}

/**
 * This method should be called by clients to
 * clean up and finalize the XACC framework. It should
 * be called after using the XACC API.
 */
void Finalize() {
	XACCInfo(
			"\n[xacc] XACC Finalizing\n[xacc::compiler] Cleaning up Compiler Registry."
					"\n[xacc::accelerator] Cleaning up Accelerator Registry.");
	xacc::CompilerRegistry::instance()->destroy();
	xacc::AcceleratorRegistry::instance()->destroy();
	xacc::PreprocessorRegistry::instance()->destroy();
	xacc::xaccFrameworkInitialized = false;
}
}


#endif
