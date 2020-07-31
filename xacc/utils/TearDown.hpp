/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 * Alexander J. McCaskey - initial API and implementation
 * Thien Nguyen - initial API and implementation
 *******************************************************************************/

#pragma once
#include <string>
// TearDown interface: all registered TearDown services
// will have its tearDown() method called once xacc is about to Finalize().
// Contributing services (e.g. plugins) can use this to clean up any global
// resources that they instantiated.
namespace xacc {
class TearDown {
public:
  virtual void tearDown() = 0;
  virtual std::string name() const { return ""; }
};
} // namespace xacc