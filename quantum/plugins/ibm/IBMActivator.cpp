#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>
#include "IBMAccelerator.hpp"

#ifdef LAPACK_FOUND
#include "LocalIBMAccelerator.hpp"
#endif

#include "OQASMCompiler.hpp"
#include "QObjectCompiler.hpp"

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL IBMActivator : public BundleActivator {

public:
  IBMActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto acc = std::make_shared<xacc::quantum::IBMAccelerator>();

#ifdef LAPACK_FOUND
    auto acc2 = std::make_shared<xacc::quantum::LocalIBMAccelerator>();
    context.RegisterService<xacc::Accelerator>(acc2);
#endif
    auto c = std::make_shared<xacc::quantum::OQASMCompiler>();
    context.RegisterService<xacc::Compiler>(c);
    context.RegisterService<xacc::OptionsProvider>(c);

    auto c2 = std::make_shared<xacc::quantum::QObjectCompiler>();
    context.RegisterService<xacc::Compiler>(c2);

    context.RegisterService<xacc::Accelerator>(acc);

    context.RegisterService<xacc::OptionsProvider>(acc);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(IBMActivator)
