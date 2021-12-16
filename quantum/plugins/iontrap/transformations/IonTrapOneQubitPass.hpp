/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *   Austin Adams - adapted for GTRI testbed
 *******************************************************************************/
#ifndef QUANTUM_ACCELERATORS_IONTRAPONEQUBITPASS_HPP_
#define QUANTUM_ACCELERATORS_IONTRAPONEQUBITPASS_HPP_

#include <armadillo>
#include "Accelerator.hpp"
#include "IRTransformation.hpp"
#include "IonTrapDecomp.hpp"
#include <Eigen/Dense>
#include "IonTrapPassesCommon.hpp"

namespace xacc {
namespace quantum {

// Based on MergeSingleQubitGatesOptimizer
class IonTrapOneQubitPass : public IRTransformation {
public:
    static constexpr double DEFAULT_THRESHOLD = 0.0001;

    IonTrapOneQubitPass() {}
    void apply(std::shared_ptr<CompositeInstruction> program,
               const std::shared_ptr<Accelerator> accelerator,
               const HeterogeneousMap &options = {}) override;

    const IRTransformationType type() const override {
        return IRTransformationType::Optimization;
    }
    const std::string name() const override { return "iontrap-1q-pass"; }
    const std::string description() const override { return ""; }

private:
    enum class GateSeqStart { CIRCUIT_START, TWO_QUBIT_GATE };
    enum class GateSeqTerm { MEASUREMENT, TWO_QUBIT_GATE, CIRCUIT_END };

    static void matrixMod(arma::mat &, double);

    double distanceFromIdentity(arma::cx_mat);
    std::vector<InstPtr> decompose(Decomp,
                                   const arma::cx_mat,
                                   double,
                                   std::size_t,
                                   std::string &,
                                   arma::cx_mat &);
    std::size_t numDistinctBuffers(const std::shared_ptr<CompositeInstruction>, std::string &);
    arma::mat runOptimizer(Decomp, arma::cx_mat, int, double &);
    std::vector<std::size_t> findSingleQubitGateSequence(const std::shared_ptr<CompositeInstruction>,
                                                         std::size_t, std::set<std::size_t> &,
                                                         GateSeqStart &, GateSeqTerm &);
    void nukeGates(std::shared_ptr<CompositeInstruction>,
                   const std::vector<std::size_t> &);
    std::size_t decomposeInPlace(Decomp,
                                 std::shared_ptr<CompositeInstruction>,
                                 const std::vector<std::size_t> &,
                                 std::map<std::size_t, arma::cx_mat> &,
                                 double,
                                 std::string &,
                                 IonTrapLogTransformCallback);
    std::size_t decomposeTracking(Decomp,
                                  std::shared_ptr<CompositeInstruction>,
                                  std::size_t,
                                  std::map<std::size_t, arma::cx_mat> &,
                                  double,
                                  std::string &,
                                  IonTrapLogTransformCallback);
};

} // namespace quantum
} // namespace xacc

#endif
