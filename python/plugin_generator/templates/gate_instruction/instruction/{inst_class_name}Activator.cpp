#include "{inst_class_name}.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

using namespace cppmicroservices;

class US_ABI_LOCAL {inst_class_name}Activator : public BundleActivator {{
public:
  {inst_class_name}Activator() {{}}

  void Start(BundleContext context) {{
    auto inst = std::make_shared<xacc::quantum::{inst_class_name}>();
    context.RegisterService<xacc::quantum::GateInstruction>(inst);
  }}

  void Stop(BundleContext context) {{}}
}};

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR({inst_class_name}Activator)
