#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include "RBMGenerator.hpp"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL RBMGeneratorActivator: public BundleActivator {

public:

	RBMGeneratorActivator() {
	}

	/**
	 */
	void Start(BundleContext context) {
		auto rbm = std::make_shared<xacc::rbm::RBMGenerator>();
		context.RegisterService<xacc::IRGenerator>(rbm);
	}

	/**
	 */
	void Stop(BundleContext /*context*/) {
	}

};

}

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(RBMGeneratorActivator)