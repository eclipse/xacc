/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
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
 *
 */
class ServiceRegistry : public Singleton<ServiceRegistry> {

protected:

	Framework framework;

	BundleContext context;

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
					if (".so" == i.path().extension() || ".a" == i.path().extension()) {
						context.InstallBundles(i.path().string());
					}
				}
			}

			boost::filesystem::path xaccLibPath(
					std::string(XACC_INSTALL_DIR) + std::string("/lib"));
			boost::filesystem::directory_iterator end_itr;
			for (boost::filesystem::directory_iterator itr(xaccLibPath);
					itr != end_itr; ++itr) {
				auto p = itr->path();
				if ((p.extension() == ".so" || p.extension() == ".a")
						&& (p.filename().string() != "libxacc.so")) {
					context.InstallBundles(p.string());
				}
			}
		} else {
			XACCError("Cannot initialize XACC plugins - there is no XACC install directory.");
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
			XACCError("Failure to start XACC plugins.");
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
