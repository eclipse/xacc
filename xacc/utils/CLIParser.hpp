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
						("accelerator",value<std::string>(),"Indicate the accelerator to be used.")
						("persist-ir",value<std::string>(), "Persist generated IR to provided file name.")
						("load", value<std::string>(), "Load a XACC plugin at the given path")
						("list-compilers", "List all available XACC Compilers")
						("list-accelerators", "List all available XACC Accelerators")
						("no-color", "Turn off colored logger output (blue for INFO, red for ERROR, etc.).")
						("use-cout", "Use std::cout for logs instead of SPDLOG Logger.")
						("queue-preamble", "Pass this option to xacc::Initialize() if you would "
								"like all startup messages to be queued until after a "
								"global logger predicate has been passed.");
	}

	void addOptionsDescription(std::shared_ptr<options_description> options) {
		xaccOptions->add(*options);
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
			XACCLogger::instance()->info(
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
			XACCLogger::instance()->info("Available XACC Compilers:");
			for (auto i : ids) {
				XACCLogger::instance()->info("\t" + i);
			}
			listTypes = true;
		}

		if (clArgs.count("list-accelerators")) {
			auto ids = ServiceRegistry::instance()->getRegisteredIds<Accelerator>();
			XACCLogger::instance()->info("Available XACC Accelerators:");
			for (auto i : ids) {
				XACCLogger::instance()->info("\t" + i);
			}
			listTypes = true;
		}

		if (listTypes) {
			XACCLogger::instance()->info(
					"\n[xacc] XACC Finalizing\n[xacc::compiler] Cleaning up Compiler Registry."
							"\n[xacc::accelerator] Cleaning up Accelerator Registry.");
			exit(0);
		}

		auto exitRequested = ServiceRegistry::instance()->handleOptions(clArgs);
		if (exitRequested) {
			XACCLogger::instance()->info(
					"\n[xacc] XACC Finalizing\n[xacc::compiler] Cleaning up Compiler Registry."
							"\n[xacc::accelerator] Cleaning up Accelerator Registry.");
			exit(0);
		}

		// Add all other string options to the global runtime option
		for (auto& kv : clArgs) {
			if (runtimeOptions->exists(kv.first)) {
				(*runtimeOptions)[kv.first] = kv.second.as<std::string>();
			} else {
				runtimeOptions->insert(
						std::make_pair(kv.first, kv.second.as<std::string>()));
			}
		}
	}

	void addStringOption(const std::string key, const std::string description = "") {
		xaccOptions->add_options()(key.c_str(), value<std::string>(), description.c_str());
	}

	void addStringOptions(const std::string& category, const std::map<std::string, std::string>& options) {
		auto desc = std::make_shared<options_description>(
						category);
		for (auto& kv : options) {
			desc->add_options()(kv.first.c_str(), value<std::string>(), kv.second.c_str());
		}
		xaccOptions->add(*desc.get());
	}
};

}

#endif
