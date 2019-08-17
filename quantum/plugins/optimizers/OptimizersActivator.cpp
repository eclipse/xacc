#include "CircuitOptimizer.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL OptimizersActivator: public BundleActivator {

public:

	OptimizersActivator() {
	}

	/**
	 */
	void Start(BundleContext context) {
		auto c4 = std::make_shared<xacc::quantum::CircuitOptimizer>();
        context.RegisterService<xacc::IRTransformation>(c4);

	}

	/**
	 */
	void Stop(BundleContext /*context*/) {
	}

};

}

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(OptimizersActivator)
