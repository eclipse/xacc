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
 *   Daniel Claudino - initial API and implementation
 *******************************************************************************/
#include "ParameterShiftGradient.hpp"
#include "CentralDifferenceGradient.hpp"
#include "ForwardDifferenceGradient.hpp"
#include "BackwardDifferenceGradient.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"
#include "QuantumNaturalGradient.hpp"

#include <memory>

using namespace cppmicroservices;

namespace {

class US_ABI_LOCAL GradientStrategyActivator : public BundleActivator {

public:
  GradientStrategyActivator() {}

  void Start(BundleContext context) {

    auto ps = std::make_shared<xacc::algorithm::ParameterShiftGradient>();
    context.RegisterService<xacc::AlgorithmGradientStrategy>(ps);
    
    auto cd = std::make_shared<xacc::algorithm::CentralDifferenceGradient>();
    context.RegisterService<xacc::AlgorithmGradientStrategy>(cd);

    auto fd = std::make_shared<xacc::algorithm::ForwardDifferenceGradient>();
    context.RegisterService<xacc::AlgorithmGradientStrategy>(fd);

    auto bd = std::make_shared<xacc::algorithm::BackwardDifferenceGradient>();
    context.RegisterService<xacc::AlgorithmGradientStrategy>(bd);

    context.RegisterService<xacc::AlgorithmGradientStrategy>(std::make_shared<xacc::algorithm::QuantumNaturalGradient>());

  }

  void Stop(BundleContext /*context*/) {}
};

}

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(GradientStrategyActivator)