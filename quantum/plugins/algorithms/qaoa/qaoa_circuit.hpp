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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#pragma once

#include "Circuit.hpp"
#include "IRProvider.hpp"

namespace xacc {
namespace circuits {
// Generate QAOA ansatz for VQE
class QAOA : public xacc::quantum::Circuit {
public:
    QAOA() : Circuit("qaoa") { 
        // Runtime arguments
        arguments.push_back(std::make_shared<xacc::CompositeArgument>("nbQubits", "int"));
        arguments.push_back(std::make_shared<xacc::CompositeArgument>("gamma", "std::vector<double>"));
        arguments.push_back(std::make_shared<xacc::CompositeArgument>("beta", "std::vector<double>"));
        arguments.push_back(std::make_shared<xacc::CompositeArgument>("cost", "Observable*"));
        arguments.push_back(std::make_shared<xacc::CompositeArgument>("ref", "Observable*"));
    }

    bool expand(const xacc::HeterogeneousMap &runtimeOptions) override;
    const std::vector<std::string> requiredKeys() override;
    void applyRuntimeArguments() override;
    DEFINE_CLONE(QAOA);

private:
    std::shared_ptr<CompositeInstruction> constructParameterizedKernel() const;
private:
    size_t m_nbQubits;
    size_t m_nbSteps;
    std::vector<std::string> m_refHam;
    std::vector<std::string> m_costHam;
};
} // namespace circuits
} // namespace xacc
