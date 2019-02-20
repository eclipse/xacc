#ifndef IMPLS_{irgenerator_class_name_upper}_HPP
#define IMPLS_{irgenerator_class_name_upper}_HPP

#include "IRGenerator.hpp"

namespace xacc {{
    namespace quantum {{
        class {irgenerator_class_name} : public xacc::IRGenerator {{
        public:
            {irgenerator_class_name}();

            virtual std::shared_ptr<Function> generate(
                            std::shared_ptr<AcceleratorBuffer> buffer,
                            std::vector<InstructionParameter> parameters) override;


            virtual std::shared_ptr<Function> generate(
                            std::map<std::string, InstructionParameter> parameters) override;

            const std::string name() const override {{
                return "{irgenerator_class_name_lower}";
            }}

            const std::string description() const override {{
                // TODO: Write a description of the compiler here
                return "";
            }}
            
            virtual ~{irgenerator_class_name}() {{}}
        }};
    }}
}}

#endif
