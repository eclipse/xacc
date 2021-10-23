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
#ifndef QUANTUM_ACCELERATORS_IONTRAPTWOQUBITPASS_HPP_
#define QUANTUM_ACCELERATORS_IONTRAPTWOQUBITPASS_HPP_

#include "Accelerator.hpp"
#include "IRTransformation.hpp"
#include "IonTrapPassesCommon.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {

typedef std::map<std::pair<std::size_t, std::size_t>, std::pair<double, double>> IonTrapMSPhaseMap;

class IonTrapTwoQubitPass : public IRTransformation {
public:
    IonTrapTwoQubitPass() {}
    void apply(std::shared_ptr<CompositeInstruction> program,
               const std::shared_ptr<Accelerator> accelerator,
               const HeterogeneousMap &options = {}) override;

    const IRTransformationType type() const override {
        return IRTransformationType::Optimization;
    }
    const std::string name() const override { return "iontrap-2q-pass"; }
    const std::string description() const override { return ""; }

private:
    std::pair<double, double> findMSPhases(IonTrapMSPhaseMap *, InstPtr);
};

} // namespace quantum
} // namespace xacc

#endif
