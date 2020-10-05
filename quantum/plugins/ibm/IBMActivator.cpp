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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>
#include "IBMAccelerator.hpp"

#ifdef LAPACK_FOUND
#include "LocalIBMAccelerator.hpp"
#endif

#include "OQASMCompiler.hpp"
#include "QObjectCompiler.hpp"

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL IBMActivator : public BundleActivator {

public:
  IBMActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto acc = std::make_shared<xacc::quantum::IBMAccelerator>();

#ifdef LAPACK_FOUND
    auto acc2 = std::make_shared<xacc::quantum::LocalIBMAccelerator>();
    context.RegisterService<xacc::Accelerator>(acc2);
#endif
    auto c = std::make_shared<xacc::quantum::OQASMCompiler>();
    context.RegisterService<xacc::Compiler>(c);
    // context.RegisterService<xacc::OptionsProvider>(c);

    auto c2 = std::make_shared<xacc::quantum::QObjectCompiler>();
    context.RegisterService<xacc::Compiler>(c2);

    context.RegisterService<xacc::Accelerator>(acc);

    auto qasm_qobj_gen = std::make_shared<xacc::quantum::QasmQObjGenerator>();
    auto pulse_qobj_gen = std::make_shared<xacc::quantum::PulseQObjGenerator>();

    context.RegisterService<quantum::QObjGenerator>(qasm_qobj_gen);
    context.RegisterService<quantum::QObjGenerator>(pulse_qobj_gen);

    auto pulse_transform = std::make_shared<xacc::quantum::IBMPulseTransform>();
    context.RegisterService<xacc::IRTransformation>(pulse_transform);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(IBMActivator)
