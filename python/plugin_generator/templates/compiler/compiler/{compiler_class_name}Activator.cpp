#include "{compiler_class_name}.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

using namespace cppmicroservices;

class US_ABI_LOCAL {compiler_class_name}Activator : public BundleActivator {{
public:
  {compiler_class_name}Activator() {{}}

  void Start(BundleContext context) {{
    auto c = std::make_shared<xacc::quantum::{compiler_class_name}>();
    context.RegisterService<xacc::Compiler>(c);
    context.RegisterService<xacc::OptionsProvider>(c);
  }}

  void Stop(BundleContext context) {{}}
}};

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR({compiler_class_name}Activator)
