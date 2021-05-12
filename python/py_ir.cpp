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
#include "py_ir.hpp"
#include "CompositeInstruction.hpp"
#include "IRTransformation.hpp"
#include "heterogeneous.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "InstructionIterator.hpp"
#include "py_heterogeneous_map.hpp"

void bind_ir(py::module &m) {

  py::enum_<xacc::IRTransformationType>(m, "IRTransformationType")
      .value("Optimization", xacc::IRTransformationType::Optimization)
      .value("Placement", xacc::IRTransformationType::Placement)
      .value("ErrorCorrection", xacc::IRTransformationType::ErrorCorrection)
      .export_values();

  py::class_<xacc::InstructionParameter>(
      m, "InstructionParameter",
      "The InstructionParameter provides a variant structure "
      "to provide parameters to XACC Instructions at runtime. "
      "This type can be an int, double, float, string, or complex value.")
      .def(py::init<int>(), "Construct as an int.")
      .def(py::init<double>(), "Construct as a double.")
      .def(py::init<std::string>(), "Construct as a string.");

  py::class_<xacc::ContributableService>(m, "ContributableService", "")
      .def(py::init<std::shared_ptr<Instruction>>(), "");

  py::class_<xacc::BaseInstructionVisitor,
             std::shared_ptr<xacc::BaseInstructionVisitor>>(
      m, "BaseInstructionVisitor");

  py::class_<xacc::Instruction, std::shared_ptr<xacc::Instruction>>(
      m, "Instruction", "")
      .def("nParameters", &xacc::Instruction::nParameters, "")
      .def("toString", &xacc::Instruction::toString, "")
      .def("isEnabled", &xacc::Instruction::isEnabled, "")
      .def("isComposite", &xacc::Instruction::isComposite, "")
      .def("nRequiredBits", &xacc::Instruction::nRequiredBits, "")
      .def("bits", &xacc::Instruction::bits, "")
      .def("setBits", &xacc::Instruction::setBits, "")
      .def("getParameter", &xacc::Instruction::getParameter, "")
      .def("getParameters", &xacc::Instruction::getParameters, "")
      .def("setParameter",
           (void (xacc::Instruction::*)(const std::size_t,
                                        InstructionParameter &)) &
               xacc::Instruction::setParameter,
           "")
      .def("mapBits", &xacc::Instruction::mapBits, "")
      .def("name", &xacc::Instruction::name, "")
      .def("description", &xacc::Instruction::description, "")
      .def("setChannel", &xacc::Instruction::setChannel, "")
      .def("setStart", &xacc::Instruction::setStart, "")
      .def("setDuration", &xacc::Instruction::setDuration, "")
      .def("setSamples", &xacc::Instruction::setSamples, "")
      .def("getSamples", &xacc::Instruction::getSamples, "")
      .def("duration", &xacc::Instruction::duration, "")
      .def("start", &xacc::Instruction::start, "")
      .def("accept",
           [](xacc::Instruction &i,
              std::shared_ptr<xacc::BaseInstructionVisitor> visitor) {
             i.accept(visitor);
           })
      .def("accept",
           [](xacc::Instruction &i, xacc::BaseInstructionVisitor &visitor) {
             i.accept(&visitor);
           });

  py::class_<xacc::CompositeInstruction,
             std::shared_ptr<xacc::CompositeInstruction>>(
      m, "CompositeInstruction", "")
      .def("nParameters", &xacc::CompositeInstruction::nParameters, "")
      .def("nLogicalBits", &xacc::CompositeInstruction::nLogicalBits, "")
      .def("nPhysicalBits", &xacc::CompositeInstruction::nPhysicalBits, "")
      .def("nInstructions", &xacc::CompositeInstruction::nInstructions, "")
      .def("getInstruction", &xacc::CompositeInstruction::getInstruction, "")
      .def("getInstructions", &xacc::CompositeInstruction::getInstructions, "")
      .def("removeInstruction", &xacc::CompositeInstruction::removeInstruction,
           "")
      .def("replaceInstruction",
           &xacc::CompositeInstruction::replaceInstruction, "")
      .def("insertInstruction", &xacc::CompositeInstruction::insertInstruction,
           "")
      .def("nLogicalBits", &xacc::CompositeInstruction::nLogicalBits, "")
      .def(
          "addInstructions",
          (void (xacc::CompositeInstruction::*)(const std::vector<InstPtr> &)) &
              xacc::CompositeInstruction::addInstructions,
          "")
      .def("addInstruction", &xacc::CompositeInstruction::addInstruction, "")
      .def("addInstruction",
           [](xacc::CompositeInstruction &ci,
              std::shared_ptr<xacc::CompositeInstruction> toadd) {
             ci.addInstruction(toadd);
           })
      .def("clear", &xacc::CompositeInstruction::clear, "")
      .def("addVariable", &xacc::CompositeInstruction::addVariable, "")
      .def("addVariables",
           (void (xacc::CompositeInstruction::*)(
               const std::vector<std::string> &)) &
               xacc::CompositeInstruction::addVariables,
           "")
      .def("getVariables", &xacc::CompositeInstruction::getVariables, "")
      .def("expand", &xacc::CompositeInstruction::expand, "")
      .def(
          "expand",
          [](xacc::CompositeInstruction &c, PyHeterogeneousMap &options) {
            xacc::HeterogeneousMap m;
            for (auto &item : options) {
              PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
              mpark::visit(vis, item.second);
            }
            return c.expand(m);
          },
          "")
      .def("__str__", &xacc::CompositeInstruction::toString, "")
      .def("eval", &xacc::CompositeInstruction::operator(), "")
      .def("name", &xacc::CompositeInstruction::name, "")
      .def("description", &xacc::CompositeInstruction::description, "")
      .def("toString", &xacc::CompositeInstruction::toString, "")
      .def("enabledView", &xacc::CompositeInstruction::enabledView, "")
      .def("enable", &xacc::CompositeInstruction::enable, "")
      .def("getCoefficient", &xacc::CompositeInstruction::getCoefficient, "")
      .def("setCoefficient", &xacc::CompositeInstruction::setCoefficient, "")
      .def("depth", &xacc::CompositeInstruction::depth, "")
      .def("persistGraph", &xacc::CompositeInstruction::persistGraph, "")
      .def("mapBits", &xacc::CompositeInstruction::mapBits, "")
      .def("setTag", &xacc::CompositeInstruction::setTag, "")
      .def("getTag", &xacc::CompositeInstruction::getTag, "")
      .def(
          "defaultPlacement",
          [&](std::shared_ptr<CompositeInstruction> program,
              std::shared_ptr<Accelerator> qpu) {
            auto placement = qpu->defaultPlacementTransformation();
            auto irt = xacc::getIRTransformation(placement);
            irt->apply(program, qpu);
            return;
          },
          "")
      .def(
          "defaultPlacement",
          [&](std::shared_ptr<CompositeInstruction> program,
              std::shared_ptr<Accelerator> qpu,
              const PyHeterogeneousMap &options) {
            HeterogeneousMap m;
            for (auto &item : options) {
              PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
              mpark::visit(vis, item.second);
            }
            auto placement = qpu->defaultPlacementTransformation();
            auto irt = xacc::getIRTransformation(placement);
            irt->apply(program, qpu, m);
            return;
          },
          "");

  py::class_<xacc::IRProvider, std::shared_ptr<xacc::IRProvider>>(
      m, "IRProvider", "")
      .def(
          "createInstruction",
          [](IRProvider &p, const std::string name,
             std::vector<std::size_t> bits,
             std::vector<InstructionParameter> parameters,
             const PyHeterogeneousMap &analog_options) {
            HeterogeneousMap m;
            for (auto &item : analog_options) {
              PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
              mpark::visit(vis, item.second);
            }
            return p.createInstruction(name, bits, parameters, m);
          },
          "Return the kernels in this IR")
      .def(
          "createInstruction",
          [](IRProvider &p, const std::string name,
             std::vector<std::size_t> bits,
             std::vector<InstructionParameter> parameters) {
            return p.createInstruction(name, bits, parameters);
          },
          "Return the kernels in this IR")
      .def(
          "createInstruction",
          [](IRProvider &p, const std::string name,
             std::vector<std::size_t> bits) {
            return p.createInstruction(name, bits);
          },
          "Return the kernels in this IR")
      .def(
          "createInstruction",
          [](IRProvider &p, const std::string name) {
            return p.createInstruction(name, {});
          },
          "Return the kernels in this IR")
      .def(
          "createComposite",
          [](IRProvider &p, const std::string name) {
            return p.createComposite(name);
          },
          "")
      .def(
          "createComposite",
          [](IRProvider &p, const std::string name,
             std::vector<std::string> vars) {
            return p.createComposite(name, vars);
          },
          "")
      .def(
          "createComposite",
          [](IRProvider &p, const std::string name,
             std::vector<std::string> vars, const std::string type) {
            return p.createComposite(name, vars, type);
          },
          "")
      .def(
          "createIR", [](IRProvider &p) { return p.createIR(); }, "");

  // Expose the IR interface
  py::class_<xacc::IR, std::shared_ptr<xacc::IR>>(
      m, "IR",
      "The XACC Intermediate Representation, "
      "serves as a container for XACC Functions.")
      .def("getComposites", &xacc::IR::getComposites,
           "Return the kernels in this IR")
      .def("mapBits", &xacc::IR::mapBits, "")
      .def("getComposite", &xacc::IR::getComposite, "")
      .def("addComposite", &xacc::IR::addComposite, "");

  py::class_<xacc::InstructionIterator>(m, "InstructionIterator", "")
      .def(py::init<std::shared_ptr<xacc::CompositeInstruction>>())
      .def("hasNext", &xacc::InstructionIterator::hasNext, "")
      .def("next", &xacc::InstructionIterator::next, "");

  py::class_<xacc::IRTransformation, std::shared_ptr<xacc::IRTransformation>,
             PyIRTransformation>(m, "IRTransformation", "")
      .def(py::init<>())
      .def(
          "apply",
          [](IRTransformation &t, std::shared_ptr<CompositeInstruction> k,
             const PyHeterogeneousMap &options = {}) {
            HeterogeneousMap m;
            for (auto &item : options) {
              PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
              mpark::visit(vis, item.second);
            }
            t.apply(k, nullptr, m);
          },
          py::arg("k"), py::arg("options") = PyHeterogeneousMap(), "")
      .def(
          "apply",
          [](IRTransformation &t, std::shared_ptr<CompositeInstruction> k,
             std::shared_ptr<Accelerator> acc,
             const PyHeterogeneousMap &options = {}) {
            HeterogeneousMap m;
            for (auto &item : options) {
              PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
              mpark::visit(vis, item.second);
            }
            t.apply(k, acc, m);
          },
          py::arg("k"), py::arg("acc"),
          py::arg("options") = PyHeterogeneousMap(), "");
}