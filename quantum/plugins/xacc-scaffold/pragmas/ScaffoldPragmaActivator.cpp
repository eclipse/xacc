#include "ScaffoldPragmaHandler.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL ScaffoldPragmaActivator : public BundleActivator {

public:
  ScaffoldPragmaActivator() {}

  /**
   */
  void Start(BundleContext context) {

    auto ph = std::make_shared<scaffold::compiler::ScaffoldPragmaHandler>();
    context.RegisterService<qcor::compiler::QCORPragmaHandler>(ph);

  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(ScaffoldPragmaActivator)
