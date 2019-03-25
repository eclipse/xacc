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
#ifndef XACC_UTILS_OPTIONSPROVIDER_HPP_
#define XACC_UTILS_OPTIONSPROVIDER_HPP_

#include <map>
#include <vector>
#include <memory>

namespace xacc {

// Option name to description.
using OptionPairs = std::map<std::string, std::string>;

/**
 * The OptionsProvider interface enables derived subclasses
 * to provide a description of any and all command line options
 * that they can take to drive and control their execution and
 * behavior.
 */
class OptionsProvider {

public:
  /**
   * Return a Boost options_description instance that
   * describes the options available for this
   * derived subclass.
   */
  virtual OptionPairs getOptions() {return OptionPairs{};}

  /**
   * Given user-input command line options, perform
   * some operation. Returns true if runtime should exit,
   * false otherwise.
   *
   * @param map The mapping of options to values
   * @return exit True if exit, false otherwise
   */
  virtual bool
  handleOptions(const std::map<std::string, std::string> &arg_map) {return false;}

  /**
   * The destructor
   */
  virtual ~OptionsProvider() {}
};

} // namespace xacc
#endif
