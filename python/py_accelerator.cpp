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
#include "py_accelerator.hpp"
#include "AcceleratorBuffer.hpp"
#include "AcceleratorDecorator.hpp"
#include "py_heterogeneous_map.hpp"
#include "xacc_service.hpp"

void bind_accelerator(py::module &m) {
  // Expose the Accelerator
  py::class_<xacc::Accelerator, std::shared_ptr<xacc::Accelerator>,
             PyAccelerator>
      acc(m, "Accelerator",
          "Accelerator wraps the XACC C++ Accelerator class "
          "and provides a mechanism for creating buffers of qubits. Execution "
          "is handled by the XACC Kernels.");
  acc.def(py::init<>())
      .def("name", &xacc::Accelerator::name,
           "Return the name of this Accelerator.")
      .def("getProperties", &xacc::Accelerator::getProperties, "")
      .def("contributeInstructions", &xacc::Accelerator::contributeInstructions,
           py::arg("custom_json_config") = std::string(""), "")
      .def("execute",
           (void (xacc::Accelerator::*)(
               std::shared_ptr<AcceleratorBuffer>,
               const std::shared_ptr<CompositeInstruction>)) &
               xacc::Accelerator::execute,
           "Execute the Function with the given AcceleratorBuffer.")
      .def(
          "execute",
          [](xacc::Accelerator &qpu, std::shared_ptr<AcceleratorBuffer> buffer,
             py::object &circuit) {
            auto mapper = [&](py::object &functor, const char *compiler_name,
                              bool exec = true) {
              auto src = functor().cast<std::string>();
              auto compiler = xacc::getCompiler(compiler_name);
              auto composite = compiler->compile(src)->getComposites()[0];
              if (exec)
                qpu.execute(buffer, composite);

              return composite;
            };
            if (py::isinstance<py::list>(circuit)) {
              std::vector<std::shared_ptr<CompositeInstruction>> programs;
              auto circuit_list = py::cast<py::list>(circuit);
              for (int i = 0; i < circuit_list.size(); i++) {
                auto _circ = circuit_list[i];
                if (py::hasattr(_circ, "qasm")) {
                  py::object functor = _circ.attr("qasm");
                  auto composite = mapper(functor, "staq", false);
                  programs.push_back(composite);
                } else if (py::hasattr(_circ, "out")) {
                  // this is probably a pyquil program
                  py::object functor = _circ.attr("out");
                  auto composite = mapper(functor, "quilc");
                  programs.push_back(composite);
                } else {
                  // Assume this is a py::list of XACC Composite.
                  // Note: This overload seems to be prioritized over the
                  // std::vector<std::shared_ptr<CompositeInstruction>>
                  // overload.
                  std::shared_ptr<CompositeInstruction> composite =
                      _circ.cast<std::shared_ptr<CompositeInstruction>>();
                  programs.push_back(composite);
                }
              }

              qpu.execute(buffer, programs);
              return;
            } else {
              if (py::hasattr(circuit, "qasm")) {
                // this is a qiskit circuit
                py::object functor = circuit.attr("qasm");
                mapper(functor, "staq");
                return;
              } else if (py::hasattr(circuit, "out")) {
                // this is probably a pyquil program
                py::object functor = circuit.attr("out");
                mapper(functor, "quilc");
                return;
              }

              xacc::error("\n\nXACC does not understand the provided pythonic "
                          "circuit type.\n"
                          "Make sure you pass an xacc::CompositeInstruction, "
                          "qiskit.QuantumCircuit, or pyquil.Program.");
              return;
            }
          },
          "")
      .def("execute",
           (void (xacc::Accelerator::*)(
               std::shared_ptr<AcceleratorBuffer>,
               const std::vector<std::shared_ptr<CompositeInstruction>>)) &
               xacc::Accelerator::execute,
           "Execute the Function with the given AcceleratorBuffer.")
      .def("initialize", &xacc::Accelerator::initialize, "")
      .def("defaultPlacementTransformation",
           &xacc::Accelerator::defaultPlacementTransformation, "")
      .def("updateConfiguration",
           (void (xacc::Accelerator::*)(const HeterogeneousMap &)) &
               xacc::Accelerator::updateConfiguration,
           "")
      .def("getConnectivity", &xacc::Accelerator::getConnectivity, "")
      .def("configurationKeys", &xacc::Accelerator::configurationKeys, "")
      .def("contributeInstructions", &xacc::Accelerator::contributeInstructions,
           "");

  py::class_<xacc::AcceleratorDecorator, xacc::Accelerator,
             std::shared_ptr<xacc::AcceleratorDecorator>,
             PyAcceleratorDecorator>
      accd(m, "AcceleratorDecorator", "");
  accd.def(py::init<>());
  accd.def("setDecorated", &xacc::AcceleratorDecorator::setDecorated, "");
  accd.def_property("decoratedAccelerator",
                    &xacc::AcceleratorDecorator::getDecorated,
                    &xacc::AcceleratorDecorator::setDecorated);

  // Expose the AcceleratorBuffer
  py::class_<xacc::AcceleratorBuffer, std::shared_ptr<xacc::AcceleratorBuffer>>(
      m, "AcceleratorBuffer",
      "The AcceleratorBuffer models a register of qubits.")
      .def(py::init<const std::string &, const int>())
      .def("getExpectationValueZ",
           &xacc::AcceleratorBuffer::getExpectationValueZ,
           "Return the expectation value with respect to the Z operator.")
      .def("resetBuffer", &xacc::AcceleratorBuffer::resetBuffer,
           "Reset this buffer for use in another computation.")
      .def("size", &xacc::AcceleratorBuffer::size, "")
      .def("setName", &xacc::AcceleratorBuffer::setName, "")
      .def("setMeasurements", &xacc::AcceleratorBuffer::setMeasurements, "")
      .def("appendMeasurement",
           (void (xacc::AcceleratorBuffer::*)(const std::string &)) &
               xacc::AcceleratorBuffer::appendMeasurement,
           "Append the measurement string")
      .def("appendMeasurement",
           (void (xacc::AcceleratorBuffer::*)(const std::string, const int)) &
               xacc::AcceleratorBuffer::appendMeasurement,
           "Append the measurement string")
      .def("getMeasurements", &xacc::AcceleratorBuffer::getMeasurements,
           "Return observed measurement bit strings")
      .def("computeMeasurementProbability",
           &xacc::AcceleratorBuffer::computeMeasurementProbability,
           "Compute the probability of a given bit string")
      .def("getMeasurementCounts",
           &xacc::AcceleratorBuffer::getMeasurementCounts,
           "Return the mapping of measure bit strings to their counts.")
      .def("getChildren",
           (std::vector<std::shared_ptr<AcceleratorBuffer>>(
               xacc::AcceleratorBuffer::*)(const std::string)) &
               xacc::AcceleratorBuffer::getChildren,
           "")
      .def("nChildren", &xacc::AcceleratorBuffer::nChildren, "")
      .def("getChildren",
           (std::vector<std::shared_ptr<AcceleratorBuffer>>(
               xacc::AcceleratorBuffer::*)()) &
               xacc::AcceleratorBuffer::getChildren,
           "")
      .def("setSize", &xacc::AcceleratorBuffer::setSize, "")
      .def("getChildrenNames", &xacc::AcceleratorBuffer::getChildrenNames, "")
      .def("keys", &xacc::AcceleratorBuffer::listExtraInfoKeys, "")
      .def("getInformation",
           (std::map<std::string, ExtraInfo>(xacc::AcceleratorBuffer::*)()) &
               xacc::AcceleratorBuffer::getInformation,
           "")
      .def(
          "__getitem__",
          [](AcceleratorBuffer &b, const std::string key) {
            return b.getInformation(key);
          },
          "")
      .def("addExtraInfo",
           (void (xacc::AcceleratorBuffer::*)(const std::string, ExtraInfo)) &
               xacc::AcceleratorBuffer::addExtraInfo,
           "")
      .def("getInformation",
           (ExtraInfo(xacc::AcceleratorBuffer::*)(const std::string)) &
               xacc::AcceleratorBuffer::getInformation,
           "")
      .def("appendChild", &xacc::AcceleratorBuffer::appendChild, "")
      .def("hasExtraInfoKey", &xacc::AcceleratorBuffer::hasExtraInfoKey, "")
      .def("name", &xacc::AcceleratorBuffer::name, "")
      .def("getAllUnique", &xacc::AcceleratorBuffer::getAllUnique,
           "Return all unique information with the provided string name")
      .def("__repr__", &xacc::AcceleratorBuffer::toString, "")
      .def("__str__", &xacc::AcceleratorBuffer::toString, "")
      .def("getChildren",
           (std::vector<std::shared_ptr<AcceleratorBuffer>>(
               xacc::AcceleratorBuffer::*)(const std::string, ExtraInfo)) &
               xacc::AcceleratorBuffer::getChildren,
           "")
      .def(
          "getMarginalCounts",
          // Default to MSB if not explicitly specified
          [](AcceleratorBuffer &b, const std::vector<int> &idxs) {
            return b.getMarginalCounts(idxs, AcceleratorBuffer::BitOrder::MSB);
          },
          "Return the mapping of marginal measure bit strings to their "
          "counts.")
      .def("getMarginalCounts", &xacc::AcceleratorBuffer::getMarginalCounts,
           "Return the mapping of marginal measure bit strings to their "
           "counts.");
  py::enum_<xacc::AcceleratorBuffer::BitOrder>(m, "BitOrder")
      .value("LSB", xacc::AcceleratorBuffer::BitOrder::LSB)
      .value("MSB", xacc::AcceleratorBuffer::BitOrder::MSB)
      .export_values();
  // Expose xacc::NoiseModel
  py::class_<xacc::NoiseModel, std::shared_ptr<xacc::NoiseModel>, PyNoiseModel>
      noise_model(m, "NoiseModel", "");
  noise_model.def(py::init<>())
      .def("initialize", &xacc::NoiseModel::initialize, "")
      .def("toJson", &xacc::NoiseModel::toJson, "");
  m.def("getNoiseModel",
        [](const std::string name, const PyHeterogeneousMap &options)
            -> std::shared_ptr<xacc::NoiseModel> {
          HeterogeneousMap m;
          for (auto &item : options) {
            PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
            mpark::visit(vis, item.second);
          }
          auto noise_model = xacc::getService<xacc::NoiseModel>(name);
          noise_model->initialize(m);
          return noise_model;
        });
}