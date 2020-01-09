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
#include "QppAccelerator.hpp"

namespace xacc {
namespace quantum {

    void QppAccelerator::initialize(const HeterogeneousMap& params)
    {
        m_visitor = std::make_shared<QppVisitor>();
        if (params.keyExists<int>("shots"))
        {
            m_shots = params.get<int>("shots");
            if (m_shots < 1)
            {
                xacc::error("Invalid 'shots' parameter.");
            }
        }
    }

    void QppAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::shared_ptr<CompositeInstruction> compositeInstruction)
    {
        const auto runCircuit = [&](bool shotsMode){
            m_visitor->initialize(buffer, shotsMode);

            // Walk the IR tree, and visit each node
            InstructionIterator it(compositeInstruction);
            while (it.hasNext())
            {
                auto nextInst = it.next();
                if (nextInst->isEnabled())
                {
                    nextInst->accept(m_visitor);
                }
            }

            m_visitor->finalize();
        };


        if (m_shots < 0)
        {
            runCircuit(false);
        }
        else
        {
           for (int i = 0; i < m_shots; ++i)
           {
                runCircuit(true);
           }
        }
    }
    void QppAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<std::shared_ptr<CompositeInstruction>> compositeInstructions)
    {
        for (auto& f : compositeInstructions)
        {
            auto tmpBuffer = std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size());
            execute(tmpBuffer, f);
            buffer->appendChild(f->name(), tmpBuffer);
        }
    }
}}