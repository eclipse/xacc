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
#include "XACC.hpp"

namespace xacc {

bool xaccFrameworkInitialized = false;
std::shared_ptr<CLIParser> xaccCLParser = std::make_shared<CLIParser>();
int argc = 0;
char** argv = NULL;

int getArgc() {
	return argc;
}
char** getArgv() {
	return argv;
}
void Initialize(std::vector<std::string> argv) {
	XACCInfo("Initializing the XACC Framework");
	std::vector<char*> cstrs;
	argv.insert(argv.begin(),"appExec");
	for (auto& s : argv) {
		cstrs.push_back(&s.front());
	}
	Initialize(argv.size(), cstrs.data());
}

void Initialize() {
	Initialize(std::vector<std::string>{});
}

void Initialize(int arc, char** arv) {

	argc = arc;
	argv = arv;

	// Create the iniitial xacc-console
	std::shared_ptr<spdlog::logger> tmpInitConsole;
	if (!spdlog::get("xacc-console")) {
		tmpInitConsole = spdlog::stdout_logger_mt("xacc-console");
	} else {
		tmpInitConsole = spdlog::get("xacc-console");
	}

	XACCInfo("[xacc] Initializing XACC Framework");

	// Get reference to the service registry
	auto serviceRegistry = xacc::ServiceRegistry::instance();

	// Parse any user-supplied command line options
	xaccCLParser->parse(argc, argv);

	// Check that we have some
	auto s = serviceRegistry->getServices<Compiler>().size();
	auto a = serviceRegistry->getServices<Accelerator>().size();

	XACCInfo(
			"[xacc::compiler] XACC has " + std::to_string(s) + " Compiler"
					+ ((s == 1 || s == 0) ? "" : "s") + " available.");
	XACCInfo(
			"[xacc::accelerator] XACC has " + std::to_string(a) + " Accelerator"
					+ ((s == 0 || s == 1) ? "" : "s") + " available.");

	// We're good if we make it here, so indicate that we've been
	// initialized
	xacc::xaccFrameworkInitialized = true;
}

void addCommandLineOption(const std::string& optionName,
		const std::string& optionDescription) {
	xaccCLParser->addStringOption(optionName, optionDescription);
}

void addCommandLineOptions(const std::string& category, const std::map<std::string, std::string>& options) {
	xaccCLParser->addStringOptions(category, options);
}

bool optionExists(const std::string& optionKey) {
	return RuntimeOptions::instance()->exists(optionKey);
}

const std::string getOption(const std::string& optionKey) {
	if (!optionExists(optionKey)) {
		XACCError("Invalid runtime option - " + optionKey);
	}
	return (*RuntimeOptions::instance())[optionKey];
}

void setOption(const std::string& optionKey, const std::string& value) {
	if (optionExists(optionKey)) {
		(*RuntimeOptions::instance())[optionKey] = value;
	} else {
		RuntimeOptions::instance()->insert(std::make_pair(optionKey, value));
	}
}

void setCompiler(const std::string& compilerName) {
	setOption("compiler", compilerName);
}
void setAccelerator(const std::string& acceleratorName) {
	setOption("accelerator", acceleratorName);
}


std::shared_ptr<Accelerator> getAccelerator() {
	if (!xacc::xaccFrameworkInitialized) {
		XACCError(
				"XACC not initialized before use. Please execute "
				"xacc::Initialize() before using API.");
	}
	if (!optionExists("accelerator")) {
		XACCError("Invalid use of XACC API. getAccelerator() with no string argument "
				"requires that you set --accelerator at the command line.");
	}
	auto acc = ServiceRegistry::instance()->getService<Accelerator>(getOption("accelerator"));
	if (acc) {
		acc->initialize();
		return acc;
	} else {
		XACCError(
				"Invalid Accelerator. Could not find " + getOption("accelerator")
						+ " in Accelerator Registry.");
	}
}

std::shared_ptr<Accelerator> getAccelerator(const std::string& name) {
	if (!xacc::xaccFrameworkInitialized) {
		XACCError(
				"XACC not initialized before use. Please execute "
				"xacc::Initialize() before using API.");
	}
	auto acc = ServiceRegistry::instance()->getService<Accelerator>(name);
	if (acc) {
		acc->initialize();
		return acc;
	} else {
		XACCError(
				"Invalid Accelerator. Could not find " + name
						+ " in Accelerator Registry.");
	}
}

std::shared_ptr<Compiler> getCompiler(const std::string& name) {
	if (!xacc::xaccFrameworkInitialized) {
		XACCError(
				"XACC not initialized before use. Please execute "
				"xacc::Initialize() before using API.");
	}
	auto c = ServiceRegistry::instance()->getService<Compiler>(name);
	if (c) {
		return c;
	} else {
		XACCError(
				"Invalid Compiler. Could not find " + name
						+ " in Service Registry.");
	}
}
std::shared_ptr<Compiler> getCompiler() {
	if (!xacc::xaccFrameworkInitialized) {
		XACCError(
				"XACC not initialized before use. Please execute "
				"xacc::Initialize() before using API.");
	}
	auto options = RuntimeOptions::instance();
	if (!optionExists("compiler")) {
		XACCError("Invalid use of XACC API. getCompiler() with no string argument "
				"requires that you set --compiler at the command line.");
	}
	auto compiler = ServiceRegistry::instance()->getService<Compiler>(getOption("compiler"));
	if (compiler) {
		return compiler;
	} else {
		XACCError(
				"Invalid Compiler. Could not find " + (*options)["compiler"]
						+ " in Compiler Registry.");
	}
}

std::shared_ptr<IRTransformation> getIRTransformations(
		const std::string& name) {
	if (!xacc::xaccFrameworkInitialized) {
		XACCError(
				"XACC not initialized before use. Please execute "
				"xacc::Initialize() before using API.");
	}
	auto t = ServiceRegistry::instance()->getService<IRTransformation>(name);
	if (t) {
		return t;
	} else {
		XACCError(
				"Invalid IRTransformation. Could not find " + name
						+ " in Service Registry.");
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
	xacc::ServiceRegistry::instance()->destroy();
	xacc::xaccFrameworkInitialized = false;
}
}

