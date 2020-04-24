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
#include "DWave.hpp"
#include "rbm.hpp"
#include "CMREmbedding.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"
#include "generators/rbm.hpp"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL DWaveActivator : public BundleActivator {

public:
  DWaveActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto acc = std::make_shared<xacc::quantum::DWave>();
    context.RegisterService<xacc::Accelerator>(acc);

    // auto accd = std::make_shared<xacc::quantum::DWDecorator>();
    // context.RegisterService<xacc::Accelerator>(accd);
    // context.RegisterService<xacc::OptionsProvider>(acc);

    auto rbm = std::make_shared<xacc::dwave::RBM>();
    context.RegisterService<xacc::Instruction>(rbm);
    auto rbmascirc = std::make_shared<xacc::dwave::RBMAsCircuitType>();
    context.RegisterService<xacc::Instruction>(rbmascirc);
    // auto c = std::make_shared<xacc::quantum::DWQMICompiler>();
    // context.RegisterService<xacc::Compiler>(c);
    // context.RegisterService<xacc::OptionsProvider>(c);

    auto cmr = std::make_shared<xacc::cmr::CMREmbedding>();
    context.RegisterService<xacc::quantum::EmbeddingAlgorithm>(cmr);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(DWaveActivator)
