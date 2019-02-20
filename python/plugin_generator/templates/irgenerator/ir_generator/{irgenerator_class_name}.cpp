#include "{irgenerator_class_name}.hpp"
#include "XACC.hpp"
#include "IRProvider.hpp"
#include "GateFunction.hpp"

namespace xacc {{
    namespace quantum {{
        {irgenerator_class_name}::{irgenerator_class_name}() = default;

        std::shared_ptr<Function> {irgenerator_class_name}::generate(std::map<std::string, InstructionParameter> parameters) {{
            
            // TODO: Process/manipulate parameters to ensure they are valid for IRGenerator to use
            std::vector<InstructionParameter> params;
            
            return generate(nullptr, params);
        }}

        std::shared_ptr<Function> {irgenerator_class_name}::generate(
                            std::shared_ptr<AcceleratorBuffer> buffer,
                            std::vector<InstructionParameter> parameters) {{
            
            // TODO: Replace this placeholder GateFunction with desired kernel
            auto kernel = std::make_shared<xacc::quantum::GateFunction>("{irgenerator_class_name}_function");
            
            // TODO: Construct the XACC Kernel 
            
            return kernel;
        }}
    }}
}}
