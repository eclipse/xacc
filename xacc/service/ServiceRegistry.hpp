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
#ifndef XACC_UTILS_SERVICEREGISTRY_HPP_
#define XACC_UTILS_SERVICEREGISTRY_HPP_

#include "Singleton.hpp"
#include "Utils.hpp"
#include "RuntimeOptions.hpp"
#include "Identifiable.hpp"
#include "OptionsProvider.hpp"
#include "Cloneable.hpp"

#include "Algorithm.hpp"
#include "Compiler.hpp"
#include "Observable.hpp"
#include "Optimizer.hpp"
#include "IRTransformation.hpp"
#include "AcceleratorDecorator.hpp"
#include "AlgorithmGradientStrategy.hpp"

#include <cppmicroservices/FrameworkFactory.h>
#include <cppmicroservices/Framework.h>
#include <cppmicroservices/BundleContext.h>
#include <cppmicroservices/Bundle.h>
#include <cppmicroservices/BundleImport.h>

#include <map>
#include <dirent.h>
#include <memory>

using namespace cppmicroservices;

namespace xacc {

using ContributableService =
    Variant<std::shared_ptr<Instruction>, std::shared_ptr<Accelerator>,
            std::shared_ptr<Compiler>, std::shared_ptr<Optimizer>, std::shared_ptr<Algorithm>,
            std::shared_ptr<IRTransformation>, std::shared_ptr<Observable>,
            std::shared_ptr<AcceleratorDecorator>, std::shared_ptr<AlgorithmGradientStrategy>>;

class ServiceRegistry {

protected:
  Framework framework;
  BundleContext context;

  bool initialized = false;
  std::string rootPathStr = "";

  std::map<std::string, ContributableService> runtimeContributed;
 
  std::vector<std::string> extra_search_paths;

public:
  ServiceRegistry() : framework(FrameworkFactory().NewFramework()) {}
  const std::string getRootPathString() { return rootPathStr; }

  void initialize(const std::string rootPath);
  void finalize() {
    auto bundles = context.GetBundles();
    for (auto b : bundles) {
      if (b.GetSymbolicName() != "system") {
        b.Stop();
      }
    }
  }

  void appendSearchPath(const std::string path) {
      extra_search_paths.push_back(path);
  }

  void contributeService(const std::string name,
                         ContributableService &service) {
    if (runtimeContributed.count(name)) {
      XACCLogger::instance()->error("Service already contributed.");
    }
    runtimeContributed.insert({name, service});
  }

  template <typename ServiceInterface> bool hasService(const std::string name) {
    auto allServiceRefs = context.GetServiceReferences<ServiceInterface>();
    for (auto s : allServiceRefs) {
      auto service = context.GetService(s);
      auto identifiable =
          std::dynamic_pointer_cast<xacc::Identifiable>(service);
      if (identifiable && identifiable->name() == name) {
        return true;
      }
    }

    return false;
  }

  template <typename ServiceInterface>
  std::shared_ptr<ServiceInterface> getService(const std::string name) {
    std::shared_ptr<ServiceInterface> ret;
    auto allServiceRefs = context.GetServiceReferences<ServiceInterface>();
    for (auto s : allServiceRefs) {
      auto service = context.GetService(s);
      auto identifiable =
          std::dynamic_pointer_cast<xacc::Identifiable>(service);
      if (identifiable && identifiable->name() == name) {
        auto checkCloneable =
            std::dynamic_pointer_cast<xacc::Cloneable<ServiceInterface>>(
                service);
        if (checkCloneable && checkCloneable->shouldClone()) {
          ret = checkCloneable->clone();
        } else {
          ret = service;
        }
      }
    }

    return ret;
  }

  template <typename ServiceInterface>
  std::shared_ptr<ServiceInterface>
  getContributedService(const std::string name) {
    std::shared_ptr<ServiceInterface> ret;

    if (runtimeContributed.count(name)) {
      ret = runtimeContributed[name].as<std::shared_ptr<ServiceInterface>>();
      auto checkCloneable =
          std::dynamic_pointer_cast<xacc::Cloneable<ServiceInterface>>(ret);
      if (checkCloneable && checkCloneable->shouldClone()) {
        ret = checkCloneable->clone();
      }
    }

    return ret;
  }

  template <typename Service>
  bool hasContributedService(const std::string name) {
    if (runtimeContributed.count(name)) {
      try {
        auto tmp =
            runtimeContributed[name].as_no_error<std::shared_ptr<Service>>();
      } catch (std::exception &e) {
        return false;
      }
      return true;
    }
    return false;
  }

  template <typename ServiceInterface>
  std::vector<std::shared_ptr<ServiceInterface>> getServices() {
    std::vector<std::shared_ptr<ServiceInterface>> services;
    auto allServiceRefs = context.GetServiceReferences<ServiceInterface>();
    for (auto s : allServiceRefs) {
      services.push_back(context.GetService(s));
    }

    return services;
  }

  template <typename ServiceInterface>
  std::vector<std::string> getRegisteredIds() {
    std::vector<std::string> ids;
    auto allServiceRefs = context.GetServiceReferences<ServiceInterface>();
    for (auto s : allServiceRefs) {
      auto service = context.GetService(s);
      auto identifiable =
          std::dynamic_pointer_cast<xacc::Identifiable>(service);
      if (identifiable) {
        ids.push_back(identifiable->name());
      }
    }
    return ids;
  }

  std::vector<OptionPairs> getRegisteredOptions() {
    std::vector<OptionPairs> values;
    // Get all OptionsProvider services and call getOptions
    auto optionProviders = getServices<xacc::OptionsProvider>();
    for (auto o : optionProviders) {
      values.push_back(o->getOptions());
    }
    return values;
  }

  bool handleOptions(const std::map<std::string, std::string> &map) {
    auto returnedTrue = false;
    // Get all OptionProvider services and call handleOptions
    auto optionProviders = getServices<xacc::OptionsProvider>();
    for (auto o : optionProviders) {
      if (o->handleOptions(map)) {
        returnedTrue = true;
      }
    }
    return returnedTrue;
  }
};
} // namespace xacc

#endif
