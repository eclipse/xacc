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
#include "QsimAccelerator.hpp"

namespace xacc {
namespace quantum {
void QsimAccelerator::initialize(const HeterogeneousMap& params)
{
    
}

void QsimAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::shared_ptr<CompositeInstruction> compositeInstruction)
{
   
}

void QsimAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<std::shared_ptr<CompositeInstruction>> compositeInstructions)
{
    
}

void QsimAccelerator::apply(std::shared_ptr<AcceleratorBuffer> buffer, std::shared_ptr<Instruction> inst) 
{
}

#define REGISTER_ACCELERATOR(QsimAccelerator)
}}

