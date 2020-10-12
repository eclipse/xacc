/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
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
#include "xacc_service.hpp"
#include "xacc_config.hpp"

#include "cxxopts.hpp"
#include "CLIParser.hpp"
using namespace cxxopts;

namespace xacc {
// class ServiceRegistry;
bool serviceAPIInitialized = false;
std::shared_ptr<ServiceRegistry> serviceRegistry =
    std::make_shared<ServiceRegistry>();

void addPluginSearchPath(const std::string path) {
    serviceRegistry->appendSearchPath(path);
}

void ServiceAPI_Initialize(int argc, char **argv) {
  if (!serviceAPIInitialized) {
    std::string rootPath = std::string(XACC_INSTALL_DIR);

    // Do a little preprocessing on the
    // command line args to see if we have
    // been given a custom internal plugin path

    options_description desc("tmp");
    desc.allow_unrecognised_options().add_options()("xacc-root-path", "",
                                                    value<std::string>());
    auto vm = desc.parse(argc, argv);

    if (vm.count("xacc-root-path")) {
      rootPath = vm["xacc-root-path"].as<std::string>();
    }

    try {
      serviceRegistry->initialize(rootPath);
    } catch (std::exception &e) {
      XACCLogger::instance()->error(
          "Failure initializing XACC Plugin Registry - " +
          std::string(e.what()));
    }

    serviceAPIInitialized = true;
  }
}

void ServiceAPI_Finalize() {
    if (serviceAPIInitialized) {
        serviceAPIInitialized = false;
        serviceRegistry->finalize();
    }
}

void contributeService(const std::string name, ContributableService& service) {
    serviceRegistry->contributeService(name, service);
}
void contributeService(const std::string name, ContributableService&& service) {
    contributeService(name, service);
}

std::vector<OptionPairs> getRegisteredOptions() {
  return serviceRegistry->getRegisteredOptions();
}
bool handleOptions(const std::map<std::string, std::string> &map) {
  return serviceRegistry->handleOptions(map);
}

std::string getRootPathString() { return serviceRegistry->getRootPathString(); }

} // namespace xacc