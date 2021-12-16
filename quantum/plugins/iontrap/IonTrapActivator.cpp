/*******************************************************************************
 * Copyright (c) 2021 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Thien Nguyen - initial API and implementation
 *   Daniel Strano - adaption from Quantum++ to Qrack
 *   Austin Adams - aggressive copying and pasting for GTRI testbed
 *******************************************************************************/
#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"
#include "IonTrapAccelerator.hpp"
#include "IonTrapOneQubitPass.hpp"
#include "IonTrapTwoQubitPass.hpp"

using namespace cppmicroservices;

class US_ABI_LOCAL IonTrapActivator : public BundleActivator {
public:
  IonTrapActivator() {}

  void Start(BundleContext context) {
    auto acc = std::make_shared<xacc::quantum::IonTrapAccelerator>();
    context.RegisterService<xacc::Accelerator>(acc);

    auto twoQubitPass = std::make_shared<xacc::quantum::IonTrapTwoQubitPass>();
    context.RegisterService<xacc::IRTransformation>(twoQubitPass);

    auto oneQubitPass = std::make_shared<xacc::quantum::IonTrapOneQubitPass>();
    context.RegisterService<xacc::IRTransformation>(oneQubitPass);
  }

  void Stop(BundleContext context) {}
};

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(IonTrapActivator)
