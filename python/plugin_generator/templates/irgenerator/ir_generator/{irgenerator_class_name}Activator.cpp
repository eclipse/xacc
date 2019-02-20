#include "{irgenerator_class_name}.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

using namespace cppmicroservices;

class US_ABI_LOCAL {irgenerator_class_name}Activator : public BundleActivator {{
public:
  {irgenerator_class_name}Activator() {{}}

  void Start(BundleContext context) {{
    auto irgen = std::make_shared<xacc::quantum::{irgenerator_class_name}>();
    context.RegisterService<xacc::IRGenerator>(irgen);
  }}

  void Stop(BundleContext context) {{}}
}};

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR({irgenerator_class_name}Activator)
