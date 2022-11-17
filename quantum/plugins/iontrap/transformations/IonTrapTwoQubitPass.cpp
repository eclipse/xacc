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
#include "IonTrapTwoQubitPassVisitor.hpp"

namespace xacc {
namespace quantum {

//
// Two-qubit decompositions
//

IonTrapTwoQubitPass::IonTrapTwoQubitPass()
{
}

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

    auto _twoQubitPassVisitor = std::make_shared<IonTrapTwoQubitPassVisitor>();

    iontrapFlattenComposite(program);

    HeterogeneousMap paramsMap{std::make_pair("composite", program),
                               std::make_pair("options", options)};

    _twoQubitPassVisitor->_paramsMap = paramsMap;

    for (std::size_t instIdx = 0; instIdx < program->nInstructions();) {
        InstPtr inst = program->getInstruction(instIdx);
        if (!inst->isEnabled()) {
            instIdx++;
            continue;
        }

        _twoQubitPassVisitor->initializeInstructionVisitor(instIdx);

        inst->attachMetadata({{"composite", program},
                              {"options", options}});
        
        //std::cout << "instruction index is " << instIdx << ", name is " << inst->name() << std::endl;
        inst->accept(_twoQubitPassVisitor);    
        //std::cout << "instruction index is " << instIdx << ", name is " << inst->name() << std::endl;

        if (!_twoQubitPassVisitor->instructionVisited())
        {
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
