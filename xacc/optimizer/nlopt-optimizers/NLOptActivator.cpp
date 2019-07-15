
#include "nlopt_optimizer.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL NLOptActivator: public BundleActivator {

public:

	NLOptActivator() {
	}

	/**
	 */
	void Start(BundleContext context) {
		auto c = std::make_shared<xacc::NLOptimizer>();

		context.RegisterService<xacc::Optimizer>(c);

	}

	/**
	 */
	void Stop(BundleContext /*context*/) {
	}

};

}

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(NLOptActivator)
