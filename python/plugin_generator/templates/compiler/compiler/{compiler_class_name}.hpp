#ifndef IMPLS_{compiler_class_name_upper}_HPP
#define IMPLS_{compiler_class_name_upper}_HPP

#include "Compiler.hpp"

namespace xacc {{
    namespace quantum {{
        class {compiler_class_name} : public xacc::Compiler {{
        public:
            {compiler_class_name}();

            std::shared_ptr<xacc::IR> compile(const std::string &src, std::shared_ptr<Accelerator> acc) override;

            std::shared_ptr<xacc::IR> compile(const std::string &src) override;

            const std::string translate(const std::string &bufferVariable, std::shared_ptr<Function> function) override;

            const std::string name() const override {{
                return "{compiler_class_name_lower}";
            }}

            const std::string description() const override {{
                // TODO: Write a description of the compiler here
                return "";
            }}

           std::shared_ptr<options_description> getOptions() override {{
                // FIXME uncomment to use
                 auto desc =
                      std::make_shared<options_description>("{compiler_class_name} Options");
                //  desc->add_options()("opt1", value<std::string>(),
                //       "opt1 description")("opt2","opt2 description");
                 return desc;
            }}

            bool handleOptions(variables_map &map) override {{ return false; }}
            
            virtual ~{compiler_class_name}() {{}}
        }};
    }}
}}

#endif
