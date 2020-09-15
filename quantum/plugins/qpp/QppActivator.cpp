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
#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"
#include "QppAccelerator.hpp"

using namespace cppmicroservices;

class US_ABI_LOCAL QppActivator : public BundleActivator {
public:
  QppActivator() {}

  void Start(BundleContext context) {
    auto acc = std::make_shared<xacc::quantum::QppAccelerator>();
    context.RegisterService<xacc::Accelerator>(acc);
    context.RegisterService<xacc::NoiseModelUtils>(std::make_shared<xacc::quantum::DefaultNoiseModelUtils>());
  }

  void Stop(BundleContext context) {}
};

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(QppActivator)
