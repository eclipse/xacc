// #include "SymVerificationDecorator.hpp"
// #include "VQERestartDecorator.hpp"
#include "ImprovedSamplingDecorator.hpp"
#include "RDMPurificationDecorator.hpp"
#include "ROErrorDecorator.hpp"
#include "RichExtrapDecorator.hpp"

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

	/**
	 */
	void Start(BundleContext context) {
		auto c = std::make_shared<xacc::quantum::ImprovedSamplingDecorator>();
		auto c2 = std::make_shared<xacc::quantum::RichExtrapDecorator>();
		auto c3 = std::make_shared<xacc::quantum::ROErrorDecorator>();
		auto c4 = std::make_shared<xacc::quantum::RDMPurificationDecorator>();

		context.RegisterService<xacc::AcceleratorDecorator>(c2);
        context.RegisterService<xacc::Accelerator>(c2);

        context.RegisterService<xacc::AcceleratorDecorator>(c);
        context.RegisterService<xacc::Accelerator>(c);

        context.RegisterService<xacc::AcceleratorDecorator>(c3);
        context.RegisterService<xacc::Accelerator>(c3);

        context.RegisterService<xacc::AcceleratorDecorator>(c4);
        context.RegisterService<xacc::Accelerator>(c4);

	}

	/**
	 */
	void Stop(BundleContext /*context*/) {
	}

};

}

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(DecoratorsActivator)
