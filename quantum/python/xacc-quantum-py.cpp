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
#include "xacc-quantum-py.hpp"

#include "FermionOperator.hpp"
#include "ObservableTransform.hpp"
#include "PauliOperator.hpp"
#include "py_heterogeneous_map.hpp"
#include "py_observable.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

namespace py = pybind11;
using namespace xacc;
using namespace xacc::quantum;

void bind_quantum(py::module &m) {
  py::module k = m.def_submodule("quantum", "XACC python quantum submodule");

  py::class_<Term>(k, "Term").def("coeff", &Term::coeff).def("ops", &Term::ops);

  py::class_<PauliOperator, xacc::Observable, std::shared_ptr<PauliOperator>>(
      k, "PauliOperator")
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
      .def(py::self * double())
      .def(double() * py::self)
      .def("__add__",
           [](const PauliOperator &op, double idCoeff) { return op + idCoeff; })
      .def("__radd__",
           [](const PauliOperator &op, double idCoeff) { return op + idCoeff; })
      .def(py::self * py::self)
      .def(py::self *= std::complex<double>())
      .def(py::self * std::complex<double>())
      .def(std::complex<double>() * py::self)
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

  py::class_<FermionTerm>(k, "FermionTerm")
      .def("coeff", &FermionTerm::coeff)
      .def("ops", &FermionTerm::ops);

  py::class_<FermionOperator, xacc::Observable,
             std::shared_ptr<FermionOperator>>(k, "FermionOperator")
      .def("observe",
           (std::vector<std::shared_ptr<xacc::CompositeInstruction>>(
               xacc::Observable::*)(
               std::shared_ptr<xacc::CompositeInstruction>)) &
               xacc::Observable::observe,
           "")
      .def(py::init<>())
      .def(py::init<std::complex<double>>())
      .def(py::init<double>())
      .def(py::init<std::string>())
      .def(py::init<std::vector<std::pair<int, bool>>>())
      .def(py::init<std::vector<std::pair<int, bool>>, double>())
      .def(py::init<std::vector<std::pair<int, bool>>, std::complex<double>>())
      .def(py::init<std::vector<std::pair<int, bool>>, double, std::string>())
      .def(py::self + py::self)
      .def(py::self += py::self)
      .def(py::self *= py::self)
      .def(py::self *= double())
      .def(py::self * py::self)
      .def(py::self *= std::complex<double>())
      .def(py::self -= py::self)
      .def(py::self - py::self)
      .def("__eq__", &FermionOperator::operator==)
      .def("__repr__", &FermionOperator::toString)
      .def("fromString", &FermionOperator::fromString)
      .def("commutator", &FermionOperator::commutator)
      .def("hermitianConjugate", &FermionOperator::hermitianConjugate);

  k.def("getOperatorPool", [](const std::string &name) {
    return xacc::getService<OperatorPool>(name);
  });

  py::class_<OperatorPool, std::shared_ptr<OperatorPool>, PyOperatorPool>(
      k, "OperatorPool",
      "The OperatorPool interface provides methods for handling sets of "
      "operators.")
      .def(py::init<>(), "")
      .def("setOperators", (void (OperatorPool::*)(const FermionOperator)) &
                               OperatorPool::setOperators)
      .def("setOperators", (void (OperatorPool::*)(const PauliOperator)) &
                               OperatorPool::setOperators)
      .def("setOperators",
           (void (OperatorPool::*)(const std::shared_ptr<Observable>)) &
               OperatorPool::setOperators)
      .def("setOperators",
           (void (OperatorPool::*)(const std::vector<FermionOperator>)) &
               OperatorPool::setOperators)
      .def("setOperators",
           (void (OperatorPool::*)(const std::vector<PauliOperator>)) &
               OperatorPool::setOperators)
      .def("generate", &OperatorPool::generate)
      .def("operatorString", &OperatorPool::operatorString)
      .def("getOperatorInstructions", &OperatorPool::getOperatorInstructions)
      .def(
          "optionalParameters",
          [](OperatorPool &op, PyHeterogeneousMap &params) {
            HeterogeneousMap m;
            for (auto &item : params) {
              PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
              mpark::visit(vis, item.second);
            }
            return op.optionalParameters(m);
          },
          "");

  k.def("getGradientStrategy", [](const std::string &name) {
    return xacc::getService<xacc::AlgorithmGradientStrategy>(name);
  });
  py::class_<AlgorithmGradientStrategy,
             std::shared_ptr<AlgorithmGradientStrategy>,
             PyAlgorithmGradientStrategy>(
      k, "AlgorithmGradientStrategy",
      "The Optimizer interface provides optimization routine implementations "
      "for use in algorithms.")
      .def(py::init<>(), "")
      .def("initialize", &AlgorithmGradientStrategy::initialize)
      .def("isNumerical", &AlgorithmGradientStrategy::isNumerical)
      .def("setFunctionValue", &AlgorithmGradientStrategy::setFunctionValue)
      .def("getGradientExecutions",
           &AlgorithmGradientStrategy::getGradientExecutions)
      .def("compute", &AlgorithmGradientStrategy::compute);
}