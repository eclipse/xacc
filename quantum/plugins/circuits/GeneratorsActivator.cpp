#include "hwe.hpp"
#include "exp.hpp"
#include "range.hpp"
// #include "uccsd.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

using namespace cppmicroservices;

class US_ABI_LOCAL GeneratorsActivator : public BundleActivator {
public:
  GeneratorsActivator() {}

  void Start(BundleContext context) {
    auto hwe = std::make_shared<xacc::circuits::HWE>();
    auto expit = std::make_shared<xacc::circuits::Exp>();
    auto r = std::make_shared<xacc::circuits::Range>();
    // auto u = std::make_shared<xacc::generators::UCCSD>();
    context.RegisterService<xacc::Instruction>(hwe);
    context.RegisterService<xacc::Instruction>(expit);
    context.RegisterService<xacc::Instruction>(r);
    // context.RegisterService<xacc::IRGenerator>(u);

  }

  void Stop(BundleContext context) {}
};

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(GeneratorsActivator)
