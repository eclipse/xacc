#include "vqe.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL VQEActivator : public BundleActivator {

public:
  VQEActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto c = std::make_shared<xacc::algorithm::VQE>();
    context.RegisterService<xacc::Algorithm>(c);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(VQEActivator)
