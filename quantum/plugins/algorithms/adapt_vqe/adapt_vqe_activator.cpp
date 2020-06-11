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
#include "adapt_vqe.hpp"
#include "operator_pools/pools.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>

using namespace cppmicroservices;

namespace {

class US_ABI_LOCAL ADAPT_VQE_Activator : public BundleActivator {

public:
  ADAPT_VQE_Activator() {}

  void Start(BundleContext context) {
    auto c = std::make_shared<xacc::algorithm::ADAPT_VQE>();
    context.RegisterService<xacc::Algorithm>(c);

    auto uccsd = std::make_shared<xacc::algorithm::UCCSD>();
    context.RegisterService<xacc::algorithm::OperatorPool>(uccsd);

    auto qpool = std::make_shared<xacc::algorithm::QubitPool>();
    context.RegisterService<xacc::algorithm::OperatorPool>(qpool);

  }

  void Stop(BundleContext /*context*/) {}
};

}

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(ADAPT_VQE_Activator)