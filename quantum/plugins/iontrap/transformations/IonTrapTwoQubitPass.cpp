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
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "IonTrapTwoQubitPass.hpp"
#include "Accelerator.hpp"

namespace xacc {
namespace quantum {

//
// Two-qubit decompositions
//

std::pair<double, double> IonTrapTwoQubitPass::findMSPhases(IonTrapMSPhaseMap *msPhases, InstPtr cnot) {
    std::size_t leftIdx = std::min(cnot->bits()[0], cnot->bits()[1]);
    std::size_t rightIdx = std::max(cnot->bits()[0], cnot->bits()[1]);
    auto idxPair = std::make_pair(leftIdx, rightIdx);

    if (!msPhases->count(idxPair)) {
        xacc::error("pair " + std::to_string(leftIdx) + "," + std::to_string(rightIdx)
                    + " missing from set of MS phases");
    }
    auto phasePair = (*msPhases)[idxPair];
    double controlMSPhase = (leftIdx == cnot->bits()[0])? phasePair.first : phasePair.second;
    double targetMSPhase = (rightIdx == cnot->bits()[1])? phasePair.second : phasePair.first;
    return std::make_pair(controlMSPhase, targetMSPhase);
}

void IonTrapTwoQubitPass::apply(std::shared_ptr<CompositeInstruction> program,
                              const std::shared_ptr<Accelerator> accelerator,
                              const HeterogeneousMap &options) {
    if (!options.keyExists<IonTrapMSPhaseMap*>("ms-phases")) {
        xacc::error("IonTrapTwoQubitPass requires the ms-phases option");
        return;
    }

    IonTrapMSPhaseMap *msPhases = options.get<IonTrapMSPhaseMap*>("ms-phases");
    IonTrapLogTransformCallback logTransCallback = nullptr;

    if (options.keyExists<IonTrapLogTransformCallback>("log-trans-cb")) {
        logTransCallback = options.get<IonTrapLogTransformCallback>("log-trans-cb");
    }

    auto gateRegistry = xacc::getService<IRProvider>("quantum");

    iontrapFlattenComposite(program);

    for (std::size_t instIdx = 0; instIdx < program->nInstructions();) {
        InstPtr inst = program->getInstruction(instIdx);
        if (!inst->isEnabled()) {
            instIdx++;
            continue;
        }

        if (inst->name() == "CNOT") {
            auto [controlMSPhase, targetMSPhase] = findMSPhases(msPhases, inst);
            InstPtr ry1 = gateRegistry->createInstruction("Ry", {inst->bits()[0]}, {-M_PI/2.0});
            InstPtr xx = gateRegistry->createInstruction("XX", inst->bits(), {M_PI/4.0});
            InstPtr ry2 = gateRegistry->createInstruction("Ry", {inst->bits()[0]}, {M_PI/2.0});
            InstPtr rz = gateRegistry->createInstruction("Rz", {inst->bits()[0]}, {M_PI/2.0});
            InstPtr rx = gateRegistry->createInstruction("Rx", {inst->bits()[1]}, {M_PI/2.0});

            std::size_t i = instIdx;
            program->insertInstruction(i++, ry1);
            // TODO: Note that this is kind of incorrect: really, the combination of these Rz gates
            //       and an MS gate is actually an XX gate (see https://doi.org/10.1088/1367-2630/18/2/023048)
            //       but we are surrounding an XX instruction with Rz instructions. But this will
            //       work for now
            if (controlMSPhase) {
                InstPtr msRz1 = gateRegistry->createInstruction("Rz", {inst->bits()[0]}, {controlMSPhase});
                program->insertInstruction(i++, msRz1);
            }
            if (targetMSPhase) {
                InstPtr msRz2 = gateRegistry->createInstruction("Rz", {inst->bits()[1]}, {targetMSPhase});
                program->insertInstruction(i++, msRz2);
            }
            program->insertInstruction(i++, xx);
            if (controlMSPhase) {
                InstPtr msRz3 = gateRegistry->createInstruction("Rz", {inst->bits()[0]}, {-controlMSPhase});
                program->insertInstruction(i++, msRz3);
            }
            if (targetMSPhase) {
                InstPtr msRz4 = gateRegistry->createInstruction("Rz", {inst->bits()[1]}, {-targetMSPhase});
                program->insertInstruction(i++, msRz4);
            }
            program->insertInstruction(i++, ry2);
            program->insertInstruction(i++, rz);
            program->insertInstruction(i++, rx);

            if (logTransCallback) {
                std::vector<InstPtr> newInsts;
                for (std::size_t j = instIdx; j < i; j++) {
                    newInsts.push_back(program->getInstruction(j));
                }
                logTransCallback({inst}, newInsts);
            }
        } else if (inst->name() == "CH") {
            InstPtr s = gateRegistry->createInstruction("S", {inst->bits()[1]});
            InstPtr h = gateRegistry->createInstruction("H", {inst->bits()[1]});
            InstPtr t = gateRegistry->createInstruction("T", {inst->bits()[1]});
            InstPtr cx = gateRegistry->createInstruction("CNOT", inst->bits());
            InstPtr tdg = gateRegistry->createInstruction("Tdg", {inst->bits()[1]});
            InstPtr h2 = gateRegistry->createInstruction("H", {inst->bits()[1]});
            InstPtr sdg = gateRegistry->createInstruction("Sdg", {inst->bits()[1]});

            program->insertInstruction(instIdx, s);
            program->insertInstruction(instIdx+1, h);
            program->insertInstruction(instIdx+2, t);
            program->insertInstruction(instIdx+3, cx);
            program->insertInstruction(instIdx+4, tdg);
            program->insertInstruction(instIdx+5, h2);
            program->insertInstruction(instIdx+6, sdg);

            if (logTransCallback) {
                logTransCallback({program->getInstruction(instIdx+7)},
                                 {program->getInstruction(instIdx),
                                  program->getInstruction(instIdx+1),
                                  program->getInstruction(instIdx+2),
                                  program->getInstruction(instIdx+3),
                                  program->getInstruction(instIdx+4),
                                  program->getInstruction(instIdx+5),
                                  program->getInstruction(instIdx+6)});
            }
        } else if (inst->name() == "CY") {
            InstPtr sdg = gateRegistry->createInstruction("Sdg", {inst->bits()[1]});
            InstPtr cx = gateRegistry->createInstruction("CNOT", inst->bits());
            InstPtr s = gateRegistry->createInstruction("S", {inst->bits()[1]});

            program->insertInstruction(instIdx, sdg);
            program->insertInstruction(instIdx+1, cx);
            program->insertInstruction(instIdx+2, s);

            if (logTransCallback) {
                logTransCallback({program->getInstruction(instIdx+3)},
                                 {program->getInstruction(instIdx),
                                  program->getInstruction(instIdx+1),
                                  program->getInstruction(instIdx+2)});
            }
        } else if (inst->name() == "CZ") {
            InstPtr h = gateRegistry->createInstruction("H", {inst->bits()[1]});
            InstPtr cx = gateRegistry->createInstruction("CNOT", inst->bits());
            InstPtr h2 = gateRegistry->createInstruction("H", {inst->bits()[1]});

            program->insertInstruction(instIdx, h);
            program->insertInstruction(instIdx+1, cx);
            program->insertInstruction(instIdx+2, h2);

            if (logTransCallback) {
                logTransCallback({program->getInstruction(instIdx+3)},
                                 {program->getInstruction(instIdx),
                                  program->getInstruction(instIdx+1),
                                  program->getInstruction(instIdx+2)});
            }
        } else if (inst->name() == "Swap") {
            InstPtr cx1 = gateRegistry->createInstruction("CNOT", inst->bits());
            InstPtr cx2 = gateRegistry->createInstruction("CNOT", {inst->bits()[1], inst->bits()[0]});
            InstPtr cx3 = gateRegistry->createInstruction("CNOT", inst->bits());

            program->insertInstruction(instIdx, cx1);
            program->insertInstruction(instIdx+1, cx2);
            program->insertInstruction(instIdx+2, cx3);

            if (logTransCallback) {
                logTransCallback({program->getInstruction(instIdx+3)},
                                 {program->getInstruction(instIdx),
                                  program->getInstruction(instIdx+1),
                                  program->getInstruction(instIdx+2)});
            }
        } else {
            instIdx++;
            continue;
        }

        inst->disable();
        // Do not increment instIdx so we can examine any new
        // instructions we've inserted at instIdx
    }

    program->removeDisabled();
}

} // namespace quantum
} // namespace xacc
