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
#ifndef XACC_UTILS_RUNTIMEOPTIONS_HPP_
#define XACC_UTILS_RUNTIMEOPTIONS_HPP_

#include "Singleton.hpp"
#include <map>

namespace xacc {

/**
 * The RuntimeOptions class is a Singleton mapping of string
 * keys to string values. It is used throughout XACC to
 * provide and share runtime options that are provided from
 * XACC users via the command line.
 */
class RuntimeOptions : public Singleton<RuntimeOptions>,
                       public std::map<std::string, std::string> {

public:
  /**
   * Convenience method to get whether the
   * give key exists in the RuntimeOptions.
   *
   * @param key The key to check exists
   */
  bool exists(const std::string &key) { return find(key) != end(); }
};
} // namespace xacc

#endif
