/*******************************************************************************
 * Copyright (c) 2022 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   
 *   
 *******************************************************************************/
#ifndef QUANTUM_ACCELERATORS_IONTRAPTWOQUBITPASSVISITOR_HPP_
#define QUANTUM_ACCELERATORS_IONTRAPTWOQUBITPASSVISITOR_HPP_

#include "Accelerator.hpp"
#include "IRTransformation.hpp"
#include "IonTrapPassesCommon.hpp"
#include "AllGateVisitor.hpp"
#include "IonTrapTwoQubitPass.hpp"

using namespace xacc;

namespace xacc {
namespace quantum {

class IonTrapTwoQubitPassVisitor : public xacc::quantum::AllGateVisitor {
public:
    
    IonTrapTwoQubitPassVisitor();
    void visit(CNOT& cnot) override;
    void visit(CH& ch) override;
    void visit(CY& cy) override;
    void visit(CZ& cz) override;
    void visit(Swap& Swap) override;

    void initializeInstructionVisitor(std::size_t instructionIndex);

    bool instructionVisited();

    HeterogeneousMap _paramsMap;

private:
    std::pair<double, double> findMSPhases(IonTrapMSPhaseMap *, Instruction *);

    std::size_t _instIdx;
    bool   _bInstructionVisited; 
    
};

} // namespace quantum
} // namespace xacc

#endif
