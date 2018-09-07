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
#ifndef XACC_UTILS_IDENTIFIABLE_HPP_
#define XACC_UTILS_IDENTIFIABLE_HPP_

#include <string>

namespace xacc {

/**
 * This interface serves as the base for all interfaces
 * and classes that have a unique name and description.
 *
 */
class Identifiable {
public:
  /**
   * Return the name of this instance.
   *
   * @return name The string name
   */
  virtual const std::string name() const = 0;

  /**
   * Return the description of this instance
   * @return description The description of this object.
   */
  virtual const std::string description() const = 0;

  /**
   * The destructor
   */
  virtual ~Identifiable() {}
};

} // namespace xacc

#endif
