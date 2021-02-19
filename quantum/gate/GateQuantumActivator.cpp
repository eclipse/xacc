/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include "CommonGates.hpp"

#include "PulseScheduler.hpp"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL GateQuantumActivator : public BundleActivator {

public:
  GateQuantumActivator() {}

  /**
   */
  void Start(BundleContext context) {

    auto h = std::make_shared<xacc::quantum::Hadamard>();
    auto cn = std::make_shared<xacc::quantum::CNOT>();
    auto cp = std::make_shared<xacc::quantum::CPhase>();
    auto cz = std::make_shared<xacc::quantum::CZ>();
    auto id = std::make_shared<xacc::quantum::Identity>();
    auto m = std::make_shared<xacc::quantum::Measure>();
    auto rx = std::make_shared<xacc::quantum::Rx>();
    auto ry = std::make_shared<xacc::quantum::Ry>();
    auto rz = std::make_shared<xacc::quantum::Rz>();
    auto x = std::make_shared<xacc::quantum::X>();
    auto y = std::make_shared<xacc::quantum::Y>();
    auto z = std::make_shared<xacc::quantum::Z>();
    auto sw = std::make_shared<xacc::quantum::Swap>();
    auto isw = std::make_shared<xacc::quantum::iSwap>();
    auto fsim = std::make_shared<xacc::quantum::fSim>();
    auto xy = std::make_shared<xacc::quantum::XY>();
    auto u = std::make_shared<xacc::quantum::U>();
    auto u1 = std::make_shared<xacc::quantum::U1>();

    auto anneal = std::make_shared<xacc::quantum::AnnealingInstruction>();

    auto s = std::make_shared<xacc::quantum::S>();
    auto sdg = std::make_shared<xacc::quantum::Sdg>();
    auto t = std::make_shared<xacc::quantum::T>();
    auto tdg = std::make_shared<xacc::quantum::Tdg>();

    auto cy = std::make_shared<xacc::quantum::CY>();
    auto crz= std::make_shared<xacc::quantum::CRZ>();
    auto ch = std::make_shared<xacc::quantum::CH>();

    auto ifstmt = std::make_shared<xacc::quantum::IfStmt>();
    auto reset = std::make_shared<xacc::quantum::Reset>();
    auto scheduler = std::make_shared<xacc::quantum::PulseScheduler>();
    context.RegisterService<xacc::Scheduler>(scheduler);

    context.RegisterService<xacc::Instruction>(h);
    context.RegisterService<xacc::Instruction>(cn);
    context.RegisterService<xacc::Instruction>(cp);
    context.RegisterService<xacc::Instruction>(cz);
    context.RegisterService<xacc::Instruction>(id);
    context.RegisterService<xacc::Instruction>(m);
    context.RegisterService<xacc::Instruction>(rx);
    context.RegisterService<xacc::Instruction>(ry);
    context.RegisterService<xacc::Instruction>(rz);
    context.RegisterService<xacc::Instruction>(x);
    context.RegisterService<xacc::Instruction>(y);
    context.RegisterService<xacc::Instruction>(z);
    context.RegisterService<xacc::Instruction>(sw);
    context.RegisterService<xacc::Instruction>(isw);
    context.RegisterService<xacc::Instruction>(fsim);
    context.RegisterService<xacc::Instruction>(xy);
    context.RegisterService<xacc::Instruction>(u);
    context.RegisterService<xacc::Instruction>(u1);

    context.RegisterService<xacc::Instruction>(ifstmt);
    context.RegisterService<xacc::Instruction>(anneal);

    context.RegisterService<xacc::Instruction>(s);
    context.RegisterService<xacc::Instruction>(sdg);
    context.RegisterService<xacc::Instruction>(t);
    context.RegisterService<xacc::Instruction>(tdg);
    context.RegisterService<xacc::Instruction>(cy);
    context.RegisterService<xacc::Instruction>(crz);
    context.RegisterService<xacc::Instruction>(ch);
    context.RegisterService<xacc::Instruction>(reset);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(GateQuantumActivator)
