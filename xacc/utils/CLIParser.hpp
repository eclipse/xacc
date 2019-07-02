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
#ifndef XACC_UTILS_CLIPARSER_HPP_
#define XACC_UTILS_CLIPARSER_HPP_

#include <memory>
#include <string>
#include "OptionsProvider.hpp"

namespace cxxopts {
class Options;
}

namespace xacc {
// class ServiceRegistry;

using options_description = cxxopts::Options;
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
  std::vector<std::string> paths;
public:
  /**
   * The constructor
   */
  CLIParser();
  void parse(int argc, char **argv);//, ServiceRegistry *serviceRegistry);

  void addOptions(const std::map<std::string, std::string> &options);
  void addStringOption(const std::string key,
                       const std::string description = "");
  void addStringOptions(const std::string &category,
                        const std::map<std::string, std::string> &options);
  std::vector<std::string> getIncludePaths();
};

} // namespace xacc

#endif
