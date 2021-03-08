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
#pragma one
#include "xacc.hpp"
#include "QppVisitor.hpp"
#include "NoiseModel.hpp"

namespace xacc {
namespace quantum {

class QppAccelerator : public Accelerator {
public:
    // Identifiable interface impls
    virtual const std::string name() const override { return "qpp"; }
    virtual const std::string description() const override { return "XACC Simulation Accelerator based on qpp library."; }

    // Accelerator interface impls
    virtual void initialize(const HeterogeneousMap& params = {}) override;
    virtual void updateConfiguration(const HeterogeneousMap& config) override {initialize(config);};
    virtual const std::vector<std::string> configurationKeys() override { return {}; }
    virtual BitOrder getBitOrder() override {return BitOrder::LSB;}
    virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::shared_ptr<CompositeInstruction> compositeInstruction) override;
    virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer, const std::vector<std::shared_ptr<CompositeInstruction>> compositeInstructions) override;
    virtual void apply(std::shared_ptr<AcceleratorBuffer> buffer, std::shared_ptr<Instruction> inst) override;
    std::vector<std::pair<int, int>> getConnectivity() override {
      return m_connectivity;
    }
    // ExecutionInfo implementation:
    virtual xacc::HeterogeneousMap getExecutionInfo() const override { return m_executionInfo; }
  
  private:
    // Cache execution info after execution
    void cacheExecutionInfo();
    std::shared_ptr<QppVisitor> m_visitor;
    // Number of 'shots' if random sampling simulation is enabled.
    // -1 means disabled (no shots, just expectation value)
    int m_shots = -1;
    bool m_vqeMode;
    std::vector<std::pair<int,int>> m_connectivity;
    xacc::HeterogeneousMap m_executionInfo;
    std::pair<AcceleratorBuffer*, size_t> m_currentBuffer;
};

class DefaultNoiseModelUtils : public NoiseModelUtils 
{
public:
    // Identifiable interface impls
    virtual const std::string name() const override { return "default"; }
    virtual const std::string description() const override { return "Default noise model utils."; }
    virtual NoiseModelUtils::cMat krausToChoi(const std::vector<NoiseModelUtils::cMat>& in_krausMats) const override;
    virtual std::vector<NoiseModelUtils::cMat> choiToKraus(const NoiseModelUtils::cMat& in_choiMat) const override;
    virtual NoiseModelUtils::cMat combineChannelOps(const std::vector<NoiseModelUtils::cMat> &in_choiMats) const override;
};
}}
