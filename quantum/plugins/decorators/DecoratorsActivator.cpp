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
#include "ImprovedSamplingDecorator.hpp"
#include "RDMPurificationDecorator.hpp"
#include "ROErrorDecorator.hpp"
#include "RichExtrapDecorator.hpp"
#include "AssignmentErrorKernelDecorator.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL DecoratorsActivator: public BundleActivator {

public:

	DecoratorsActivator() {
	}

	void Start(BundleContext context) {
		auto c = std::make_shared<xacc::quantum::ImprovedSamplingDecorator>();
		auto c2 = std::make_shared<xacc::quantum::RichExtrapDecorator>();
		auto c3 = std::make_shared<xacc::quantum::ROErrorDecorator>();
		auto c4 = std::make_shared<xacc::quantum::RDMPurificationDecorator>();
        auto c5 = std::make_shared<xacc::quantum::AssignmentErrorKernelDecorator>();

		context.RegisterService<xacc::AcceleratorDecorator>(c2);
        context.RegisterService<xacc::Accelerator>(c2);

        context.RegisterService<xacc::AcceleratorDecorator>(c);
        context.RegisterService<xacc::Accelerator>(c);

        context.RegisterService<xacc::AcceleratorDecorator>(c3);
        context.RegisterService<xacc::Accelerator>(c3);

        context.RegisterService<xacc::AcceleratorDecorator>(c4);
        context.RegisterService<xacc::Accelerator>(c4);

        context.RegisterService<xacc::AcceleratorDecorator>(c5);
        context.RegisterService<xacc::Accelerator>(c5);

	}

	/**
	 */
	void Stop(BundleContext /*context*/) {
	}

};

}

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(DecoratorsActivator)
