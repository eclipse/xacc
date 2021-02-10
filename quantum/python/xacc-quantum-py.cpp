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

#define EXPOSE_PYGATE(GATE) py::class_<GATE, Gate, std::shared_ptr<GATE>>(k, #GATE);
#define EXPOSE_PY_VISIT_METHOD(GATE) .def("visit", (void (AllGateVisitor::*)(GATE& )) &AllGateVisitor::visit)

void bind_quantum(py::module &m) {
  py::module k = m.def_submodule("quantum", "XACC python quantum submodule");
  py::class_<Term>(k, "Term").def("coeff", &Term::coeff).def("ops", &Term::ops);

  py::class_<Gate, Instruction, std::shared_ptr<Gate>>(k, "Gate");
  EXPOSE_PYGATE(Hadamard)
  EXPOSE_PYGATE(CNOT)   
  EXPOSE_PYGATE(Rz)
  EXPOSE_PYGATE(Rx)
  EXPOSE_PYGATE(Ry)
  EXPOSE_PYGATE(X)
  EXPOSE_PYGATE(Y)
  EXPOSE_PYGATE(Z)
  EXPOSE_PYGATE(CPhase)
  EXPOSE_PYGATE(Swap)
  EXPOSE_PYGATE(iSwap)
  EXPOSE_PYGATE(fSim)
  EXPOSE_PYGATE(Measure)
  EXPOSE_PYGATE(Identity)
  EXPOSE_PYGATE(CZ)
  EXPOSE_PYGATE(CY)
  EXPOSE_PYGATE(CRZ)
  EXPOSE_PYGATE(CH)
  EXPOSE_PYGATE(S)
  EXPOSE_PYGATE(Sdg)  
  EXPOSE_PYGATE(T)
  EXPOSE_PYGATE(Tdg)
  EXPOSE_PYGATE(U)
  EXPOSE_PYGATE(U1)
  EXPOSE_PYGATE(XY)

  py::class_<AllGateVisitor, std::shared_ptr<AllGateVisitor>, PyAllGateVisitor, xacc::BaseInstructionVisitor>(k, "AllGateVisitor")
    .def(py::init<>(), "")
  EXPOSE_PY_VISIT_METHOD(Hadamard)
  EXPOSE_PY_VISIT_METHOD(CNOT)
  EXPOSE_PY_VISIT_METHOD(Rz)
  EXPOSE_PY_VISIT_METHOD(Rx)
  EXPOSE_PY_VISIT_METHOD(Ry)
  EXPOSE_PY_VISIT_METHOD(X)
  EXPOSE_PY_VISIT_METHOD(Y)
  EXPOSE_PY_VISIT_METHOD(Z)
  EXPOSE_PY_VISIT_METHOD(CPhase)
  EXPOSE_PY_VISIT_METHOD(Swap)
  EXPOSE_PY_VISIT_METHOD(iSwap)
  EXPOSE_PY_VISIT_METHOD(fSim)
  EXPOSE_PY_VISIT_METHOD(Measure)
  EXPOSE_PY_VISIT_METHOD(Identity)
  EXPOSE_PY_VISIT_METHOD(CZ)
  EXPOSE_PY_VISIT_METHOD(CY)
  EXPOSE_PY_VISIT_METHOD(CRZ)
  EXPOSE_PY_VISIT_METHOD(CH)
  EXPOSE_PY_VISIT_METHOD(S)
  EXPOSE_PY_VISIT_METHOD(Sdg)  
  EXPOSE_PY_VISIT_METHOD(T)
  EXPOSE_PY_VISIT_METHOD(Tdg)
  EXPOSE_PY_VISIT_METHOD(U)
  EXPOSE_PY_VISIT_METHOD(U1)
  EXPOSE_PY_VISIT_METHOD(XY);

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
      .def("__sub__",
           [](const PauliOperator &op, double idCoeff) { return op - idCoeff; })
      .def("__rsub__",
           [](const PauliOperator &op, double idCoeff) { return idCoeff - op; })
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
      .def("getNonIdentitySubTerms", &PauliOperator::getNonIdentitySubTerms)
      .def("getIdentitySubTerm", &PauliOperator::getIdentitySubTerm)      
      .def("to_numpy", [](PauliOperator& op) {
           auto mat_el = op.to_sparse_matrix();
           auto size = std::pow(2, op.nBits());
           Eigen::MatrixXcd mat = Eigen::MatrixXcd::Zero(size, size);
           for (auto el : mat_el) {
                mat(el.row(), el.col()) = el.coeff();
           }
           return mat;
      })
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
      .def("initialize", [](AlgorithmGradientStrategy &a, PyHeterogeneousMap &params) {
            HeterogeneousMap m;
            for (auto &item : params) {
              PyHeterogeneousMap2HeterogeneousMap vis(m, item.first);
              mpark::visit(vis, item.second);
            }
            return a.initialize(m);
          })
      .def("isNumerical", &AlgorithmGradientStrategy::isNumerical)
      .def("setFunctionValue", &AlgorithmGradientStrategy::setFunctionValue)
      .def("getGradientExecutions",
           &AlgorithmGradientStrategy::getGradientExecutions)
      .def("compute", &AlgorithmGradientStrategy::compute);
}