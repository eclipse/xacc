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
#ifndef XACC_SERVICE_SERVICEAPI_HPP_
#define XACC_SERVICE_SERVICEAPI_HPP_

#include "ServiceRegistry.hpp"
#include <memory>

namespace xacc {

extern std::shared_ptr<ServiceRegistry> serviceRegistry;
extern bool serviceAPIInitialized;

void ServiceAPI_Initialize(int argc, char **argv);
void ServiceAPI_Finalize();

void contributeService(const std::string name, ContributableService& service);
void contributeService(const std::string name, ContributableService&& service);

void addPluginSearchPath(const std::string path);

template <class Service>
std::shared_ptr<Service> getService(const std::string &serviceName, bool failIfNotFound = true) {
  if (!xacc::serviceAPIInitialized) {
    XACCLogger::instance()->error(
        "XACC not initialized before use. Please execute "
        "xacc::Initialize() before using API.");
  }
  auto service = serviceRegistry->getService<Service>(serviceName);
  if (!service && failIfNotFound) {
    XACCLogger::instance()->error("Invalid XACC Service. Could not find " +
                                  serviceName + " in Service Registry.");
  }
  return service;
}

template <typename Service> bool hasService(const std::string &serviceName) {
  if (!xacc::serviceAPIInitialized) {
    XACCLogger::instance()->error(
        "XACC not initialized before use. Please execute "
        "xacc::Initialize() before using API.");
  }
  return serviceRegistry->hasService<Service>(serviceName);
}


template <class Service>
std::shared_ptr<Service> getContributedService(const std::string &serviceName, bool failIfNotFound = true) {
  if (!xacc::serviceAPIInitialized) {
    XACCLogger::instance()->error(
        "XACC not initialized before use. Please execute "
        "xacc::Initialize() before using API.");
  }
  auto service = serviceRegistry->getContributedService<Service>(serviceName);
  if (!service && failIfNotFound) {
    XACCLogger::instance()->error("Invalid XACC Service. Could not find " +
                                  serviceName + " in Service Registry.");
  }
  return service;
}

template<typename Service>
bool hasContributedService(const std::string &serviceName) {
  if (!xacc::serviceAPIInitialized) {
    XACCLogger::instance()->error(
        "XACC not initialized before use. Please execute "
        "xacc::Initialize() before using API.");
  }
  return serviceRegistry->hasContributedService<Service>(serviceName);
}

template <typename ServiceInterface>
std::vector<std::string> getRegisteredIds() {
  return serviceRegistry->getRegisteredIds<ServiceInterface>();
}

template <typename ServiceInterface>
std::vector<std::shared_ptr<ServiceInterface>> getServices() {
  return serviceRegistry->getServices<ServiceInterface>();
}

std::vector<OptionPairs> getRegisteredOptions();
bool handleOptions(const std::map<std::string, std::string> &map);

std::string getRootPathString();

} // namespace xacc

#endif