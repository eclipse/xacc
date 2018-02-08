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

extern int argc;
extern char** argv;

/**
 * This method should be called by
 * clients to initialize the XACC framework.
 * It should be called before using any of the
 * XACC API.
 */
void Initialize(int argc, char** argv);

int getArgc();
char** getArgv();

void Initialize(std::vector<std::string> argv);

void Initialize();

bool isInitialized();

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

void addCommandLineOptions(std::shared_ptr<options_description> options);

void setGlobalLoggerPredicate(MessagePredicate predicate);
void info(const std::string& msg, MessagePredicate predicate = std::function<bool(void)>([]() {return true;}));
void debug(const std::string& msg, MessagePredicate predicate = std::function<bool(void)>([]() {return true;}));
void error(const std::string& msg, MessagePredicate predicate = std::function<bool(void)>([]() {return true;}));

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
std::shared_ptr<Accelerator> getAccelerator();

bool hasAccelerator(const std::string& name);

/**
 * Return the Compiler with given name
 */
std::shared_ptr<Compiler> getCompiler(const std::string& name);
std::shared_ptr<Compiler> getCompiler();
bool hasCompiler(const std::string& name);

/*
 * Return the IRTransformation with given name
 */
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
