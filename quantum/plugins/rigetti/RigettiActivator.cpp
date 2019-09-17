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
#include "QVMAccelerator.hpp"
#include "QuilCompiler.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL RigettiActivator : public BundleActivator {

public:
  RigettiActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto acc = std::make_shared<xacc::quantum::QVMAccelerator>();
    context.RegisterService<xacc::Accelerator>(acc);
    context.RegisterService<xacc::OptionsProvider>(acc);

    auto c = std::make_shared<xacc::quantum::QuilCompiler>();
    context.RegisterService<xacc::Compiler>(c);
    context.RegisterService<xacc::OptionsProvider>(c);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(RigettiActivator)
