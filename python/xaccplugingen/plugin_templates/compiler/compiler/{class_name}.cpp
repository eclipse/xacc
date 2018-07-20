#include "{class_name}.hpp"
#include "XACC.hpp"

namespace xacc {{
    namespace quantum {{
        {class_name}::{class_name}() = default;

        std::shared_ptr<IR> {class_name}::compile(const std::string &src, std::shared_ptr<Accelerator> acc) {{
            accelerator = acc;
            return compile(src);
        }}

        std::shared_ptr<IR> {class_name}::compile(const std::string &src) {{
            auto ir = xacc::getService<IRProvider>("gate")->createIR();

            // TODO: Add kernel(s) to IR

            return ir;
        }}

        std::string {class_name}::translate(const std::string &bufferVariable, std::shared_ptr<xacc::Function> function) {{
            // TODO: Implement this
            return "";
        }}
    }}
}}
