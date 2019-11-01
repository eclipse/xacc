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
#include "rbm_classification.hpp"
#include "classical_data_expectations.hpp"
#include "dwave_rbm_mcmc_expectations.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL RBMClassificationActivator : public BundleActivator {

public:
  RBMClassificationActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto c = std::make_shared<xacc::algorithm::RBMClassification>();
    context.RegisterService<xacc::Algorithm>(c);

    auto cd =
        std::make_shared<xacc::algorithm::ClassicalDataExpectationStrategy>();
    context.RegisterService<xacc::algorithm::ExpectationStrategy>(cd);

    auto cdd = std::make_shared<
        xacc::algorithm::DWaveRBM_MCMCDataExpectationStrategy>();
    context.RegisterService<xacc::algorithm::ExpectationStrategy>(cdd);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(RBMClassificationActivator)
