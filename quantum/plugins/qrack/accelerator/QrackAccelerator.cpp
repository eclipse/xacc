/*******************************************************************************
 * Copyright (c) 2019-2020 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *   Daniel Strano - adaption from Quantum++ to Qrack
 *******************************************************************************/
#include <typeinfo>
#include "QrackAccelerator.hpp"

namespace xacc {
namespace quantum {

    void QrackAccelerator::initialize(const HeterogeneousMap& params)
    {
        m_visitor = std::make_shared<QrackVisitor>();

        if (params.keyExists<int>("shots"))
        {
            m_shots = params.get<int>("shots");
            if (m_shots < 1)
            {
                xacc::error("Invalid 'shots' parameter. (Must be >= 1.)");
            }
        }

        if (params.keyExists<bool>("use_opencl"))
        {
            m_use_opencl = params.get<bool>("use_opencl");
        }

        if (params.keyExists<bool>("use_qunit"))
        {
            m_use_qunit = params.get<bool>("use_qunit");
        }

        if (params.keyExists<bool>("use_opencl_multi"))
        {
            m_use_opencl_multi = params.get<bool>("use_opencl_multi");
        }

        if (params.keyExists<bool>("use_stabilizer"))
        {
            m_use_stabilizer = params.get<bool>("use_stabilizer");
        }

        if (params.keyExists<int>("device_id"))
        {
            m_device_id = params.get<int>("device_id");
        }

        if (params.keyExists<bool>("do_normalize"))
        {
            m_do_normalize = params.get<bool>("do_normalize");
        }

        if (params.keyExists<double>("zero_threshold"))
        {
            m_zero_threshold = params.get<double>("zero_threshold");
            if (m_zero_threshold < 0)
            {
                xacc::error("Invalid 'zero_threshold' parameter. (Must be >= 0.)");
            }
        }
    }

    void QrackAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::shared_ptr<CompositeInstruction> compositeInstruction)
    {
        bool canSample = true;
        if (m_shots > 1) {
            bool didMeasure = false;

            InstructionIterator shotsIt(compositeInstruction);
            while (shotsIt.hasNext())
            {
                auto nextInst = shotsIt.next();
                if (!nextInst->isEnabled()) {
                    continue;
                }

                if (nextInst->name() == "Identity") {
                    continue;
                }

                if (nextInst->name() == "ifstmt") {
                    canSample = false;
                    break;
                }

                if (nextInst->name() == "Measure")
                {
                    didMeasure = true;
                }
                else if (didMeasure)
                {
                    canSample = false;
                    break;
                }
            }

            if (!canSample && xacc::verbose)
            {
                std::cout << "Cannot sample; must repeat circuit per shot. If possible, consider removing conditionals, running 1 shot, and/or only measuring at the end of the circuit." << std::endl;
            }
        }

        const auto runCircuit = [&](int shots){
            m_visitor->initialize(buffer, shots, m_use_opencl, m_use_qunit, m_use_opencl_multi, m_use_stabilizer, m_device_id, m_do_normalize, m_zero_threshold);

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

        if (canSample || (m_shots < 1))
        {
            runCircuit(m_shots);
        }
        else
        {
           for (int i = 0; i < m_shots; ++i)
           {
                runCircuit(1);
           }
        }
    }
    void QrackAccelerator::execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<std::shared_ptr<CompositeInstruction>> compositeInstructions)
    {
        for (auto& f : compositeInstructions)
        {
            auto tmpBuffer = std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size());
            execute(tmpBuffer, f);
            buffer->appendChild(f->name(), tmpBuffer);
        }
    }
}}
