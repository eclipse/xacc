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
#include "QFT.hpp"
#include "InverseQFT.hpp"
#include "KernelReplacementPreprocessor.hpp"
#include "ReadoutErrorIRPreprocessor.hpp"
#include "QubitMapIRPreprocessor.hpp"
#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include "GateIRProvider.hpp"
#include "Hadamard.hpp"
#include "CNOT.hpp"
#include "X.hpp"
#include "Y.hpp"
#include "Z.hpp"
#include "ConditionalFunction.hpp"
#include "Rz.hpp"
#include "Rx.hpp"
#include "Ry.hpp"
#include "CPhase.hpp"
#include "Swap.hpp"
#include "Measure.hpp"
#include "Identity.hpp"
#include "CZ.hpp"

#include "CircuitOptimizer.hpp"

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
    auto qft = std::make_shared<xacc::quantum::QFT>();
    auto iqft = std::make_shared<xacc::quantum::InverseQFT>();
    auto kp = std::make_shared<xacc::quantum::KernelReplacementPreprocessor>();

    auto readout =
        std::make_shared<xacc::quantum::ReadoutErrorIRPreprocessor>();
    auto qbitmap = std::make_shared<xacc::quantum::QubitMapIRPreprocessor>();

    auto giservice = std::make_shared<xacc::quantum::GateIRProvider>();

    auto opt = std::make_shared<xacc::quantum::CircuitOptimizer>();

    context.RegisterService<xacc::IRProvider>(giservice);
    context.RegisterService<xacc::Preprocessor>(kp);
    context.RegisterService<xacc::IRGenerator>(iqft);
    context.RegisterService<xacc::IRGenerator>(qft);

    context.RegisterService<xacc::IRPreprocessor>(readout);
    context.RegisterService<xacc::IRPreprocessor>(qbitmap);

    context.RegisterService<xacc::IRTransformation>(opt);
    context.RegisterService<xacc::OptionsProvider>(opt);

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

    context.RegisterService<xacc::quantum::GateInstruction>(h);
    context.RegisterService<xacc::quantum::GateInstruction>(cn);
    context.RegisterService<xacc::quantum::GateInstruction>(cp);
    context.RegisterService<xacc::quantum::GateInstruction>(cz);
    context.RegisterService<xacc::quantum::GateInstruction>(id);
    context.RegisterService<xacc::quantum::GateInstruction>(m);
    context.RegisterService<xacc::quantum::GateInstruction>(rx);
    context.RegisterService<xacc::quantum::GateInstruction>(ry);
    context.RegisterService<xacc::quantum::GateInstruction>(rz);
    context.RegisterService<xacc::quantum::GateInstruction>(x);
    context.RegisterService<xacc::quantum::GateInstruction>(y);
    context.RegisterService<xacc::quantum::GateInstruction>(z);
    context.RegisterService<xacc::quantum::GateInstruction>(sw);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(GateQuantumActivator)
