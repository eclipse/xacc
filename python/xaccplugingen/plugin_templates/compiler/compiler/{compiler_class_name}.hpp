#ifndef IMPLS_{compiler_class_name_upper}_HPP
#define IMPLS_{compiler_class_name_upper}_HPP

#include "Compiler.hpp"

namespace xacc {{
    namespace quantum {{
        class {compiler_class_name} : public xacc::Compiler {{
        public:
            {compiler_class_name}();

            virtual std::shared_ptr<xacc::IR> compile(const std::string &src, std::shared_ptr<Accelerator> acc);

            virtual std::shared_ptr<xacc::IR> compile(const std::string &src);

            virtual const std::string translate(const std::string &bufferVariable, std::shared_ptr<Function> function);

            virtual const std::string name() const {{
                return "{compiler_class_name_lower}";
            }}

            virtual const std::string description() const {{
                // TODO: Write a description of the compiler here
                return "";
            }}

            virtual ~{compiler_class_name}() {{}}
        }}
    }}
}}

#endif
