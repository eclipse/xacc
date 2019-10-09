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
#include "CompositeInstruction.hpp"
#include "Optimizer.hpp"
#include "xacc.hpp"
#include "heterogeneous.hpp"
#include "xacc_service.hpp"

#include "IRProvider.hpp"
#include "InstructionIterator.hpp"
#include "AcceleratorBuffer.hpp"
#include "AcceleratorDecorator.hpp"
// #include "EmbeddingAlgorithm.hpp"

#include "PauliOperator.hpp"
#include "FermionOperator.hpp"

#include <memory>
#include <pybind11/complex.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/eigen.h>
#include <pybind11/iostream.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>

namespace py = pybind11;
using namespace xacc;
using namespace xacc::quantum;

// `boost::variant` as an example -- can be any `std::variant`-like container
namespace pybind11 {
namespace detail {
template <typename... Ts>
struct type_caster<Variant<Ts...>> : variant_caster<Variant<Ts...>> {};

// Specifies the function used to visit the variant -- `apply_visitor` instead
// of `visit`
template <> struct visit_helper<Variant> {
  template <typename... Args>
  static auto call(Args &&... args) -> decltype(mpark::visit(args...)) {
    return mpark::visit(args...);
  }
};

template <typename... Ts>
struct type_caster<mpark::variant<Ts...>>
    : variant_caster<mpark::variant<Ts...>> {};

// Specifies the function used to visit the variant -- `apply_visitor` instead
// of `visit`
template <> struct visit_helper<mpark::variant> {
  template <typename... Args>
  static auto call(Args &&... args) -> decltype(mpark::visit(args...)) {
    return mpark::visit(args...);
  }
};
} // namespace detail
} // namespace pybind11

using PyHeterogeneousMapTypes =
    xacc::Variant<int, double, std::string, std::vector<std::string>,
                  std::vector<double>, std::vector<int>, std::complex<double>,
                  std::shared_ptr<CompositeInstruction>,
                  std::shared_ptr<Instruction>, std::shared_ptr<Accelerator>,
                  std::shared_ptr<Observable>, std::shared_ptr<Optimizer>>;
using PyHeterogeneousMap = std::map<std::string, PyHeterogeneousMapTypes>;

class PyHeterogeneousMap2HeterogeneousMap {
protected:
  HeterogeneousMap &m;
  const std::string &key;

public:
  PyHeterogeneousMap2HeterogeneousMap(HeterogeneousMap &map,
                                      const std::string &k)
      : m(map), key(k) {}
  template <typename T> void operator()(const T &t) { m.insert(key, t); }
};

class PyAccelerator : public xacc::Accelerator {
public:
  /* Inherit the constructors */
  using Accelerator::Accelerator;

  const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::Accelerator, name);
  }

  const std::string description() const override { return ""; }

  void initialize(const HeterogeneousMap &params = {}) override { return; }

  std::vector<std::shared_ptr<IRTransformation>>
  getIRTransformations() override {
    return {};
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
};

class PyCompiler : public xacc::Compiler {
public:
  /* Inherit the constructors */
  using Compiler::Compiler;

  const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::Compiler, name);
  }
  const std::string description() const override { return ""; }

  std::shared_ptr<IR> compile(const std::string &src,
                              std::shared_ptr<Accelerator> acc) override {
    PYBIND11_OVERLOAD_PURE(std::shared_ptr<IR>, xacc::Compiler, compile, src,
                           acc);
  }

  std::shared_ptr<IR> compile(const std::string &src) override {
    return compile(src, nullptr);
  }

  const std::string
  translate(std::shared_ptr<CompositeInstruction> function) override {
    return "";
  }
};

PYBIND11_MODULE(_pyxacc, m) {
  m.doc() =
      "Python bindings for XACC. XACC provides a plugin infrastructure for "
      "programming, compiling, and executing quantum kernels in a language and "
      "hardware agnostic manner.";

  py::class_<xacc::HeterogeneousMap>(m, "HeterogeneousMap", "")
      .def(py::init<>(), "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(const std::string, const int &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(const std::string,
                                             const double &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(const std::string,
                                             const std::string &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(const std::string,
                                             const std::vector<int> &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(const std::string,
                                             const std::vector<double> &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(
               const std::string, const std::vector<std::pair<int, int>> &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(
               const std::string, const std::shared_ptr<Observable> &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("insert",
           (void (xacc::HeterogeneousMap::*)(
               const std::string, const std::shared_ptr<Optimizer> &)) &
               xacc::HeterogeneousMap::insert,
           "")
      .def("__getitem__", &xacc::HeterogeneousMap::get<std::vector<double>>, "")
      .def("__getitem__", &xacc::HeterogeneousMap::get<double>, "")
      .def("__getitem__", &xacc::HeterogeneousMap::get<int>, "")
      .def("__getitem__", &xacc::HeterogeneousMap::get<std::vector<int>>, "")
      .def("__getitem__",
           &xacc::HeterogeneousMap::get<std::vector<std::string>>, "")
      .def("__getitem__",
           &xacc::HeterogeneousMap::get<std::vector<std::pair<int, int>>>, "")
      .def("__getitem__",
           &xacc::HeterogeneousMap::get<std::shared_ptr<Observable>>, "")
      .def("__getitem__",
           &xacc::HeterogeneousMap::get<std::shared_ptr<Optimizer>>, "");

  py::class_<xacc::InstructionParameter>(
      m, "InstructionParameter",
      "The InstructionParameter provides a variant structure "
      "to provide parameters to XACC Instructions at runtime. "
      "This type can be an int, double, float, string, or complex value.")
      .def(py::init<int>(), "Construct as an int.")
      .def(py::init<double>(), "Construct as a double.")
      .def(py::init<std::string>(), "Construct as a string.");

  py::class_<PyHeterogeneousMapTypes>(
      m, "PyHeterogeneousMap",
      "The PyHeterogeneousMap provides a variant structure "
      "to provide parameters to XACC HeterogeneousMaps."
      "This type can be an int, double, string, List[int], List[double]"
      "List[string], Observable, Accelerator, Function, or Optimizer.")
      .def(py::init<int>(), "Construct as an int.")
      .def(py::init<double>(), "Construct as a double.")
      .def(py::init<std::string>(), "Construct as a string.")
      .def(py::init<std::vector<std::string>>(), "Construct as a List[string].")
      .def(py::init<std::vector<int>>(), "Construct as a List[int].")
      .def(py::init<std::vector<double>>(), "Construct as a List[double].")
      .def(py::init<std::shared_ptr<xacc::Accelerator>>(),
           "Construct as an Accelerator.")
      .def(py::init<std::shared_ptr<xacc::CompositeInstruction>>(),
           "Construct as a CompositeInstruction.")
      .def(py::init<std::shared_ptr<xacc::Instruction>>(),
           "Construct as a CompositeInstruction.")
      .def(py::init<std::shared_ptr<xacc::Observable>>(),
           "Construct as an Observable.")
      .def(py::init<std::shared_ptr<xacc::Optimizer>>(),
           "Construct as an Optimizer.");

  py::class_<xacc::ContributableService>(m, "ContributableService", "")
      .def(py::init<std::shared_ptr<Instruction>>(), "");

  py::class_<xacc::Instruction, std::shared_ptr<xacc::Instruction>>(
      m, "Instruction", "")
      .def("nParameters", &xacc::Instruction::nParameters, "")
      .def("toString", &xacc::Instruction::toString, "")
      .def("isEnabled", &xacc::Instruction::isEnabled, "")
      .def("isComposite", &xacc::Instruction::isComposite, "")
      .def("bits", &xacc::Instruction::bits, "")
      .def("getParameter", &xacc::Instruction::getParameter, "")
      .def("getParameters", &xacc::Instruction::getParameters, "")
      .def("setParameter",
           (void (xacc::Instruction::*)(const std::size_t,
                                        InstructionParameter &)) &
               xacc::Instruction::setParameter,
           "")
      .def("mapBits", &xacc::Instruction::mapBits, "")
      .def("name", &xacc::Instruction::name, "")
      .def("description", &xacc::Instruction::description, "");

  py::class_<xacc::CompositeInstruction,
             std::shared_ptr<xacc::CompositeInstruction>>(
      m, "CompositeInstruction", "")
      .def("nInstructions", &xacc::CompositeInstruction::nInstructions, "")
      .def("getInstruction", &xacc::CompositeInstruction::getInstruction, "")
      .def("getInstructions", &xacc::CompositeInstruction::getInstructions, "")
      .def("removeInstruction", &xacc::CompositeInstruction::removeInstruction,
           "")
      .def("replaceInstruction",
           &xacc::CompositeInstruction::replaceInstruction, "")
      .def("insertInstruction", &xacc::CompositeInstruction::insertInstruction,
           "")
      .def(
          "addInstructions",
          (void (xacc::CompositeInstruction::*)(const std::vector<InstPtr> &)) &
              xacc::CompositeInstruction::addInstructions,
          "")
      .def("addInstruction", &xacc::CompositeInstruction::addInstruction, "")

      .def("addVariable", &xacc::CompositeInstruction::addVariable, "")
      .def("addVariables",
           (void (xacc::CompositeInstruction::*)(
               const std::vector<std::string> &)) &
               xacc::CompositeInstruction::addVariables,
           "")
      .def("expand", &xacc::CompositeInstruction::expand, "")
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
      .def("mapBits", &xacc::CompositeInstruction::mapBits, "");

  // Expose the IR interface
  py::class_<xacc::IR, std::shared_ptr<xacc::IR>>(
      m, "IR",
      "The XACC Intermediate Representation, "
      "serves as a container for XACC Functions.")
      .def("getComposites", &xacc::IR::getComposites,
           "Return the kernels in this IR")
      .def("mapBits", &xacc::IR::mapBits, "")
      .def("addComposite", &xacc::IR::addComposite, "");

  py::class_<xacc::InstructionIterator>(m, "InstructionIterator", "")
      .def(py::init<std::shared_ptr<xacc::CompositeInstruction>>())
      .def("hasNext", &xacc::InstructionIterator::hasNext, "")
      .def("next", &xacc::InstructionIterator::next, "");

  py::class_<xacc::IRTransformation, std::shared_ptr<xacc::IRTransformation>>(
      m, "IRTransformation", "")
      .def("transform", &xacc::IRTransformation::transform, "");

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
      .def("execute",
           (void (xacc::Accelerator::*)(
               std::shared_ptr<AcceleratorBuffer>,
               const std::shared_ptr<CompositeInstruction>)) &
               xacc::Accelerator::execute,
           "Execute the Function with the given AcceleratorBuffer.")
      .def("execute",
           (void (xacc::Accelerator::*)(
               std::shared_ptr<AcceleratorBuffer>,
               const std::vector<std::shared_ptr<CompositeInstruction>>)) &
               xacc::Accelerator::execute,
           "Execute the Function with the given AcceleratorBuffer.")
      .def("initialize", &xacc::Accelerator::initialize, "")
      .def("getIRTransformations", &xacc::Accelerator::getIRTransformations, "")
      .def("updateConfiguration",
           (void (xacc::Accelerator::*)(const HeterogeneousMap &)) &
               xacc::Accelerator::updateConfiguration,
           "")
      .def("configurationKeys", &xacc::Accelerator::configurationKeys, "")
      .def("getOneBitErrorRates", &xacc::Accelerator::getOneBitErrorRates, "")
      .def("getTwoBitErrorRates", &xacc::Accelerator::getTwoBitErrorRates, "");

  py::class_<xacc::AcceleratorDecorator, xacc::Accelerator,
             std::shared_ptr<xacc::AcceleratorDecorator>>
      accd(m, "AcceleratorDecorator", "");
  accd.def("setDecorated", &xacc::AcceleratorDecorator::setDecorated, "");

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
      .def("appendMeasurement",
           (void (xacc::AcceleratorBuffer::*)(const std::string &)) &
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
      .def("getChildrenNames", &xacc::AcceleratorBuffer::getChildrenNames, "")
      .def("listExtraInfoKeys", &xacc::AcceleratorBuffer::listExtraInfoKeys, "")
      .def("getInformation",
           (std::map<std::string, ExtraInfo>(xacc::AcceleratorBuffer::*)()) &
               xacc::AcceleratorBuffer::getInformation,
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
           "");

  // Expose the Compiler
  py::class_<xacc::Compiler, std::shared_ptr<xacc::Compiler>, PyCompiler>(
      m, "Compiler",
      "The XACC Compiler takes as input quantum kernel source code, "
      "and compiles it to the XACC intermediate representation.")
      .def(py::init<>(), "")
      .def("name", &xacc::Compiler::name, "Return the name of this Compiler.")
      .def("compile",
           (std::shared_ptr<xacc::IR>(xacc::Compiler::*)(
               const std::string &, std::shared_ptr<xacc::Accelerator>)) &
               xacc::Compiler::compile,
           "Compile the "
           "given source code against the given Accelerator.")
      .def("compile",
           (std::shared_ptr<xacc::IR>(xacc::Compiler::*)(const std::string &)) &
               xacc::Compiler::compile,
           "Compile the "
           "given source code.")
      .def("translate", &xacc::Compiler::translate,
           "Translate the given IR Function instance to source code in this "
           "Compiler's language.");

  py::class_<xacc::Algorithm, std::shared_ptr<xacc::Algorithm>>(
      m, "Algorithm",
      "The XACC Algorithm interface takes as input a dictionary of "
      "AlgorithmParameters "
      "and executes the desired Algorithm.")
      .def("name", &xacc::Algorithm::name, "Return the name of this Algorithm.")
      .def("execute",
           (void (xacc::Algorithm::*)(
               const std::shared_ptr<xacc::AcceleratorBuffer>)) &
               xacc::Algorithm::execute,
           "Execute the Algorithm, storing the results in provided "
           "AcceleratorBuffer.")
      .def("initialize",
           (bool (xacc::Algorithm::*)(const HeterogeneousMap &)) &
               xacc::Algorithm::initialize,
           "Initialize the algorithm with given AlgorithmParameters.");

  py::class_<xacc::OptFunction>(m, "OptFunction", "")
      .def(py::init<std::function<double(const std::vector<double> &,
                                         std::vector<double> &)>,
                    const int>());

  // Expose Optimizer
  py::class_<xacc::Optimizer, std::shared_ptr<xacc::Optimizer>>(
      m, "Optimizer",
      "The Optimizer interface provides optimization routine implementations "
      "for use in algorithms.")
      .def("optimize",
           [&](xacc::Optimizer &o,
               std::function<std::pair<double, std::vector<double>>(
                   const std::vector<double> &x)> &f,
               const int ndim) {
             OptFunction opt(
                 [&](const std::vector<double> &x, std::vector<double> &grad) {
                   auto ret = f(x);
                   if (grad.size() == ndim) {
                     for (int i = 0; i < grad.size(); i++) {
                       grad[i] = ret.second[i];
                     }
                   }
                   return ret.first;
                 },
                 ndim);
             return o.optimize(opt);
           })
    .def("optimize",
           [&](xacc::Optimizer &o,
               std::function<double(
                   const std::vector<double> &x)> &f,
               const int ndim) {
             OptFunction opt(
                 [&](const std::vector<double> &x, std::vector<double> &grad) {
                   return f(x);
                 },
                 ndim);
             return o.optimize(opt);
           })
      .def("setOptions",
           (void (xacc::Optimizer::*)(const HeterogeneousMap &)) &
               xacc::Optimizer::setOptions,
           "");

  py::class_<xacc::Observable, std::shared_ptr<xacc::Observable>>(
      m, "Observable", "The Observable interface.")
      .def("observe",
           (std::vector<std::shared_ptr<xacc::CompositeInstruction>>(
               xacc::Observable::*)(
               std::shared_ptr<xacc::CompositeInstruction>)) &
               xacc::Observable::observe,
           "")
      .def("toString", &xacc::Observable::toString,
           "Return string representation of this Observable.")
      .def("nBits", &xacc::Observable::nBits,
           "Return the number of bits this Observable represents.")
      .def("fromOptions",
           (void (xacc::Observable::*)(const HeterogeneousMap &)) &
               xacc::Observable::fromOptions,
           "")
      .def("fromString",
           (void (xacc::Observable::*)(const std::string)) &
               xacc::Observable::fromString,
           "");

  py::class_<Term>(m, "Term").def("coeff", &Term::coeff).def("ops", &Term::ops);
  py::class_<PauliOperator, xacc::Observable, std::shared_ptr<PauliOperator>>(
      m, "PauliOperator")
      .def(py::init<>())
      .def(py::init<std::complex<double>>())
      .def(py::init<double>())
      .def(py::init<std::string>())
      .def(py::init<std::map<int, std::string>>())
      .def(py::init<std::map<int, std::string>, double>())
      .def(py::init<std::map<int, std::string>, std::complex<double>>())
      .def(py::init<std::map<int, std::string>, std::string>())
      .def(py::init<std::map<int, std::string>, std::complex<double>,
                    std::string>())
      .def(py::self + py::self)
      .def(py::self += py::self)
      .def(py::self *= py::self)
      .def(py::self *= double())
      .def(py::self * py::self)
      .def(py::self *= std::complex<double>())
      .def(py::self -= py::self)
      .def(py::self - py::self)
      .def("__eq__", &PauliOperator::operator==)
      .def("__repr__", &PauliOperator::toString)
      .def("eval", &PauliOperator::eval)
      .def("toBinaryVectors", &PauliOperator::toBinaryVectors)
      .def("toXACCIR", &PauliOperator::toXACCIR)
      .def("fromXACCIR", &PauliOperator::fromXACCIR)
      .def("fromString", &PauliOperator::fromString)
      .def("nTerms", &PauliOperator::nTerms)
      .def("isClose", &PauliOperator::isClose)
      .def("commutes", &PauliOperator::commutes)
      .def("__len__", &PauliOperator::nTerms)
      .def("nQubits", &PauliOperator::nQubits)
      .def("computeActionOnKet", &PauliOperator::computeActionOnKet)
      .def("computeActionOnBra", &PauliOperator::computeActionOnBra)
      .def(
          "__iter__",
          [](PauliOperator &op) {
            return py::make_iterator(op.begin(), op.end());
          },
          py::keep_alive<0, 1>());

  // Expose XACC API functions
  m.def("Initialize", (void (*)(std::vector<std::string>)) & xacc::Initialize,
        "Initialize the framework. Can provide a list of strings to model "
        "command line arguments. For instance, "
        "to print the XACC help message, pass ['--help'] to this function, or "
        "to set the compiler to use, ['--compiler','scaffold'].");
  m.def("Initialize", (void (*)()) & xacc::Initialize,
        "Initialize the framework. Use this if there are no command line "
        "arguments to pass.");
  m.def("PyInitialize", &xacc::PyInitialize,
        "Initialize the framework from Python.");
  // m.def("help", )
  m.def(
      "getAccelerator",
      [](const std::string &name, const PyHeterogeneousMap &p = {}) {
        HeterogeneousMap m;
        for (auto &item : p) {
          PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
          mpark::visit(vis, item.second);
        }
        return xacc::getAccelerator(name, m);
      },
      py::arg("name"), py::arg("p") = PyHeterogeneousMap(),
      py::return_value_policy::reference,
      "Return the accelerator with given name.");
  m.def("getObservable",
        [](const std::string &type,
           const std::string representation) -> std::shared_ptr<Observable> {
          if (type == "pauli") {
            return representation.empty()
                       ? std::make_shared<PauliOperator>()
                       : std::make_shared<PauliOperator>(representation);
          } else if (type == "fermion") {
            return representation.empty()
                       ? std::make_shared<FermionOperator>()
                       : std::make_shared<FermionOperator>(representation);
          } else if (xacc::hasService<Observable>(type)) {
            auto obs = xacc::getService<Observable>(type);
            obs->fromString(representation);
            return obs;
          } else {
            xacc::error("Invalid observable type");
            return std::make_shared<PauliOperator>();
          }
        });
  m.def("getCompiler",
        (std::shared_ptr<xacc::Compiler>(*)(const std::string &)) &
            xacc::getCompiler,
        py::return_value_policy::reference,
        "Return the Compiler of given name.");
  m.def("getIRTransformation",
        (std::shared_ptr<xacc::IRTransformation>(*)(const std::string &)) &
            xacc::getService<IRTransformation>,
        py::return_value_policy::reference,
        "Return the IRTransformation of given name.");
  m.def("getConnectivity",
        [](const std::string acc) -> std::vector<std::pair<int, int>> {
          auto a = xacc::getAccelerator(acc);
          return a->getConnectivity();
        });
  m.def("translate", &xacc::translate,
        "Translate the provided IR Function to the given language.");
  m.def("setOption", [](const std::string s, InstructionParameter p) {
    xacc::setOption(s, p.toString());
  });
  m.def("qasm", &xacc::qasm, "");
  m.def("getCompiled", &xacc::getCompiled, "");
  m.def(
      "qalloc",
      [](const int n) -> std::shared_ptr<xacc::AcceleratorBuffer> {
        return xacc::qalloc(n);
      },
      "");
  m.def(
      "info", [](const std::string s) { xacc::info(s); }, "");
  m.def(
      "error", [](const std::string s) { xacc::error(s); }, "");
  m.def("setOption", &xacc::setOption, "Set an XACC framework option.");
  m.def(
      "setOptions",
      [](std::map<std::string, std::string> options) {
        for (auto &kv : options)
          xacc::setOption(kv.first, kv.second);
      },
      "Set a number of options at once.");
  m.def("getAcceleratorDecorator",
        [](const std::string name, std::shared_ptr<Accelerator> acc) {
          auto accd = xacc::getService<AcceleratorDecorator>(name);
          accd->setDecorated(acc);
          return accd;
        });
  m.def(
      "setOptions",
      [](std::map<std::string, InstructionParameter> options) {
        for (auto &kv : options)
          xacc::setOption(kv.first, kv.second.toString());
      },
      "Set a number of options at once.");
  m.def("unsetOption", &xacc::unsetOption, "");
  m.def("getOption",
        (const std::string (*)(const std::string &)) & xacc::getOption,
        "Get an XACC framework option.");
  m.def("hasAccelerator", &xacc::hasAccelerator,
        "Does XACC have the given Accelerator installed?");
  m.def("hasCompiler", &xacc::hasCompiler,
        "Does XACC have the given Accelerator installed?");
  m.def("optionExists", &xacc::optionExists, "Set an XACC framework option.");
  m.def("setIsPyApi", &xacc::setIsPyApi,
        "Indicate that this is using XACC via the Python API.");
  m.def("getCache", &xacc::getCache, "");
  m.def("Finalize", &xacc::Finalize, "Finalize the framework");
  m.def(
      "loadBuffer",
      [](const std::string json) {
        std::istringstream s(json);
        auto buffer = std::make_shared<AcceleratorBuffer>();
        buffer->load(s);
        return buffer;
      },
      "");

  m.def("getAlgorithm",
        (std::shared_ptr<xacc::Algorithm>(*)(const std::string)) &
            xacc::getAlgorithm,
        "");
  m.def(
      "getAlgorithm",
      [](const std::string algo, const PyHeterogeneousMap &options) {
        HeterogeneousMap m;
        for (auto &item : options) {
          PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
          mpark::visit(vis, item.second);
        }
        return xacc::getAlgorithm(algo, m);
      },
      "");
  m.def("getOptimizer",
        (std::shared_ptr<xacc::Optimizer>(*)(const std::string)) &
            xacc::getOptimizer,
        "");
  m.def(
      "getOptimizer",
      [](const std::string optName, const PyHeterogeneousMap &options) {
        HeterogeneousMap m;
        for (auto &item : options) {
          PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
          mpark::visit(vis, item.second);
        }
        return xacc::getOptimizer(optName, m);
      },

      "");
  m.def("getComposite", [](const std::string &name) {
    auto c = xacc::getService<Instruction>(name);
    return std::dynamic_pointer_cast<CompositeInstruction>(c);
  });
  m.def(
      "contributeService",
      [](const std::string name, ContributableService &service) {
        xacc::contributeService(name, service);
      },
      "");

  m.def(
      "compileKernel",
      [](std::shared_ptr<Accelerator> acc, const std::string &src,
         const std::string &compilerName =
             "") -> std::shared_ptr<CompositeInstruction> {
        if (!compilerName.empty())
          xacc::setOption("compiler", compilerName);

        auto compiler = xacc::getCompiler();
        auto ir = compiler->compile(src, acc);
        return ir->getComposites()[0];
      },
      py::arg("acc"), py::arg("src"), py::arg("compilerName") = std::string(""),
      py::return_value_policy::move, "");

  m.def(
      "compositeToInstruction",
      [](std::shared_ptr<CompositeInstruction> f) {
        return std::dynamic_pointer_cast<Instruction>(f);
      },
      py::return_value_policy::copy);

  py::module gatesub =
      m.def_submodule("gate", "Gate model quantum computing data structures.");
  gatesub.def(
      "create",
      [](const std::string &name, std::vector<std::size_t> qbits,
         std::vector<InstructionParameter> params =
             std::vector<InstructionParameter>{})
          -> std::shared_ptr<Instruction> {
        return xacc::getService<IRProvider>("quantum")->createInstruction(
            name, qbits, params);
      },
      "", py::arg("name"), py::arg("qbits"),
      py::arg("params") = std::vector<InstructionParameter>{});
  gatesub.def(
      "createComposite",
      [](const std::string &name,
         std::vector<std::string> variables = std::vector<std::string>{})
          -> std::shared_ptr<CompositeInstruction> {
        return xacc::getService<IRProvider>("quantum")->createComposite(
            name, variables);
      },
      "", py::arg("name"), py::arg("variables") = std::vector<std::string>{});
  gatesub.def(
      "createIR",
      []() -> std::shared_ptr<IR> {
        return xacc::getService<IRProvider>("quantum")->createIR();
      },
      "");

  gatesub.def(
      "getState",
      [](std::shared_ptr<Accelerator> acc,
         std::shared_ptr<CompositeInstruction> f) {
        auto results = acc->getAcceleratorState(f);
        Eigen::VectorXcd ret =
            Eigen::Map<Eigen::VectorXcd>(results.data(), results.size());
        return ret;
      },
      "Compute and return the state after execution of the given program on "
      "the given accelerator.");

  //   py::module aqcsub =
  //       m.def_submodule("dwave", "Gate model quantum computing data
  //       structures.");
  //   aqcsub.def(
  //       "create",
  //       [](const std::string &name, std::vector<int> qbits,
  //          std::vector<InstructionParameter> params =
  //              std::vector<InstructionParameter>{})
  //           -> std::shared_ptr<Instruction> {
  //         return xacc::getService<IRProvider>("dwave")->createInstruction(
  //             name, qbits, params);
  //       },
  //       "Convenience function for creating a new DWInstruction.",
  //       py::arg("name"), py::arg("qbits"), py::arg("params") =
  //       std::vector<InstructionParameter>{});
  //   aqcsub.def(
  //       "createFunction",
  //       [](const std::string &name, std::vector<int> qbits,
  //          std::vector<InstructionParameter> params =
  //              std::vector<InstructionParameter>{}) ->
  //              std::shared_ptr<Function> {
  //         return xacc::getService<IRProvider>("dwave")->createFunction(
  //             name, qbits, params);
  //       },
  //       "Convenience function for creating a new DWFunction.",
  //       py::arg("name"), py::arg("qbits"), py::arg("params") =
  //       std::vector<InstructionParameter>{});
  //   aqcsub.def(
  //       "createIR",
  //       []() -> std::shared_ptr<IR> {
  //         return xacc::getService<IRProvider>("dwave")->createIR();
  //       },
  //       "Convenience function for creating a new DWIR.");

  //   aqcsub.def(
  //       "embed",
  //       [](std::shared_ptr<Function> f, std::shared_ptr<Accelerator> acc,
  //          const std::string algo) -> std::map<int, std::vector<int>> {
  //         auto a = xacc::getService<xacc::quantum::EmbeddingAlgorithm>(algo);
  //         auto hardwareconnections = acc->getAcceleratorConnectivity();
  //         std::set<int> nUniqueBits;
  //         for (auto &edge : hardwareconnections) {
  //           nUniqueBits.insert(edge.first);
  //           nUniqueBits.insert(edge.second);
  //         }

  //         int nBits =
  //             *std::max_element(nUniqueBits.begin(), nUniqueBits.end()) + 1;

  //         auto hardware = xacc::getService<Graph>("boost-ugraph");
  //         for (int i = 0; i < nBits; i++) {
  //           std::map<std::string, InstructionParameter> m{{"bias", 1.0}};
  //           hardware->addVertex(m);
  //         }

  //         for (auto &edge : hardwareconnections) {
  //           hardware->addEdge(edge.first, edge.second);
  //         }

  //         int maxBitIdx = 0;
  //         for (auto inst : f->getInstructions()) {
  //           if (inst->name() == "dw-qmi") {
  //             auto qbit1 = inst->bits()[0];
  //             auto qbit2 = inst->bits()[1];
  //             if (qbit1 > maxBitIdx) {
  //               maxBitIdx = qbit1;
  //             }
  //             if (qbit2 > maxBitIdx) {
  //               maxBitIdx = qbit2;
  //             }
  //           }
  //         }

  //         auto problemGraph = xacc::getService<Graph>("boost-ugraph");
  //         for (int i = 0; i < maxBitIdx + 1; i++) {
  //           std::map<std::string, InstructionParameter> m{{"bias", 1.0}};
  //           problemGraph->addVertex(m);
  //         }

  //         for (auto inst : f->getInstructions()) {
  //           if (inst->name() == "dw-qmi") {
  //             auto qbit1 = inst->bits()[0];
  //             auto qbit2 = inst->bits()[1];
  //             if (qbit1 != qbit2) {
  //               problemGraph->addEdge(qbit1, qbit2, 1.0);
  //             }
  //           }
  //         }

  //         return a->embed(problemGraph, hardware);
  //       },
  //       "");
}
