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
#include "DirectedBoostGraph.hpp"
#include "UndirectedBoostGraph.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

using namespace cppmicroservices;

class US_ABI_LOCAL UtilsActivator : public BundleActivator {
public:
  UtilsActivator() {}

  void Start(BundleContext context) {
    auto g = std::make_shared<xacc::DirectedBoostGraph>();
    auto u = std::make_shared<xacc::UndirectedBoostGraph>();

    context.RegisterService<xacc::Graph>(g);
    context.RegisterService<xacc::Graph>(u);

  }

  void Stop(BundleContext context) {}
};

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(UtilsActivator)
