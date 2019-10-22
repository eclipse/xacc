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
#include "ddcl.hpp"
#include "strategies/js.hpp"
#include "strategies/mmd.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL DDCLActivator : public BundleActivator {

public:
  DDCLActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto c = std::make_shared<xacc::algorithm::DDCL>();
    context.RegisterService<xacc::Algorithm>(c);

    auto js = std::make_shared<xacc::algorithm::JSLossStrategy>();
    context.RegisterService<xacc::algorithm::LossStrategy>(js);
    auto js_g_ps =
        std::make_shared<xacc::algorithm::JSParameterShiftGradientStrategy>();
    context.RegisterService<xacc::algorithm::GradientStrategy>(js_g_ps);

    auto mmd = std::make_shared<xacc::algorithm::MMDLossStrategy>();
    context.RegisterService<xacc::algorithm::LossStrategy>(mmd);
    auto mmd_g_ps =
      std::make_shared<xacc::algorithm::MMDParameterShiftGradientStrategy>();
    context.RegisterService<xacc::algorithm::GradientStrategy>(mmd_g_ps);

  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(DDCLActivator)
