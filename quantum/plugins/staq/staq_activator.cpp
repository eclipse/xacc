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
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "staq_compiler.hpp"
#include "staq_rotation_folding.hpp"
#include "staq_swap_short.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL StaqActivator : public BundleActivator {

public:
  StaqActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto c = std::make_shared<xacc::StaqCompiler>();
    context.RegisterService<xacc::Compiler>(c);

    auto c1 = std::make_shared<xacc::quantum::RotationFolding>();
    context.RegisterService<xacc::IRTransformation>(c1);


    auto c2 = std::make_shared<xacc::quantum::SwapShort>();
    context.RegisterService<xacc::IRTransformation>(c2);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(StaqActivator)