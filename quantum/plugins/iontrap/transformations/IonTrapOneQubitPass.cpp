/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *   Austin Adams - adapted for GTRI testbed
 *******************************************************************************/
#include <limits>
#include "xacc.hpp"
#include "xacc_service.hpp"
#include <ensmallen.hpp>
#include "IonTrapOneQubitPass.hpp"
#include "IonTrapOptFunction.hpp"
#include "Accelerator.hpp"
#include "CommonGates.hpp"
#include "GateFusion.hpp"

namespace xacc {
namespace quantum {

//
// Single-qubit decomposition
//

void IonTrapOneQubitPass::matrixMod(arma::mat &mat, double mod) {
    for (double &elem : mat) {
        elem = std::fmod(elem, mod);
    }
}

arma::mat IonTrapOneQubitPass::runOptimizer(Decomp decomp, arma::cx_mat goal, int rotations, double &obj_final) {
    if (rotations) {
        arma::mat angles(rotations, 1, arma::fill::randu);
        IonTrapOptFunction func(decomp, goal);
        ens::L_BFGS opt;
        opt.NumBasis() = 15;
        ens::StoreBestCoordinates<arma::mat> cb;

        opt.Optimize(func, angles, cb);
        obj_final = cb.BestObjective();
        matrixMod(angles, 2*M_PI);
        return angles;
    } else {
        // Special case: for no rotations, don't bother running the optimizer
        obj_final = distanceFromIdentity(goal);
        return {};
    }
}


std::vector<InstPtr> IonTrapOneQubitPass::decompose(Decomp decomp,
                                                 const arma::cx_mat goal,
                                                 double threshold,
                                                 std::size_t bitIdx,
                                                 std::string &bufferName,
                                                 arma::cx_mat &tracking_out) {
    static const int max_rotations = 4;
    int rotations = 0;
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");

    xacc::debug("Running " + decompName(decomp) + " optimizer...");

    // Save results in case we get poor results and need to backtrack
    std::vector<double> saved_obj;
    // Column X stores the angles found for using X rotations
    arma::mat saved_angles(max_rotations, max_rotations + 1, arma::fill::zeros);

    while (1) {
        if (decompShouldSkip(decomp, rotations)) {
            // Make sure we never use this number of rotations
            saved_obj.push_back(std::numeric_limits<double>::infinity());
            xacc::debug("Skipping " + decompName(decomp) + " optimizer with "
                        + std::to_string(rotations) + " rotations because there is "
                        "no optimizer for it. Adding a rotation and retrying...");
            rotations++;
            continue;
        }

        double obj_final;
        arma::mat angles = runOptimizer(decomp, goal, rotations, obj_final);

        if (rotations) {
            saved_angles.col(rotations).head(rotations) = angles.col(0);
        }
        saved_obj.push_back(obj_final);

        xacc::debug("Final obj function value with " + std::to_string(rotations)
                    + " rotations: " + std::to_string(obj_final));

        if (obj_final <= threshold) {
            break;
        } else if (rotations == max_rotations) {
            if (decomp == Decomp::Exact) {
                int min_idx = 0;
                for (int i = 1; i <= max_rotations; i++) {
                    if (saved_obj[i] < saved_obj[min_idx]) {
                        min_idx = i;
                    }
                }

                xacc::warning("Should never require more than 4 rotations to "
                              "find a unitary. Is the " + decompName(decomp) +
                              " optimizer broken? Falling back to " +
                              std::to_string(min_idx) + " rotations with "
                              "objective function value " +
                              std::to_string(saved_obj[min_idx]) + "...");
                if (xacc::verbose) {
                    std::stringstream ss;
                    ss << "Goal\n" << goal;
                    xacc::warning(ss.str());
                }
                rotations = min_idx;
                break;
            } else {
                Decomp nextToTry = decompNextToTry(decomp);
                xacc::warning("Should never require more than 4 rotations to "
                              "find a unitary. Is the " + decompName(decomp) +
                              " optimizer broken? Falling back to generating an " +
                              decompName(nextToTry) + " decomposition...");
                return decompose(nextToTry, goal, threshold,
                                 bitIdx, bufferName, tracking_out);
            }
        } else {
            rotations++;
        }
    }

    for (int i = 0; i < rotations; i++) {
        xacc::debug("rotation " + std::to_string(i) + ": " + std::to_string(saved_angles(i, rotations)));
    }

    std::size_t nonPirotAngles = decompFromRotationCount(decomp) + decompCount(decomp);
    std::vector<InstPtr> ops(std::max<int>(rotations - (int)nonPirotAngles, 0));
    for (int i = 0; i < ops.size(); i++) {
        double phi = saved_angles(i + (int)decompFromRotationCount(decomp), rotations);
        // On the IonTrap system, theta is always pi/2.
        // See: https://doi.org/10.1088/1367-2630/18/2/023048
        double theta = M_PI/2.0;
        InstPtr rot = gateRegistry->createInstruction("Rphi", {bitIdx}, {phi, theta});
        rot->setBufferNames({bufferName});
        ops[i] = rot;
    }

    if (rotations && decompCount(decomp) && decompShouldTrack(decomp)) {
        if (decomp == Decomp::RZ) {
            xacc::debug("discarding Rz(" + std::to_string(saved_angles(rotations-1, rotations))
                        + ") on qubit " + std::to_string(bitIdx));
        }
        // TODO: if decompCount(decomp) > 1, this will break
        tracking_out = decompMat(decomp, saved_angles(rotations-1, rotations));
    } else {
        tracking_out = arma::eye<arma::cx_mat>(2,2);
    }

    return ops;
}

double IonTrapOneQubitPass::distanceFromIdentity(arma::cx_mat mat) {
    double abs = std::abs(arma::trace(mat));
    return 4 - abs*abs;
}

void IonTrapOneQubitPass::nukeGates(std::shared_ptr<CompositeInstruction> program,
                                   const std::vector<std::size_t> &sequence) {
    for (const auto instIdx : sequence) {
        auto instrPtr = program->getInstruction(instIdx);
        instrPtr->disable();
    }
    program->removeDisabled();
}

std::size_t IonTrapOneQubitPass::decomposeTracking(Decomp decomp,
                                                std::shared_ptr<CompositeInstruction> program,
                                                std::size_t instIdx,
                                                std::map<std::size_t, arma::cx_mat> &tracking,
                                                double threshold,
                                                std::string &bufferName,
                                                IonTrapLogTransformCallback logTransCallback) {
    std::size_t totalDecomp = 0;
    std::vector<std::size_t> bits;
    if (instIdx >= program->nInstructions()) {
        // end of circuit
        for (auto const &kv : tracking) {
            bits.push_back(kv.first);
        }
    } else {
        // measure instruction etc
        InstPtr inst = program->getInstruction(instIdx);
        bits = inst->bits();
    }

    for (const auto bit : bits) {
        if (tracking.count(bit) && distanceFromIdentity(tracking[bit]) >= threshold) {
            std::vector<InstPtr> decomposed = decompose(decomp, tracking[bit],
                                                        threshold, bit, bufferName,
                                                        tracking[bit]);

            for (std::size_t i = 0; i < decomposed.size(); i++) {
                if (instIdx + i < program->nInstructions()) {
                    program->insertInstruction(instIdx + i, decomposed[i]);
                } else {
                    program->addInstruction(decomposed[i]);
                }
            }

            totalDecomp += decomposed.size();
        }
    }

    if (logTransCallback) {
        std::vector<InstPtr> newInsts(totalDecomp);
        for (std::size_t i = 0; i < totalDecomp; i++) {
            newInsts[i] = program->getInstruction(instIdx + i);
        }
        logTransCallback({}, newInsts);
    }

    return totalDecomp;
}

std::size_t IonTrapOneQubitPass::decomposeInPlace(Decomp decomp,
                                               std::shared_ptr<CompositeInstruction> program,
                                               const std::vector<std::size_t> &sequence,
                                               std::map<std::size_t, arma::cx_mat> &tracking,
                                               double threshold,
                                               std::string &bufferName,
                                               IonTrapLogTransformCallback logTransCallback) {
    auto gateRegistry = xacc::getService<xacc::IRProvider>("quantum");
    auto tmpKernel = gateRegistry->createComposite("__TMP__");
    for (const auto instIdx: sequence) {
        auto instrPtr = program->getInstruction(instIdx)->clone();
        assert(instrPtr->bits().size() == 1);
        // Remap to bit 0 for fusing
        instrPtr->setBits({ 0 });
        tmpKernel->addInstruction(instrPtr);
    }

    Eigen::Matrix2cd uMat = GateFuser::fuseGates(tmpKernel, 1);
    const arma::cx_mat unitary = arma::cx_mat(uMat.data(), uMat.rows(), uMat.cols());
    const std::size_t bitIdx = program->getInstruction(sequence[0])->bits()[0];

    // Disable existing instructions (so we can remove them at the end
    // of this method)
    for (const auto instIdx : sequence) {
        auto instrPtr = program->getInstruction(instIdx);
        instrPtr->disable();
    }

    if (!tracking.count(bitIdx)) {
        tracking[bitIdx] = arma::eye<arma::cx_mat>(2,2);
    }

    std::vector<InstPtr> decomposed = decompose(decomp, unitary * tracking[bitIdx],
                                                threshold, bitIdx, bufferName,
                                                tracking[bitIdx]);
    auto locationToInsert = sequence[0];
    for (auto& newInst : decomposed) {
        program->insertInstruction(locationToInsert, newInst);
        locationToInsert++;
    }

    if (logTransCallback) {
        std::vector<InstPtr> oldInsts(sequence.size());
        for (std::size_t i = 0; i < sequence.size(); i++) {
            oldInsts[i] = program->getInstruction(sequence[i] + decomposed.size());
        }
        std::vector<InstPtr> newInsts(decomposed.size());
        for (std::size_t i = 0; i < decomposed.size(); i++) {
            newInsts[i] = program->getInstruction(sequence[0] + i);
        }

        logTransCallback(oldInsts, newInsts);
    }

    program->removeDisabled();

    return decomposed.size();
}

// Yoinked from MergeSingleQubitGatesOptimizer
// Check if the composite uses a single qubit register,
// the qubit line is determined by the qubit idx,
// hence we don't want to accidentally merge wrong gates.
std::size_t IonTrapOneQubitPass::numDistinctBuffers(const std::shared_ptr<CompositeInstruction> program, std::string &bufferNameOut) {
    std::set<std::string> allBuffers;
    InstructionIterator it(program);
    while (it.hasNext()) {
        auto nextInst = it.next();
        for (const auto &bufferName : nextInst->getBufferNames()) {
            allBuffers.emplace(bufferName);
        }
    }

    if (!allBuffers.empty()) {
        bufferNameOut = *allBuffers.begin();
    }

    return allBuffers.size();
}

std::vector<std::size_t> IonTrapOneQubitPass::findSingleQubitGateSequence(const std::shared_ptr<CompositeInstruction> program, size_t startIdx, std::set<std::size_t> &qubitsSeen, GateSeqStart &start, GateSeqTerm &term) {
    const auto nInstructions = program->nInstructions();

    if (startIdx >= nInstructions) {
        // TODO: set start to some sentinel value instead? this is a little misleading
        //       since we don't have enough information to actually say
        start = GateSeqStart::TWO_QUBIT_GATE;
        term = GateSeqTerm::CIRCUIT_END;
        return {};
    }

    auto firstInst = program->getInstruction(startIdx);

    auto bits = firstInst->bits();
    start = std::any_of(bits.begin(), bits.end(), [&](std::size_t idx) {
                return qubitsSeen.count(idx);
            }) ? GateSeqStart::TWO_QUBIT_GATE : GateSeqStart::CIRCUIT_START;
    //qubitsSeen.insert(firstInst->bits().begin(), firstInst->bits().end());
    qubitsSeen.insert(bits.begin(), bits.end());

    // Not a single-qubit gate.
    if (bits.size() > 1) {
        term = GateSeqTerm::TWO_QUBIT_GATE;
        return {};
    } else if (firstInst->name() == "Measure") {
        term = GateSeqTerm::MEASUREMENT;
        return {};
    }

    const std::size_t bitIdx = firstInst->bits()[0];
    std::vector<std::size_t> gateSequence;
    gateSequence.emplace_back(startIdx);

    for (size_t instIdx = startIdx + 1; instIdx < nInstructions; instIdx++) {
        auto instPtr = program->getInstruction(instIdx);

        if (instPtr->bits().size() == 1 && instPtr->bits()[0] == bitIdx) {
            if (instPtr->name() != "Measure") {
                gateSequence.emplace_back(instIdx);
            } else {
                term = GateSeqTerm::MEASUREMENT;
                return gateSequence;
            }
        // If this is a two-qubit gate that involves this qubit wire,
        // terminate the scan and return.
        } else if (instPtr->bits().size() == 2 && xacc::container::contains(instPtr->bits(), bitIdx)) {
            term = GateSeqTerm::TWO_QUBIT_GATE;
            return gateSequence;
        }
    }

    // Reach the end of the circuit:
    term = GateSeqTerm::CIRCUIT_END;
    return gateSequence;
}

void IonTrapOneQubitPass::apply(std::shared_ptr<CompositeInstruction> program, const std::shared_ptr<Accelerator> accelerator, const HeterogeneousMap &options) {
    IonTrapLogTransformCallback logTransCallback = nullptr;
    if (options.keyExists<IonTrapLogTransformCallback>("log-trans-cb")) {
        logTransCallback = options.get<IonTrapLogTransformCallback>("log-trans-cb");
    }
    bool keepTrailingGates = options.keyExists<bool>("keep-trailing-gates")
                             && options.get<bool>("keep-trailing-gates");
    bool keepRzBeforeMeas = options.keyExists<bool>("keep-rz-before-meas")
                            && options.get<bool>("keep-rz-before-meas");
    bool keepRxBeforeXX = options.keyExists<bool>("keep-rx-before-xx")
                          && options.get<bool>("keep-rx-before-xx");
    bool keepLeadingRz = options.keyExists<bool>("keep-leading-rz")
                         && options.get<bool>("keep-leading-rz");
    double threshold = DEFAULT_THRESHOLD;
    if (options.keyExists<double>("threshold")) {
        threshold = options.get<double>("threshold");
    }

    std::string bufferName;
    if (numDistinctBuffers(program, bufferName) > 1) {
        // If there are multiple buffers, we cannot apply gate merging
        // due to qubit Id ambiguity.
        xacc::error("Cannot run single qubit merge optimizer on a program with multiple buffers!");
        return;
    }

    iontrapFlattenComposite(program);
    std::set<std::size_t> qubitsSeen;
    std::map<std::size_t, arma::cx_mat> tracking;

    for (std::size_t instIdx = 0; instIdx <= program->nInstructions();) {
        GateSeqStart start;
        GateSeqTerm term;
        const auto sequence = findSingleQubitGateSequence(program, instIdx, qubitsSeen, start, term);

        std::size_t decompSize;
        if (sequence.empty()) {
            // We don't need to consider GateSeqStart here because decomposeTracking() only
            // considers qubits with tracking set. But tracking is set only when a
            // one-qubit or two-qubit gate has been applied to a qubit, which means it may
            // not be in state |0>.
            Decomp decomp = keepRzBeforeMeas? Decomp::Exact : Decomp::RZ;

            switch (term) {
                case GateSeqTerm::CIRCUIT_END:
                    if (keepTrailingGates) {
                        decompSize = decomposeTracking(decomp, program, instIdx, tracking, threshold, bufferName, logTransCallback);
                    } else {
                        decompSize = 0;
                    }
                    break;
                case GateSeqTerm::TWO_QUBIT_GATE:
                    // This Rx(theta) still commutes past this XX gate, nothing to do
                    decompSize = 0;
                    break;
                case GateSeqTerm::MEASUREMENT:
                    decompSize = decomposeTracking(decomp, program, instIdx, tracking, threshold, bufferName, logTransCallback);
                    break;
            }

            // +1 because we always want to skip whatever caused
            // findSingleQubitGateSequence() to return an empty list
            instIdx += decompSize + 1;
        } else {
            Decomp decompBeforeMeas, decompBeforeXX;
            if (start == GateSeqStart::CIRCUIT_START && !keepLeadingRz) {
                // Can safely extract leading Rz(theta) since this qubit is |0>
                decompBeforeMeas = keepRzBeforeMeas ? Decomp::RZExact : Decomp::RZRZ;
                decompBeforeXX = keepRxBeforeXX ? Decomp::RZExact : Decomp::RZRX;
            } else {
                decompBeforeMeas = keepRzBeforeMeas ? Decomp::Exact : Decomp::RZ;
                decompBeforeXX = keepRxBeforeXX ? Decomp::Exact : Decomp::RX;
            }

            switch (term) {
                case GateSeqTerm::CIRCUIT_END:
                    if (keepTrailingGates) {
                        decompSize = decomposeInPlace(decompBeforeMeas, program, sequence, tracking, threshold, bufferName, logTransCallback);
                    } else {
                        // If there was no measurement, no need to apply tracking
                        // or these gates
                        nukeGates(program, sequence);
                        decompSize = 0;
                    }
                    break;
                case GateSeqTerm::TWO_QUBIT_GATE:
                    decompSize = decomposeInPlace(decompBeforeXX, program, sequence, tracking, threshold, bufferName, logTransCallback);
                    break;
                case GateSeqTerm::MEASUREMENT:
                    decompSize = decomposeInPlace(decompBeforeMeas, program, sequence, tracking, threshold, bufferName, logTransCallback);
                    break;
            }

            instIdx += decompSize;
        }
    }
}

} // namespace quantum
} // namespace xacc
