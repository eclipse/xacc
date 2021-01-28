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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/

#pragma once

#include "Circuit.hpp"
#include "IRProvider.hpp"
#include <Eigen/Dense>
#include <pybind11/embed.h>

namespace py = pybind11;

namespace xacc {
namespace circuits {
class PyQfactor : public xacc::quantum::Circuit {
protected:
  bool initialized = false;
  std::shared_ptr<py::scoped_interpreter> guard;
  void *libpython_handle = nullptr;

public:
  PyQfactor() : Circuit("qfactor") {}
  void initialize();
  bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
  const std::vector<std::string> requiredKeys() override;
  ~PyQfactor();
  DEFINE_CLONE(PyQfactor);
};
} // namespace circuits
} // namespace xacc
