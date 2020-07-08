/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
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

#include "QuantumNaturalGradient.hpp"

using namespace xacc;

namespace xacc {
namespace algorithm {
bool QuantumNaturalGradient::optionalParameters(const HeterogeneousMap in_parameters)
{
    // TODO: 
    return true;
}

std::vector<std::shared_ptr<CompositeInstruction>> QuantumNaturalGradient::getGradientExecutions(std::shared_ptr<CompositeInstruction> in_circuit, const std::vector<double>& in_x) 
{
    // TODO:
    return { in_circuit };
}

void QuantumNaturalGradient::compute(std::vector<double>& out_dx, std::vector<std::shared_ptr<AcceleratorBuffer>> in_results)
{
    // TODO
}
}
}