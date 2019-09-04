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
#ifndef XACC_UTILS_PERSISTABLE_HPP_
#define XACC_UTILS_PERSISTABLE_HPP_

#include <string>

namespace xacc {

class Persistable {
public:
  virtual void persist(std::ostream& out) = 0;
  virtual void load(std::istream& in) = 0;
};

} // namespace xacc

#endif
