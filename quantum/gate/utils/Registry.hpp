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
#ifndef XACC_UTILS_REGISTRY_HPP_
#define XACC_UTILS_REGISTRY_HPP_

#include "Singleton.hpp"
#include "XACC.hpp"
#include "RuntimeOptions.hpp"
#include <map>
#include <iostream>
#include <boost/program_options.hpp>

using namespace boost::program_options;

namespace xacc {

/**
 * Registry is a Singleton that provides a
 * mapping of string ids to creation functions that
 * create and return the provided Registry template
 * parameter T.
 *
 * Clients can add new creation functions to be placed
 * in the map with a unique name key, and can request
 * that the Registry return a new created instance of
 * the template parameter T.
 *
 */
template<typename T, typename... TArgs>
class Registry : public Singleton<Registry<T, TArgs...>> {

public:

	using CreatorFunction = std::function<std::shared_ptr<T>(TArgs...)>;
	using CreatorFunctionPtr = std::shared_ptr<CreatorFunction>;

	/**
	 * Add a new creation function to the Registry, keyed
	 * on the provided string id.
	 *
	 * @param id The Id of the creation function
	 * @param f The object's creation function
	 * @return success Bool indicating if this creator was added successfully.
	 */
	bool add(const std::string& id,
			CreatorFunctionPtr f) {
		if (registry.find(id) != registry.end()) {
			XACCLogger::instance()->info(id + " already exists in Registry. Ignoring and retaining previous Registry entry");
			return true;
		}

		if (RuntimeOptions::instance()->exists("verbose-registry")) XACCLogger::instance()->info("Registry adding " + id);
		bool s = registry.emplace(std::make_pair(id, f)).second;
		if (s) {
			return true;
		}
		xacc::error("Could not add " + id + " to the Registry.");
		return false;
	}

	/**
	 * Add a new creation function to the Registry, keyed
	 * on the provided string id, and related
	 * command line options for the object to be created.
	 *
	 * @param id The Id of the creation function
	 * @param f The object's creation function
	 * @param options The objects command line options
	 * @return success Bool indicating if this creator was added successfully.
	 */
	bool add(const std::string& id,
			CreatorFunctionPtr f, std::shared_ptr<options_description> options) {
		if (registry.find(id) != registry.end()) {
			return true;
		}
		if (RuntimeOptions::instance()->exists("verbose-registry")) XACCLogger::instance()->info("Registry adding " + id);
		bool s = registry.emplace(std::make_pair(id, f)).second;
		bool s2 = registryOptions.insert(std::make_pair(id, std::move(options))).second;
		if (!s || ! s2) {
			xacc::error("Could not add " + id + " to the Registry.");
			return false;
		} else {
			return true;
		}
	}

	bool add(const std::string& id, CreatorFunctionPtr f,
			std::shared_ptr<options_description> options,
			std::function<bool(variables_map&)> optionsHandler) {
		if (registry.find(id) != registry.end()) {
			XACCLogger::instance()->info(
					id
							+ " already exists in Registry. Ignoring and retaining previous Registry entry");
			return true;
		}
		if (RuntimeOptions::instance()->exists("verbose-registry"))
			XACCLogger::instance()->info("Registry adding " + id);
		bool s = registry.emplace(std::make_pair(id, f)).second;
		bool s2 =
				registryOptions.insert(std::make_pair(id, std::move(options))).second;
		bool s3 = registryOptionHandlers.insert(std::make_pair(id, std::move(optionsHandler))).second;
		if (!s || !s2) {
			xacc::error("Could not add " + id + " to the Registry.");
			return false;
		} else {
			return true;
		}
	}

	/**
	 * Create an instance of T by using the creation
	 * function found at the given key string id.
	 *
	 * @param id The Id of the creation function
	 * @return object Shared Pointer for the created object.
	 */
	std::shared_ptr<T> create(const std::string& id, TArgs... args) {
		auto search = registry.find(id);
		if (search != registry.end()) {
			return registry[id]->operator()(args...);
		} else {
			xacc::error("Invalid Registry map id string - " + id);
			return std::make_shared<T>(args...);
		}
	}

	/**
	 * Return the keys from the registry map.
	 *
	 * @return ids The registered creator Ids
	 */
	std::vector<std::string> getRegisteredIds() {
		std::vector<std::string> keys;

		std::transform(registry.begin(), registry.end(),
				std::back_inserter(keys),
				std::bind(
						&std::map<std::string,
						CreatorFunctionPtr>::value_type::first,
						std::placeholders::_1));
		return keys;
	}


	std::vector<std::shared_ptr<options_description>> getRegisteredOptions() {
		std::vector<std::shared_ptr<options_description>> values;

		auto getSecond = [](const std::pair<std::string, std::shared_ptr<options_description>>& key_val) {
			return key_val.second;
		};
		std::transform(registryOptions.begin(), registryOptions.end(), std::back_inserter(values),
		               getSecond);

		return values;

	}

	bool handleOptions(variables_map& map) {
		for (auto& kv : registryOptionHandlers) {
			if (kv.second(map)) {
				return true;
			}
		}
		return false;
	}
	/**
	 * Return the number of creation functions
	 * this registry contains.
	 *
	 * @return size The number of creators in this Registry
	 */
	std::size_t size() {
		return registry.size();
	}

protected:

	/**
	 * Reference to the database of creation functions
	 * for classes of superclass type T.
	 */
	std::map<std::string, CreatorFunctionPtr> registry;

	std::map<std::string, std::shared_ptr<options_description>> registryOptions;

	std::map<std::string, std::function<bool(variables_map&)>> registryOptionHandlers;

};
}

#endif
