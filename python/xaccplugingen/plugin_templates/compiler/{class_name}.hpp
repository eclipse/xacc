#ifndef IMPLS_{class_name_upper}_HPP
#define IMPLS_{class_name_upper}_HPP

#include "Compiler.hpp"

namespace xacc {{
    namespace quantum {{
        class {class_name} : public xacc::Compiler {{
        public:
            {class_name}();

            virtual std::shared_ptr<xacc::IR> compile(const std::string &src, std::shared_ptr<Accelerator> acc);

            virtual std::shared_ptr<xacc::IR> compile(const std::string &src);

            virtual const std::string translate(const std::string &bufferVariable, std::shared_ptr<Function> function);

            virtual const std::string name() const {{
                return "{class_name_lower}";
            }}

            virtual const std::string description() const {{
                // TODO: Write a description of the compiler here
                return "";
            }}

            virtual ~{class_name}() {{}}
        }}
    }}
}}

#endif
