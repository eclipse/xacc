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
#include <memory>
#include <pybind11/complex.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/eigen.h>
#include <pybind11/iostream.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>

#include "xacc.hpp"
#include "AcceleratorDecorator.hpp"
#include "NoiseModel.hpp"

namespace py = pybind11;
using namespace xacc;
using Connectivity = std::vector<std::pair<int, int>>;

class PyAccelerator : public xacc::Accelerator {
public:
  /* Inherit the constructors */
  using Accelerator::Accelerator;

  const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::Accelerator, name);
  }
  const std::string description() const override { return ""; }

  HeterogeneousMap getProperties() override {
    PYBIND11_OVERLOAD(HeterogeneousMap, xacc::Accelerator, getProperties);
  }

  Connectivity getConnectivity() override {
    PYBIND11_OVERLOAD(Connectivity, xacc::Accelerator, getConnectivity);
  }

  void initialize(const HeterogeneousMap &params = {}) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Accelerator, initialize, params);
  }

  void execute(std::shared_ptr<xacc::AcceleratorBuffer> buf,
               std::shared_ptr<xacc::CompositeInstruction> f) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Accelerator, execute, buf, f);
  }

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   functions) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Accelerator, execute, buffer, functions);
  }
  void updateConfiguration(const HeterogeneousMap &config) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::Accelerator, updateConfiguration,
                           config);
  }
  const std::vector<std::string> configurationKeys() override {
    PYBIND11_OVERLOAD_PURE(std::vector<std::string>, xacc::Accelerator,
                           configurationKeys)
  }

  void contributeInstructions(const std::string &custom_json_config) override {
    PYBIND11_OVERLOAD(void, xacc::Accelerator, contributeInstructions,
                      custom_json_config);
  }
};

class PyAcceleratorDecorator : public xacc::AcceleratorDecorator {
public:
  /* Inherit the constructors */
  using AcceleratorDecorator::AcceleratorDecorator;
  const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::AcceleratorDecorator, name);
  }
  const std::string description() const override { return ""; }
  void initialize(const HeterogeneousMap &params = {}) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::AcceleratorDecorator, initialize,
                           params);
  }
  void execute(std::shared_ptr<xacc::AcceleratorBuffer> buf,
               std::shared_ptr<xacc::CompositeInstruction> f) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::AcceleratorDecorator, execute, buf, f);
  }

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   functions) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::AcceleratorDecorator, execute, buffer,
                           functions);
  }

  void updateConfiguration(const HeterogeneousMap &config) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::AcceleratorDecorator,
                           updateConfiguration, config);
  }
  const std::vector<std::string> configurationKeys() override {
    PYBIND11_OVERLOAD_PURE(std::vector<std::string>, xacc::AcceleratorDecorator,
                           configurationKeys)
  }
};

// Forward declare
namespace quantum {
class Gate;
}
class PyNoiseModel : public xacc::NoiseModel {
public:
  using NoiseModel::NoiseModel;
  const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::NoiseModel, name);
  }
  const std::string description() const override { return ""; }
  void initialize(const HeterogeneousMap &params = {}) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::NoiseModel, initialize, params);
  }
  std::string toJson() const override {
    PYBIND11_OVERLOAD_PURE(std::string, xacc::NoiseModel, toJson);
  }

  std::vector<RoErrors> readoutErrors() const override {
    PYBIND11_OVERLOAD_PURE(std::vector<RoErrors>, xacc::NoiseModel,
                           readoutErrors);
  }
  RoErrors readoutError(size_t qubitIdx) const override {
    PYBIND11_OVERLOAD_PURE(RoErrors, xacc::NoiseModel, readoutError, qubitIdx);
  }

  double gateErrorProb(xacc::quantum::Gate &gate) const override { return 0.0; }
  size_t nQubits() const override {
    PYBIND11_OVERLOAD_PURE(size_t, xacc::NoiseModel, nQubits);
  }
  std::vector<double> averageSingleQubitGateFidelity() const override {
    return {};
  }
  std::vector<std::tuple<size_t, size_t, double>>
  averageTwoQubitGateFidelity() const override {
    return {};
  }
};

void bind_accelerator(py::module &m);