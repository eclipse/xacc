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
#ifndef XACC_UTILS_SERVICEREGISTRY_HPP_
#define XACC_UTILS_SERVICEREGISTRY_HPP_

#include "Singleton.hpp"
#include "Utils.hpp"
#include "RuntimeOptions.hpp"
#include "Identifiable.hpp"
#include "OptionsProvider.hpp"

#include "xacc_config.hpp"
#include <boost/algorithm/string.hpp>

#include <cppmicroservices/FrameworkFactory.h>
#include <cppmicroservices/Framework.h>
#include <cppmicroservices/BundleContext.h>
#include <cppmicroservices/Bundle.h>
#include <cppmicroservices/BundleImport.h>

#include <map>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

using namespace boost::program_options;
using namespace cppmicroservices;

namespace xacc {

/**
 * The ServiceRegistry provides the plugin infrastructure for XACC.
 * It delegates to the CppMicroServices Framework and BundleContexts,
 * and provides methods for creating instances of XACC plugins or services.
 */
class ServiceRegistry : public Singleton<ServiceRegistry> {

protected:

	/**
	 * Reference to the CppMicroServices Framework instance
	 */
	Framework framework;

	/**
	 * The BundleContext instance, which provides
	 * service references.
	 */
	BundleContext context;

	/**
	 * The constructor
	 */
	ServiceRegistry() : framework(FrameworkFactory().NewFramework()) {
		framework = FrameworkFactory().NewFramework();

		try {
			// Initialize the framework, such that we can call
			// GetBundleContext() later.
			framework.Init();
		} catch (const std::exception& e) {
			XACCError(std::string(e.what()) + " - Could not initialize XACC Framework");
		}

		context = framework.GetBundleContext();
		if (!context) {
			XACCError("Invalid XACC Framework plugin context.");
		}

		boost::filesystem::path xaccPluginPath(
				std::string(XACC_INSTALL_DIR) + std::string("/lib/plugins"));
		if (boost::filesystem::exists(xaccPluginPath)) {
			boost::filesystem::recursive_directory_iterator dir(xaccPluginPath);
			for (auto&& i : dir) {
				if (!boost::filesystem::is_directory(i)) {
					boost::filesystem::path path = i.path();
					if (".dylib" == i.path().extension()
							|| ".so" == i.path().extension()
							|| ".a" == i.path().extension()) {
						context.InstallBundles(i.path().string());
					}
				}
			}
		} else {
			XACCInfo("There are no plugins. Install plugins to begin working with XACC.");
		}

		boost::filesystem::path xaccLibPath(
				std::string(XACC_INSTALL_DIR) + std::string("/lib"));
		boost::filesystem::directory_iterator end_itr;
		for (boost::filesystem::directory_iterator itr(xaccLibPath);
				itr != end_itr; ++itr) {
			auto p = itr->path();
			if ((p.extension() == ".dylib" || p.extension() == ".so"
					|| p.extension() == ".a")
					&& (p.filename().string() != "libxacc.dylib"
							&& p.filename().string() != "libxacc.so")
					&& !boost::contains(p.filename().string(),
							"libCppMicroServices")
					&& !boost::contains(p.filename().string(), "libus")) {
				context.InstallBundles(p.string());
			}
		}

		try {
			// Start the framework itself.
			framework.Start();

			// Our bundles depend on each other in the sense that the consumer
			// bundle expects a ServiceTime service in its activator Start()
			// function. This is done here for simplicity, but is actually
			// bad practice.
			auto bundles = context.GetBundles();
			for (auto b : bundles) {
				b.Start();
			}

		} catch (const std::exception& e) {
			XACCError("Failure to start XACC plugins. " + std::string(e.what()));
		}
	}

public:

	// Overriding here so we can have a custom constructor
	static ServiceRegistry* instance() {
		if (!instance_) {
			instance_ = new ServiceRegistry();
		}
		return instance_;
	}

	template<typename ServiceInterface>
	bool hasService(const std::string name) {
		auto allServiceRefs = context.GetServiceReferences<ServiceInterface>();
		for (auto s : allServiceRefs) {
			auto service = context.GetService(s);
			auto identifiable = std::dynamic_pointer_cast<xacc::Identifiable>(service);
			if (identifiable && identifiable->name() == name) {
				return true;
			}
		}
		return false;
	}

	template<typename ServiceInterface>
	std::shared_ptr<ServiceInterface> getService(const std::string name) {
		auto allServiceRefs = context.GetServiceReferences<ServiceInterface>();
		for (auto s : allServiceRefs) {
			auto service = context.GetService(s);
			auto identifiable = std::dynamic_pointer_cast<xacc::Identifiable>(service);
			if (identifiable && identifiable->name() == name) {
				return service;
			}
		}
		XACCError("Could not find service with name " + name + ". "
				"Perhaps the service is not Identifiable.");
	}

	template<typename ServiceInterface>
	std::vector<std::shared_ptr<ServiceInterface>> getServices() {
		std::vector<std::shared_ptr<ServiceInterface>> services;
		auto allServiceRefs = context.GetServiceReferences<ServiceInterface>();
		for (auto s : allServiceRefs) {
			services.push_back(context.GetService(s));
		}

		return services;
	}

	/**
	 * Return the keys from the registry map.
	 *
	 * @return ids The registered creator Ids
	 */
	template<typename ServiceInterface>
	std::vector<std::string> getRegisteredIds() {
		std::vector<std::string> ids;
		auto allServiceRefs = context.GetServiceReferences<ServiceInterface>();
		for (auto s : allServiceRefs) {
			auto service = context.GetService(s);
			auto identifiable = std::dynamic_pointer_cast<xacc::Identifiable>(
					service);
			if (identifiable) {
				ids.push_back(identifiable->name());
			}
		}
		return ids;
	}


	std::vector<std::shared_ptr<options_description>> getRegisteredOptions() {
		std::vector<std::shared_ptr<options_description>> values;
		// Get all OptionsProvider services and call getOptions
		auto optionProviders = getServices<xacc::OptionsProvider>();
		for (auto o : optionProviders) {
			values.push_back(o->getOptions());
		}
		return values;

	}

	bool handleOptions(variables_map& map) {
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

	void loadPlugin(const std::string path) {
		context.InstallBundles(path);
		auto b = context.GetBundles(path);
		b[0].Start();
	}

};
}

#endif
