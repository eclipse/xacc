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
 *   Austin Adams - initial API and implementation
 *******************************************************************************/
#include "InstructionIterator.hpp"
#include "IonTrapPassesCommon.hpp"

namespace xacc {
namespace quantum {

// Yoinked from MergeSingleQubitGatesOptimizer, except does not clone()
// instructions
void iontrapFlattenComposite(std::shared_ptr<CompositeInstruction> composite) {
    std::vector<xacc::InstPtr> flatten;
    InstructionIterator iter(composite);
    while (iter.hasNext()) {
        auto inst = iter.next();
        if (!inst->isComposite()) {
            // Previously, inst was clone()d here
            flatten.emplace_back(inst);
        }
    }
    composite->clear();
    composite->addInstructions(flatten);
}

}
}
