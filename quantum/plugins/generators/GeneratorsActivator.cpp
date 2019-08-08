#include "hwe.hpp"
#include "exp.hpp"
#include "range.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

using namespace cppmicroservices;

class US_ABI_LOCAL GeneratorsActivator : public BundleActivator {
public:
  GeneratorsActivator() {}

  void Start(BundleContext context) {
    auto hwe = std::make_shared<xacc::generators::HWE>();
    auto expit = std::make_shared<xacc::generators::Exp>();
    auto r = std::make_shared<xacc::generators::Range>();
    context.RegisterService<xacc::IRGenerator>(hwe);
    context.RegisterService<xacc::IRGenerator>(expit);
    context.RegisterService<xacc::IRGenerator>(r);

  }

  void Stop(BundleContext context) {}
};

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(GeneratorsActivator)
