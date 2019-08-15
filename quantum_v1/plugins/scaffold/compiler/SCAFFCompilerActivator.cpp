#include "ScaffCompiler.hpp"
#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL SCAFFActivator : public BundleActivator {

public:
  SCAFFActivator() {}

  /**
   */
  void Start(BundleContext context) {

    auto s = std::make_shared<scaffold::ScaffCompiler>();
    context.RegisterService<xacc::Compiler>(s);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(SCAFFActivator)
