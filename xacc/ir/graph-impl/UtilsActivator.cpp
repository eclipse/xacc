#include "DirectedBoostGraph.hpp"
#include "UndirectedBoostGraph.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

using namespace cppmicroservices;

class US_ABI_LOCAL UtilsActivator : public BundleActivator {
public:
  UtilsActivator() {}

  void Start(BundleContext context) {
    auto g = std::make_shared<xacc::DirectedBoostGraph>();
    auto u = std::make_shared<xacc::UndirectedBoostGraph>();

    context.RegisterService<xacc::Graph>(g);
    context.RegisterService<xacc::Graph>(u);

  }

  void Stop(BundleContext context) {}
};

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(UtilsActivator)
