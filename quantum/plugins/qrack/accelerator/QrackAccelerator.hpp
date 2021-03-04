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
#pragma once
#include "xacc.hpp"
#include "QrackVisitor.hpp"

namespace xacc {
namespace quantum {

class QrackAccelerator : public Accelerator {
public:
    // Identifiable interface impls
    virtual const std::string name() const override { return "qrack"; }
    virtual const std::string description() const override { return "XACC Simulation Accelerator based on Qrack library."; }

    // Accelerator interface impls
    virtual void initialize(const HeterogeneousMap& params = {}) override;
    virtual void updateConfiguration(const HeterogeneousMap& config) override {initialize(config);};
    virtual const std::vector<std::string> configurationKeys() override { return {}; }
    virtual BitOrder getBitOrder() override {return BitOrder::LSB;}
    virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::shared_ptr<CompositeInstruction> compositeInstruction) override;
    virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<std::shared_ptr<CompositeInstruction>> compositeInstructions) override;
private:
    std::shared_ptr<QrackVisitor> m_visitor;
    int m_shots = -1;
    bool m_use_opencl = true;
    bool m_use_qunit = true;
    bool m_use_opencl_multi = false;
    bool m_use_stabilizer = true;
    int m_device_id = -1;
    bool m_do_normalize = false;
    double m_zero_threshold = REAL1_EPSILON;
};
}}
