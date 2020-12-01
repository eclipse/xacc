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
#pragma once

#include <type_traits>
#include <utility>

#include "Optimizer.hpp"
#include <pybind11/embed.h>

namespace py = pybind11;

namespace xacc {

class ScikitQuantOptimizer : public Optimizer {
protected: 
  void initialize();
  std::unique_ptr<py::scoped_interpreter> guard;
  void * libpython_handle = nullptr;
  bool initialized = false;
  
public:
  OptResult optimize(OptFunction &function) override;
  const bool isGradientBased() const override;
  virtual const std::string get_algorithm() const;

  const std::string name() const override { return "skquant"; }
  const std::string description() const override { return ""; }
  ~ScikitQuantOptimizer();
};
} // namespace xacc
