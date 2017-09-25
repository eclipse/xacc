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
#ifndef XACC_UTILS_OPTIONSPROVIDER_HPP_
#define XACC_UTILS_OPTIONSPROVIDER_HPP_

#include <memory>
#include <boost/program_options.hpp>

using namespace boost::program_options;

namespace xacc {

/**
 * The OptionsProvider interface enables derived subclasses
 * to provide a description of any and all command line options
 * that they can take to drive and control their execution and
 * behavior.
 */
class __attribute__((visibility("default"))) OptionsProvider {

public:

	/**
	 * Return a Boost options_description instance that
	 * describes the options available for this
	 * derived subclass.
	 */
	virtual std::shared_ptr<options_description> getOptions() = 0;

	/**
	 * Given user-input command line options, perform
	 * some operation. Returns true if runtime should exit,
	 * false otherwise.
	 *
	 * @param map The mapping of options to values
	 * @return exit True if exit, false otherwise
	 */
	virtual bool handleOptions(variables_map& map) = 0;

	/**
	 * The destructor
	 */
	virtual ~OptionsProvider() {}

};

}
#endif
