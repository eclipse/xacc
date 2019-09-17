/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include <string>

#include "CLIParser.hpp"
#include "RuntimeOptions.hpp"
#include "xacc_config.hpp"
#include "Compiler.hpp"
#include "Accelerator.hpp"

#include "cxxopts.hpp"

#include "xacc_service.hpp"

using namespace cxxopts;

namespace xacc {

CLIParser::CLIParser()
    : xaccOptions(std::make_shared<options_description>("XACC Help")) {
}

void CLIParser::addOptions(const std::map<std::string, std::string> &options) {
  for (auto &kv : options) {
    xaccOptions->add_options()(kv.first, kv.second, value<std::string>());
  }
}

/**
 */
void CLIParser::parse(int argc,
                      char **argv) { // ServiceRegistry *serviceRegistry) {

  // Get a reference to the RuntimeOptions
  auto runtimeOptions = RuntimeOptions::instance();

  if (xacc::serviceAPIInitialized) {
    auto registeredOptions = xacc::getRegisteredOptions();

    for (auto s : registeredOptions) {
      addOptions(s);
    }
  }

  xaccOptions->allow_unrecognised_options().add_options()("h,help", "Help message")(
      "compiler", "Indicate the compiler to be used", value<std::string>())(
      "a,accelerator", "Indicate the accelerator to be used.",
      value<std::string>())("logger-name", "The name of the spd logger",
                            value<std::string>())
      ("I,include","", value<std::vector<std::string>>())
      (
      "list-compilers", "List all available XACC Compilers")(
      "list-accelerators", "List all available XACC Accelerators")(
      "no-color",
      "Turn off colored logger output (blue for INFO, red for ERROR, etc.).")(
      "use-cout", "Use std::cout for logs instead of SPDLOG Logger.")(
      "queue-preamble", "Pass this option to xacc::Initialize() if you would "
                        "like all startup messages to be queued until after a "
                        "global logger predicate has been passed.");

  // Parse the command line options
  auto clArgs = xaccOptions->parse(argc, argv);
  if (clArgs.count("help")) {
    XACCLogger::instance()->dumpQueue();
    // std::cout << *xaccOptions->get() << "\n";
    XACCLogger::instance()->info(
        "\n[xacc] XACC Finalizing\n[xacc::compiler] Cleaning up Compiler "
        "Registry."
        "\n[xacc::accelerator] Cleaning up Accelerator Registry.");
    exit(0);
  }

  bool listTypes = false;
  if (clArgs.count("list-compilers") && serviceRegistry) {
    auto ids = xacc::getRegisteredIds<Compiler>();
    XACCLogger::instance()->enqueueLog("\nAvailable XACC Compilers:");
    for (auto i : ids) {
      XACCLogger::instance()->enqueueLog("\t" + i);
    }
    XACCLogger::instance()->enqueueLog("\n");
    listTypes = true;
  }

  if (clArgs.count("list-accelerators") && serviceRegistry) {
    auto ids = xacc::getRegisteredIds<Accelerator>();
    XACCLogger::instance()->enqueueLog("\nAvailable XACC Accelerators:");
    for (auto i : ids) {
      XACCLogger::instance()->enqueueLog("\t" + i);
    }
    XACCLogger::instance()->enqueueLog("\n");
    listTypes = true;
  }

  if (listTypes) {
    XACCLogger::instance()->dumpQueue();
    XACCLogger::instance()->info(
        "\n[xacc] XACC Finalizing\n[xacc::compiler] Cleaning up Compiler "
        "Registry."
        "\n[xacc::accelerator] Cleaning up Accelerator Registry.");
    exit(0);
  }

  if (clArgs.count("include")) {
    paths = clArgs["include"].as<std::vector<std::string>>();
  }

  if (xacc::serviceAPIInitialized) {
    auto kvargs = clArgs.arguments();
    std::map<std::string, std::string> givenopts;
    for (auto &kv : kvargs) {
      givenopts.insert({kv.key(), kv.value()});
    }

    auto exitRequested = xacc::handleOptions(givenopts);
    if (exitRequested) {
      XACCLogger::instance()->dumpQueue();
      XACCLogger::instance()->info(
          "\n[xacc] XACC Finalizing\n[xacc::compiler] Cleaning up Compiler "
          "Registry."
          "\n[xacc::accelerator] Cleaning up Accelerator Registry.");
      exit(0);
    }
  }

  // Add all other string options to the global runtime option
  for (auto &kv : clArgs.arguments()) {
    if (runtimeOptions->exists(kv.key())) {
      (*runtimeOptions)[kv.key()] = kv.value();
    } else {
      runtimeOptions->insert(std::make_pair(kv.key(), kv.value()));
    }
  }
}

std::vector<std::string> CLIParser::getIncludePaths() {
    return paths;
}

void CLIParser::addStringOption(const std::string key,
                                const std::string description) {
  xaccOptions->add_options()(key, description, value<std::string>());
}

void CLIParser::addStringOptions(
    const std::string &category,
    const std::map<std::string, std::string> &options) {
  auto desc = xaccOptions->add_options(category);
  // std::make_shared<options_description>(category);
  for (auto &kv : options) {
    desc(kv.first, kv.second, value<std::string>());
  }
}

} // namespace xacc
