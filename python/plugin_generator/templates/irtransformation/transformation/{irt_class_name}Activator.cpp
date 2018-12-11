#include "{irt_class_name}.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

using namespace cppmicroservices;

class US_ABI_LOCAL {irt_class_name}Activator : public BundleActivator {{
public:
  {irt_class_name}Activator() {{}}

  void Start(BundleContext context) {{
    auto irt = std::make_shared<xacc::quantum::{irt_class_name}>();
    context.RegisterService<xacc::IRTransformation>(irt);
    context.RegisterService<xacc::OptionsProvider>(irt);
  }}

  void Stop(BundleContext context) {{}}
}};

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR({irt_class_name}Activator)
