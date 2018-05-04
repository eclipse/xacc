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
extern std::shared_ptr<ServiceRegistry> serviceRegistry;

extern int argc;
extern char** argv;

/**
 * This method should be called by
 * clients to initialize the XACC framework.
 * It should be called before using any of the
 * XACC API.
 */
void Initialize(int argc, char** argv);

/**
 * Return command line argc integer.
 *
 * @return argc The number of arguments on the command line
 */
int getArgc();

/**
 * Return the command line arguments
 *
 * @return argv The command line arguments
 */
char** getArgv();

/**
 * Initialize the framework with a vector of strings as arguments.
 *
 * @param argv Vector of string arguments
 */
void Initialize(std::vector<std::string> argv);

/**
 * Initialize the framework with no arguments
 */
void Initialize();

/**
 * Return if the framework has been initialized.
 *
 * @return initialized Bool indicating true if initialized
 */
bool isInitialized();

/**
 * Add a valid command line option
 *
 * @param optionName The name of the string option
 * @param optionDescription The description of the option
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
 * Add a set of options as a boost options_description object. This method
 * gives clients the most flexibility to provide custom options.
 *
 * @param options The command line options
 */
void addCommandLineOptions(std::shared_ptr<options_description> options);

/**
 * Provide a predicate function that determines
 * whether the XACCLogger should print logs.
 *
 * @param predicate Function with no arguments that returns bool
 */
void setGlobalLoggerPredicate(MessagePredicate predicate);

/**
 * Print an info log to the XACC Logger.
 *
 * @param msg The log to print
 * @param predicate Function with no arguments that returns bool
 */
void info(const std::string& msg, MessagePredicate predicate = std::function<bool(void)>([]() {return true;}));

/**
 * Print a warning log to the XACC Logger.
 *
 * @param msg The log to print
 * @param predicate Function with no arguments that returns bool
 */
void warning(const std::string& msg, MessagePredicate predicate = std::function<bool(void)>([]() {return true;}));

/**
 * Print an debug log to the XACC Logger.
 *
 * @param msg The log to print
 * @param predicate Function with no arguments that returns bool
 */
void debug(const std::string& msg, MessagePredicate predicate = std::function<bool(void)>([]() {return true;}));

/**
 * Print an error log to the XACC Logger.
 *
 * @param msg The log to print
 * @param predicate Function with no arguments that returns bool
 */
void error(const std::string& msg, MessagePredicate predicate = std::function<bool(void)>([]() {return true;}));

/**
 * Clear the options database
 */
void clearOptions();

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
void unsetOption(const std::string& optionKey);

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
 *
 * @param name The name of the Accelerator
 * @return accelerator The Accelerator
 */
std::shared_ptr<Accelerator> getAccelerator(const std::string& name);

/**
 * Get the Accelerator that is currently specified by the
 * 'accelerator' option key.
 *
 * @return accelerator The Accelerator
 */
std::shared_ptr<Accelerator> getAccelerator();

/**
 * Return true if the framework has an Accelerator
 * with the given name.
 *
 * @param name Accelerator name
 * @return exists Bool that's true if framework has given Accelerator
 */
bool hasAccelerator(const std::string& name);

/**
 * Return the Compiler with given name
 */
std::shared_ptr<Compiler> getCompiler(const std::string& name);

/**
 * Get the Compiler that is currently specified by the
 * 'compiler' option key.
 *
 * @return compiler The Compiler
 */
std::shared_ptr<Compiler> getCompiler();

/**
 * Return true if the framework has a Compiler
 * with the given name.
 *
 * @param name Compiler name
 * @return exists Bool that's true if framework has given Compiler
 */
bool hasCompiler(const std::string& name);

template<typename Service>
std::shared_ptr<Service> getService(const std::string& serviceName) {
	auto service = serviceRegistry->getService<Service>(
			serviceName);
	if (!service) {
		error(
				"Invalid XACC Service. Could not find " + serviceName
						+ " in Service Registry.");
	}
	return service;
}

template<typename Service>
bool hasService(const std::string& serviceName) {
	return serviceRegistry->hasService<Service>(
			serviceName);
}
template<typename ServiceInterface>
std::vector<std::string> getRegisteredIds() {
	return serviceRegistry->getRegisteredIds<ServiceInterface>();
}
template<typename ServiceInterface>
std::vector<std::shared_ptr<ServiceInterface>> getServices() {
	return serviceRegistry->getServices<ServiceInterface>();
}

std::shared_ptr<IRTransformation> getIRTransformation(const std::string& name);

const std::string translate(const std::string& original, const std::string& originalLanguageName,
		const std::string& newLanguageName, const std::string bufferName);

const std::string translateWithVisitor(const std::string& originalSource, const std::string& originalLanguage,
		const std::string& visitorMapping, const std::string& accelerator, const int kernel);

/**
 * This method should be called by clients to
 * clean up and finalize the XACC framework. It should
 * be called after using the XACC API.
 */
void Finalize();

}


#endif
