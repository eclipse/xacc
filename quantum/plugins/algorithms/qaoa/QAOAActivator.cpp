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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include "qaoa.hpp"
#include "qaoa_maxcut.hpp"
#include "qaoa_circuit.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL QAOAActivator : public BundleActivator {

public:
  QAOAActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto c = std::make_shared<xacc::algorithm::QAOA>();
    auto c1 = std::make_shared<xacc::algorithm::maxcut_qaoa>();
    context.RegisterService<xacc::Algorithm>(c);
    context.RegisterService<xacc::Algorithm>(c1);
    context.RegisterService<xacc::Instruction>(std::make_shared<xacc::circuits::QAOA>());
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(QAOAActivator)
