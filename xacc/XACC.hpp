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
#include "ServiceRegistry.hpp"

namespace xacc {

// Boolean indicating that framework has been initialized
extern bool xaccFrameworkInitialized;

// Reference to the command line parser
extern std::shared_ptr<CLIParser> xaccCLParser;

/**
 * This method should be called by
 * clients to initialize the XACC framework.
 * It should be called before using any of the
 * XACC API.
 */
void Initialize(int argc, char** argv);

/**
 * Add a valid command line option
 */
void addCommandLineOption(const std::string& optionName,
		const std::string& optionDescription = "");

/**
 * Add a set of options under the provided category.
 *
 * @param category The name of the category these options fall under
 * @param options The options, a map of option names to descriptions
 */
void addCommandLineOptions(const std::string& category, const std::map<std::string, std::string>& options);

/**
 * Return true if the option exists.
 *
 * @param optionKey The name of the option
 * @return exists True if it exists
 */
bool optionExists(const std::string& optionKey);

/**
 * Return the value of the options with the given name.
 * @param optionKey The name of the option
 * @return value The value of this option
 */
const std::string getOption(const std::string& optionKey);

/**
 * Set the value of the option with the given name.
 *
 * @param optionKey The name of the option
 * @param value The new option value.
 */
void setOption(const std::string& optionKey, const std::string& value);

/**
 * Set the Compiler to use.
 *
 * @param compilerName The name of the compiler
 */
void setCompiler(const std::string& compilerName);

/**
 * Set the Accelerator to use.
 *
 * @param acceleratorName The name of the accelerator
 */
void setAccelerator(const std::string& acceleratorName);

/**
 * Return the Accelerator with given name
 */
std::shared_ptr<Accelerator> getAccelerator(const std::string& name);

/**
 * Return the Compiler with given name
 */
std::shared_ptr<Compiler> getCompiler(const std::string& name);

/*
 * Return the IRTransformation with given name
 */
std::shared_ptr<IRTransformation> getIRTransformation(const std::string& name);

/**
 * This method should be called by clients to
 * clean up and finalize the XACC framework. It should
 * be called after using the XACC API.
 */
void Finalize();

}


#endif
