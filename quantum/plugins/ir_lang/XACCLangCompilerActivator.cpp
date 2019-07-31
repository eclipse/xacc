#include "XACCLangCompiler.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL XACCLangCompilerActivator : public BundleActivator {

public:
  XACCLangCompilerActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto c = std::make_shared<xacc::XACCLangCompiler>();
    context.RegisterService<xacc::Compiler>(c);
    context.RegisterService<xacc::OptionsProvider>(c);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(XACCLangCompilerActivator)