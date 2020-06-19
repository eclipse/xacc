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
#include "adapt.hpp"
#include "operator_pools/SingletAdaptedUCCSD.hpp"
#include "operator_pools/QubitPool.hpp"
#include "operator_pools/SingleQubitQAOA.hpp"
#include "operator_pools/MultiQubitQAOA.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>

using namespace cppmicroservices;

namespace {

class US_ABI_LOCAL ADAPT_Activator : public BundleActivator {

public:
  ADAPT_Activator() {}

  void Start(BundleContext context) {
    auto c = std::make_shared<xacc::algorithm::ADAPT>();
    context.RegisterService<xacc::Algorithm>(c);

    auto uccsd = std::make_shared<xacc::algorithm::SingletAdaptedUCCSD>();
    context.RegisterService<xacc::OperatorPool>(uccsd);

    auto qpool = std::make_shared<xacc::algorithm::QubitPool>();
    context.RegisterService<xacc::OperatorPool>(qpool);

    auto sqaoa = std::make_shared<xacc::algorithm::SingleQubitQAOA>();
    context.RegisterService<xacc::OperatorPool>(sqaoa);

    auto mqaoa = std::make_shared<xacc::algorithm::MultiQubitQAOA>();
    context.RegisterService<xacc::OperatorPool>(mqaoa);

  }

  void Stop(BundleContext /*context*/) {}
};

}

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(ADAPT_Activator)