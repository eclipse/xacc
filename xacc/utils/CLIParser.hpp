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
#include <boost/dll.hpp>
#include <boost/program_options.hpp>
#include "RuntimeOptions.hpp"
#include "xacc_config.hpp"

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

	/**
	 * Argc, number of arguments
	 */
	int argc;

	/**
	 * Argv, the command line arguments
	 */
	char** argv;

public:

	/**
	 * The constructor
	 */
	CLIParser(int arc, char** arv) : argc(arc), argv(arv) {}

	/**
	 * Parse the command line options. Provide a Boost options_description
	 * built up and provided by all available OptionsProviders. This
	 * method also loads all Compilers and Accelerators available
	 * in the XACC_INSTALL_DIR.
	 */
	void parse() {

		// Get a reference to the RuntimeOptions
		auto runtimeOptions = RuntimeOptions::instance();

		// Create a base options_description, we will add
		// to this with all OptionsProviders
		auto compilerOptions = std::make_shared<options_description>(
				"XACC Options");
		compilerOptions->add_options()("help", "Help Message")("compiler",
				value<std::string>()->default_value("scaffold"),
				"Indicate the compiler to be used.")("writeIR",
				value<std::string>(), "Persist generated IR to provided file name.")(
				"loadCompiler", value<std::string>(), "Load a XACC plugin")
				("loadAccelerator", value<std::string>(), "Load an XACC Accelerator");

		// Load all default Compilers and Accelerators,
		// ie those in XACC INSTALL DIR/lib
		boost::filesystem::path xaccPath(
				std::string(XACC_INSTALL_DIR) + std::string("/lib"));
		if (boost::filesystem::exists(xaccPath)) {
			boost::filesystem::directory_iterator end_itr;

			// cycle through the directory
			for (boost::filesystem::directory_iterator itr(xaccPath);
					itr != end_itr; ++itr) {
				auto p = itr->path();
				if (p.extension() == ".so") {
					namespace dll = boost::dll;
					dll::shared_library lib(p,
							dll::load_mode::append_decorations);
					if (lib.has("registerCompiler")) {
						typedef void (RegisterCompiler)();
						auto regFunc =
								boost::dll::import_alias<RegisterCompiler>(p,
										"registerCompiler",
										dll::load_mode::append_decorations);
						regFunc();
					}
					if (lib.has("registerAccelerator")) {
						typedef void (RegisterAccelerator)();
						auto regFunc = boost::dll::import_alias<
								RegisterAccelerator>(p, "registerAccelerator",
								dll::load_mode::append_decorations);
						regFunc();
					}
					if (lib.has("registerEmbeddingAlgorithm")) {
						typedef void (RegisterEmbeddingAlgorithm)();
						auto regFunc = boost::dll::import_alias<
								RegisterEmbeddingAlgorithm>(p,
								"registerEmbeddingAlgorithm",
								dll::load_mode::append_decorations);
						regFunc();
					}
				}
			}
		}

		// Add Accelerator specific options
		auto registeredIds = AcceleratorRegistry::instance()->getRegisteredIds();
		for (auto s : registeredIds) {
			{
				compilerOptions->add(
						*(AcceleratorRegistry::instance()->create(s)->getOptions().get()));
			}
		}

		// Add all Compiler specific options
		registeredIds = CompilerRegistry::instance()->getRegisteredIds();
		for (auto s : registeredIds) {
			{
				compilerOptions->add(
						*(CompilerRegistry::instance()->create(s)->getOptions().get()));
			}
		}

		// Parse the command line options
		variables_map compileParameters;
		store(parse_command_line(argc, argv, *compilerOptions.get()), compileParameters);
		if (compileParameters.count("help")) {
			std::cout << *compilerOptions.get() << "\n";
			exit(0);
		}

		// If the user provides a path to a compiler plugin,
		// then load it
		if (compileParameters.count("loadCompiler")) {
			auto loadPath = compileParameters["loadCompiler"].as<std::string>();
			boost::filesystem::path p(loadPath);
			namespace dll = boost::dll;
			dll::shared_library lib(p, dll::load_mode::append_decorations);
			if (lib.has("registerCompiler")) {
				typedef void (RegisterCompiler)();
				auto regFunc = boost::dll::import_alias<RegisterCompiler>(p,
						"registerCompiler", dll::load_mode::append_decorations);
				regFunc();
			}
			compileParameters.erase("loadCompiler");
		}

		// If the user provides a path to a accelerator plugin,
		// then load it
		if (compileParameters.count("loadAccelerator")) {
			auto loadPath = compileParameters["loadAccelerator"].as<std::string>();
			boost::filesystem::path p(loadPath);
			namespace dll = boost::dll;
			dll::shared_library lib(p, dll::load_mode::append_decorations);
			if (lib.has("registerAccelerator")) {
				typedef void (RegisterAccelerator)();
				auto regFunc = boost::dll::import_alias<RegisterAccelerator>(p,
						"registerAccelerator", dll::load_mode::append_decorations);
				regFunc();
			}
			compileParameters.erase("loadAccelerator");
		}

		// Add all other string options to the global runtime option
		for (auto it = compileParameters.begin(); it != compileParameters.end();
				++it) {
			runtimeOptions->insert(
					std::make_pair(it->first, it->second.as<std::string>()));
		}
	}

};

}

#endif
