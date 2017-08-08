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
#ifndef XACC_UTILS_CLIPARSER_HPP_
#define XACC_UTILS_CLIPARSER_HPP_

#include <memory>
#include <string>
#include <boost/program_options.hpp>
#include "RuntimeOptions.hpp"
#include "xacc_config.hpp"
#include "ServiceRegistry.hpp"

using namespace boost::program_options;

namespace xacc {

/**
 * The role of the CLIParser is to parse all command line
 * options provided to an XACC-enabled program. It takes upon
 * construction to available argc and argv variables from the
 * command line, and parses them to fill the RuntimeOptions
 * singleton and load any XACC Compiler or Accelerator plugins.
 *
 * It also queries all available OptionProviders to display
 * all available options to the XACC user.
 */
class CLIParser {

protected:

	std::shared_ptr<options_description> xaccOptions;

public:

	/**
	 * The constructor
	 */
	CLIParser() :
			 xaccOptions(
					std::make_shared<options_description>("XACC Options")) {
		xaccOptions->add_options()
						("help", "Help Message")
						("compiler",value<std::string>()->default_value("scaffold"),"Indicate the compiler to be used.")
						("persist-ir",value<std::string>(), "Persist generated IR to provided file name.")
						("load", value<std::string>(), "Load a XACC plugin at the given path")
						("list-compilers", "List all available XACC Compilers")
						("list-accelerators", "List all available XACC Accelerators");
	}

	/**
	 */
	void parse(int argc, char** argv) {

		// Get a reference to the RuntimeOptions
		auto runtimeOptions = RuntimeOptions::instance();

		auto registeredOptions = ServiceRegistry::instance()->getRegisteredOptions();

		for (auto s : registeredOptions) {
			xaccOptions->add(*s.get());
		}

		// Parse the command line options
		variables_map clArgs;
		store(parse_command_line(argc, argv, *xaccOptions.get()), clArgs);
		if (clArgs.count("help")) {
			std::cout << *xaccOptions.get() << "\n";
			XACCInfo(
					"\n[xacc] XACC Finalizing\n[xacc::compiler] Cleaning up Compiler Registry."
							"\n[xacc::accelerator] Cleaning up Accelerator Registry.");
			exit(0);
		}

		// If the user provides a path to a compiler plugin,
		// then load it
		if (clArgs.count("load")) {
			auto loadPath = clArgs["load-compiler"].as<std::string>();
			ServiceRegistry::instance()->loadPlugin(loadPath);
		}

		bool listTypes = false;
		if (clArgs.count("list-compilers")) {
			auto ids = ServiceRegistry::instance()->getRegisteredIds<Compiler>();
			XACCInfo("Available XACC Compilers:");
			for (auto i : ids) {
				XACCInfo("\t" + i);
			}
			listTypes = true;
		}

		if (clArgs.count("list-accelerators")) {
			auto ids = ServiceRegistry::instance()->getRegisteredIds<Accelerator>();
			XACCInfo("Available XACC Accelerators:");
			for (auto i : ids) {
				XACCInfo("\t" + i);
			}
			listTypes = true;
		}

		if (listTypes) {
			XACCInfo(
					"\n[xacc] XACC Finalizing\n[xacc::compiler] Cleaning up Compiler Registry."
							"\n[xacc::accelerator] Cleaning up Accelerator Registry.");
			exit(0);
		}

		auto exitRequested = ServiceRegistry::instance()->handleOptions(clArgs);
		if (exitRequested) {
			XACCInfo(
					"\n[xacc] XACC Finalizing\n[xacc::compiler] Cleaning up Compiler Registry."
							"\n[xacc::accelerator] Cleaning up Accelerator Registry.");
			exit(0);
		}

		// Add all other string options to the global runtime option
		for (auto it = clArgs.begin(); it != clArgs.end();
				++it) {
			runtimeOptions->insert(
					std::make_pair(it->first, it->second.as<std::string>()));
		}
	}

	void addStringOption(const std::string key, const std::string description = "") {
		xaccOptions->add_options()(key.c_str(), value<std::string>(), description.c_str());
	}

};

}

#endif
