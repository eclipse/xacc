#include "exprtk_parsing_util.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

#include <memory>
#include <set>

using namespace cppmicroservices;

namespace {

class US_ABI_LOCAL ExprtkActivator : public BundleActivator {

public:
  ExprtkActivator() = default;

  void Start(BundleContext context) {
    auto c = std::make_shared<xacc::ExprtkExpressionParsingUtil>();
    context.RegisterService<xacc::ExpressionParsingUtil>(c);
  }

  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(ExprtkActivator)
