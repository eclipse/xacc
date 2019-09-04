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
#include "exprtk_parsing_util.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

class US_ABI_LOCAL ExprtkActivator : public BundleActivator {

public:
  ExprtkActivator() = default;

  void Start(BundleContext context) {
    auto c = std::make_shared<xacc::ExprtkExpressionParsingUtil>();
    context.RegisterService<xacc::ExpressionParsingUtil>(c);
  }

  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(ExprtkActivator)
