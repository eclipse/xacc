/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#include "QFT.hpp"
#include "InverseQFT.hpp"
#include "KernelReplacementPreprocessor.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL GateQuantumActivator: public BundleActivator {

public:

	GateQuantumActivator() {
	}

	/**
	 */
	void Start(BundleContext context) {
		auto qft = std::make_shared<xacc::quantum::QFT>();
		auto iqft = std::make_shared<xacc::quantum::InverseQFT>();
		auto kp =
				std::make_shared<xacc::quantum::KernelReplacementPreprocessor>();
		context.RegisterService<xacc::Preprocessor>(kp);
		context.RegisterService<xacc::IRGenerator>(iqft);
		context.RegisterService<xacc::IRGenerator>(qft);
	}

	/**
	 */
	void Stop(BundleContext /*context*/) {
	}

};

}

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(GateQuantumActivator)
