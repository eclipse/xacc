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
#include "CircuitOptimizer.hpp"
#include "default_placement.hpp"
#include "GateMergeOptimizer.hpp"
#include "PulseTransform.hpp"
#include "GateFusion.hpp"
#include "NearestNeighborTransform.hpp"
// #include "qsearch.hpp"
#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"
#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL OptimizersActivator : public BundleActivator {

public:
  OptimizersActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto c4 = std::make_shared<xacc::quantum::CircuitOptimizer>();
    context.RegisterService<xacc::IRTransformation>(c4);

    auto c5 = std::make_shared<xacc::quantum::DefaultPlacement>();
    context.RegisterService<xacc::IRTransformation>(c5);

    // auto c6 = std::make_shared<xacc::quantum::QsearchOptimizer>();
    // context.RegisterService<xacc::IRTransformation>(c6);

    context.RegisterService<xacc::IRTransformation>(c5);
    context.RegisterService<xacc::IRTransformation>(
        std::make_shared<xacc::quantum::PulseTransform>());
    context.RegisterService<GateFuser>(std::make_shared<GateFuser>());
    context.RegisterService<xacc::IRTransformation>(
        std::make_shared<xacc::quantum::MergeSingleQubitGatesOptimizer>());
    context.RegisterService<xacc::IRTransformation>(
        std::make_shared<xacc::quantum::MergeTwoQubitBlockOptimizer>());
    context.RegisterService<xacc::IRTransformation>(
        std::make_shared<xacc::quantum::NearestNeighborTransform>());
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(OptimizersActivator)
