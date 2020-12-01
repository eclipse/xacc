/*******************************************************************************
 * Copyright (c) 2018 UT-Battelle, LLC.
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
#ifndef XACC_MITIQZNEDECORATOR_HPP_
#define XACC_MITIQZNEDECORATOR_HPP_

#include "AcceleratorDecorator.hpp"
#include <pybind11/embed.h>

namespace py = pybind11;

namespace xacc {

namespace python {

class MitiqZNE : public AcceleratorDecorator {
protected:
  std::unique_ptr<py::scoped_interpreter> guard;
  void * libpython_handle = nullptr;
  py::module xacc;
  py::module qiskit;
  py::module mitiq;
public:
  void initialize(const HeterogeneousMap &params = {}) override;

  void updateConfiguration(const HeterogeneousMap &config) override {
    decoratedAccelerator->updateConfiguration(config);
  }
  const std::vector<std::string> configurationKeys() override { return {}; }

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> function) override;
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   functions) override;

  const std::string name() const override { return "mitiq-zne"; }
  const std::string description() const override { return ""; }
  ~MitiqZNE();
};

} // namespace python
} // namespace xacc
#endif
