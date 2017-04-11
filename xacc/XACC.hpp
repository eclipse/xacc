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
#ifndef XACC_XACC_HPP_
#define XACC_XACC_HPP_

#include <iostream>
#include <memory>
#include "spdlog/spdlog.h"
#include "Program.hpp"

namespace xacc {

bool xaccFrameworkInitialized = false;

/**
 * This method should be called by
 * clients to initialize the XACC framework.
 * It should be called before using any of the
 * XACC API.
 */
void Initialize() {
	auto console = spdlog::stdout_logger_mt("console", true);
	console->info("[xacc] Initializing XACC Framework");
	auto compilerRegistry = xacc::CompilerRegistry::instance();
	auto s = compilerRegistry->size();
	console->info("\t[xacc::compiler] XACC has " + std::to_string(s) + " Compiler" + (s==1 ? "" : "s") + " available.");
	xacc::xaccFrameworkInitialized = true;
}

std::shared_ptr<Accelerator> getAccelerator(const std::string& name) {
	if (!xacc::xaccFrameworkInitialized) {
		XACCError("XACC not initialized before use. Please execute xacc::Initialize() before using API.");
	}
	auto acc = AcceleratorRegistry::instance()->create(name);
	if (acc) {
		return acc;
	} else {
		XACCError("Invalid Accelerator. Could not find " + name + " in Accelerator Registry.");
	}
}

//template<typename ... RuntimeArgs>
//std::function<void(std::shared_ptr<AcceleratorBuffer>, RuntimeArgs...)> createKernel(
//		const std::string& kernelName, std::shared_ptr<Accelerator> acc,
//		const std::string& src) {
//	xacc::Program p(acc, src);
//	return p.getKernel<RuntimeArgs...>(kernelName);
//}

/**
 * This method should be called by clients to
 * clean up and finalize the XACC framework. It should
 * be called after using the XACC API.
 */
void Finalize() {
	auto console = spdlog::get("console");
	console->info("[xacc] XACC Finalizing\n\tCleaning up Compiler Registry.");
	xacc::CompilerRegistry::instance()->destroy();
	xacc::xaccFrameworkInitialized = false;
}
}


#endif
