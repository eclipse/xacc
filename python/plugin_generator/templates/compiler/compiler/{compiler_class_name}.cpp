#include "{compiler_class_name}.hpp"
#include "xacc.hpp"
#include "IRProvider.hpp"

namespace xacc {{
    namespace quantum {{
        {compiler_class_name}::{compiler_class_name}() = default;

        std::shared_ptr<IR> {compiler_class_name}::compile(const std::string &src, std::shared_ptr<Accelerator> acc) {{
            accelerator = acc;
            return compile(src);
        }}

        std::shared_ptr<IR> {compiler_class_name}::compile(const std::string &src) {{
            auto ir = xacc::getService<IRProvider>("gate")->createIR();

            // TODO: Add kernel(s) to IR

            return ir;
        }}

        const std::string {compiler_class_name}::translate(const std::string &bufferVariable, std::shared_ptr<Function> function) {{
            // TODO: Implement this
            return "";
        }}
    }}
}}
