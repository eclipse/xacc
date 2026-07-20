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

#include "AllGateVisitor.hpp"
#include "IRTransformation.hpp"
#include <Eigen/Dense>

namespace xacc {
namespace quantum {


using namespace Eigen;

// The following visitor is adapted from xacc AutodiffVisitor.

constexpr std::complex<double> I{0.0, 1.0};
// Note: cannot use std::complex_literal
// because https://gcc.gnu.org/onlinedocs/gcc/Complex.html#Complex
inline constexpr std::complex<double> operator"" _i(long double x) noexcept {
  return {0., static_cast<double>(x)};
}

MatrixXcd kroneckerProduct(MatrixXcd &lhs, MatrixXcd &rhs);
enum class Rot { X, Y, Z };

class KernelToUnitaryVisitor : public xacc::quantum::AllGateVisitor {
 public:
  KernelToUnitaryVisitor(size_t in_nbQubits);

  void visit(xacc::quantum::Hadamard &h) override;
  void visit(xacc::quantum::CNOT &cnot) override;
  void visit(xacc::quantum::Rz &rz) override;
  void visit(xacc::quantum::Ry &ry) override;
  void visit(xacc::quantum::Rx &rx) override;
  void visit(xacc::quantum::X &x) override;
  void visit(xacc::quantum::Y &y) override;
  void visit(xacc::quantum::Z &z) override;
  void visit(xacc::quantum::CY &cy) override;
  void visit(xacc::quantum::CZ &cz) override;
  void visit(xacc::quantum::Swap &s) override;
  void visit(xacc::quantum::CRZ &crz) override;
  void visit(xacc::quantum::CH &ch) override;
  void visit(xacc::quantum::S &s) override;
  void visit(xacc::quantum::Sdg &sdg) override;
  void visit(xacc::quantum::T &t) override;
  void visit(xacc::quantum::Tdg &tdg) override;
  void visit(xacc::quantum::CPhase &cphase) override;
  void visit(xacc::quantum::Measure &measure) override;
  void visit(xacc::quantum::Identity &i) override;
  void visit(xacc::quantum::U &u) override;
  void visit(xacc::quantum::IfStmt &ifStmt) override;
  // Identifiable Impl
  const std::string name() const override { return "xacc-to-unitary"; }
  const std::string description() const override { return ""; }
  MatrixXcd getMat() const;
  MatrixXcd singleQubitGateExpand(MatrixXcd &in_gateMat, size_t in_loc) const;

  MatrixXcd singleParametricQubitGateExpand(double in_var, Rot in_rotType,
                                            size_t in_bitLoc) const;

  MatrixXcd twoQubitGateExpand(MatrixXcd &in_gateMat, size_t in_bit1,
                               size_t in_bit2) const;

 private:
  MatrixXcd m_circuitMat;
  size_t m_nbQubit;
};

class QsearchOptimizer : public IRTransformation {
protected:

  std::shared_ptr<xacc::CompositeInstruction> circ_synth;
public:
  QsearchOptimizer();
  void apply(std::shared_ptr<CompositeInstruction> program,
             const std::shared_ptr<Accelerator> accelerator,
             const HeterogeneousMap &options = {}) override;
  const IRTransformationType type() const override {
    return IRTransformationType::Optimization;
  }

  const std::string name() const override { return "qsearch"; }
  const std::string description() const override { return ""; }
};
} // namespace quantum
} // namespace xacc
