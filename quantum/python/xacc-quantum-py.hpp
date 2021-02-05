/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Daniel Claudino - initial API and implementation
 *******************************************************************************/
#ifndef XACC_QUANTUM_BINDINGS_HPP_
#define XACC_QUANTUM_BINDINGS_HPP_

#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/complex.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/eigen.h>
#include <pybind11/iostream.h>
#include <pybind11/operators.h>
#include <pybind11/functional.h>
#include <pybind11/embed.h>

#include "xacc.hpp"
#include "FermionOperator.hpp"
#include "PauliOperator.hpp"
#include "OperatorPool.hpp"
#include "AlgorithmGradientStrategy.hpp"
#include "AllGateVisitor.hpp"

namespace py = pybind11;
using namespace xacc;
using namespace xacc::quantum;

class PyOperatorPool : public OperatorPool {
public:
  /* Inherit the constructors */
  using OperatorPool::OperatorPool;

  void setOperators(const FermionOperator op) override {
    PYBIND11_OVERLOAD(void, xacc::quantum::OperatorPool, setOperators, op);
  }

  void setOperators(const PauliOperator op) override {
    PYBIND11_OVERLOAD(void, xacc::quantum::OperatorPool, setOperators, op);
  }

  void setOperators(const std::shared_ptr<Observable> op) override {
    PYBIND11_OVERLOAD(void, xacc::quantum::OperatorPool, setOperators, op);
  }

  void setOperators(const std::vector<FermionOperator> op) override {
    PYBIND11_OVERLOAD(void, xacc::quantum::OperatorPool, setOperators, op);
  }
  
  void setOperators(const std::vector<PauliOperator> op) override {
    PYBIND11_OVERLOAD(void, xacc::quantum::OperatorPool, setOperators, op);
  }

  const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::quantum::OperatorPool, name);
  }

  const std::string description() const override { return ""; }

  bool optionalParameters(const HeterogeneousMap parameters) override {
    PYBIND11_OVERLOAD_PURE(bool, xacc::quantum::OperatorPool, optionalParameters, parameters);
  }

  std::vector<std::shared_ptr<xacc::Observable>>
  generate(const int &nQubits) override {
    PYBIND11_OVERLOAD_PURE(std::vector<std::shared_ptr<xacc::Observable>>, xacc::quantum::OperatorPool, generate, nQubits);
  }

  std::string operatorString(const int index) override{
    PYBIND11_OVERLOAD_PURE(std::string, xacc::quantum::OperatorPool, operatorString, index);
  }

  std::shared_ptr<CompositeInstruction> getOperatorInstructions(
    const int opIdx, const int varIdx) const override {
    PYBIND11_OVERLOAD_PURE(std::shared_ptr<CompositeInstruction>, xacc::quantum::OperatorPool, getOperatorInstructions, opIdx, varIdx);
  }

};

class PyAlgorithmGradientStrategy : public AlgorithmGradientStrategy {
public:
  /* Inherit the constructors */
  using AlgorithmGradientStrategy::AlgorithmGradientStrategy;

  const std::string name() const override {
    PYBIND11_OVERLOAD_PURE(const std::string, xacc::AlgorithmGradientStrategy, name);
  }
  const std::string description() const override { return ""; }

  bool isNumerical() const override {
    PYBIND11_OVERLOAD_PURE(bool, xacc::AlgorithmGradientStrategy, isNumerical);
  }

  bool initialize(const HeterogeneousMap parameters) override {
    PYBIND11_OVERLOAD_PURE(bool, xacc::AlgorithmGradientStrategy, initialize, parameters);
  }

  std::vector<std::shared_ptr<CompositeInstruction>> getGradientExecutions(
    std::shared_ptr<CompositeInstruction> circuit, const std::vector<double> &x) override {
    PYBIND11_OVERLOAD_PURE(std::vector<std::shared_ptr<CompositeInstruction>>, xacc::AlgorithmGradientStrategy, getGradientExecutions, circuit, x);
  }

  void compute(std::vector<double> &dx,
    std::vector<std::shared_ptr<AcceleratorBuffer>> results) override {
    PYBIND11_OVERLOAD_PURE(void, xacc::AlgorithmGradientStrategy, compute, dx, results);        
  }

  void setFunctionValue(const double expValue) override {
    PYBIND11_OVERLOAD(void, xacc::AlgorithmGradientStrategy, setFunctionValue, expValue);        
  }

};

#define QUICK_WRITE_VISIT_METHOD(GATE) void visit(GATE& gate) override { PYBIND11_OVERLOAD(void, AllGateVisitor, visit, gate); }
class PyAllGateVisitor : public AllGateVisitor {
  using AllGateVisitor::AllGateVisitor;
  QUICK_WRITE_VISIT_METHOD(Hadamard)
  QUICK_WRITE_VISIT_METHOD(CNOT)   
  QUICK_WRITE_VISIT_METHOD(Rz)
  QUICK_WRITE_VISIT_METHOD(Rx)
  QUICK_WRITE_VISIT_METHOD(Ry)
  QUICK_WRITE_VISIT_METHOD(X)
  QUICK_WRITE_VISIT_METHOD(Y)
  QUICK_WRITE_VISIT_METHOD(Z)
  QUICK_WRITE_VISIT_METHOD(CPhase)
  QUICK_WRITE_VISIT_METHOD(Swap)
  QUICK_WRITE_VISIT_METHOD(iSwap)
  QUICK_WRITE_VISIT_METHOD(fSim)
  QUICK_WRITE_VISIT_METHOD(Measure)
  QUICK_WRITE_VISIT_METHOD(Identity)
  QUICK_WRITE_VISIT_METHOD(CZ)
  QUICK_WRITE_VISIT_METHOD(CY)
  QUICK_WRITE_VISIT_METHOD(CRZ)
  QUICK_WRITE_VISIT_METHOD(CH)
  QUICK_WRITE_VISIT_METHOD(S)
  QUICK_WRITE_VISIT_METHOD(Sdg)  
  QUICK_WRITE_VISIT_METHOD(T)
  QUICK_WRITE_VISIT_METHOD(Tdg)
  QUICK_WRITE_VISIT_METHOD(U)
  QUICK_WRITE_VISIT_METHOD(U1)
  QUICK_WRITE_VISIT_METHOD(XY)
};

void bind_quantum(py::module& m);

#endif