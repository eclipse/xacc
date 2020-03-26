/*******************************************************************************
 * Copyright (c) 2020 UT-Battelle, LLC.
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
#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"
#include <memory>
#include "OptimalControl.hpp"
#include "GOAT.hpp"
#include "GRAPE.hpp"

using namespace cppmicroservices;

namespace {
class US_ABI_LOCAL OptimalControlActivator : public BundleActivator 
{

public:
  OptimalControlActivator() {}

  void Start(BundleContext context) 
  {
    context.RegisterService<xacc::Optimizer>(std::make_shared<xacc::ControlOptimizer>());
    context.RegisterService<xacc::UnitaryMatrixUtil>(std::make_shared<xacc::PauliUnitaryMatrixUtil>());
    // Register the GOAT pulse optimization
    context.RegisterService<xacc::Optimizer>(std::make_shared<xacc::PulseOptimGOAT>());
    // Register the GRAPE pulse optimization
    context.RegisterService<xacc::Optimizer>(std::make_shared<xacc::PulseOptimGRAPE>());
  }

  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(OptimalControlActivator)
