#ifndef IMPLS_{irt_class_name_upper}_HPP
#define IMPLS_{irt_class_name_upper}_HPP

#include "IRTransformation.hpp"
#include "OptionsProvider.hpp"

#include <boost/program_options.hpp>
using namespace boost::program_options;

namespace xacc {{
    namespace quantum {{
        class {irt_class_name} : public IRTransformation, public OptionsProvider {{
        public:
            {irt_class_name}();

            std::shared_ptr<IR> transform(std::shared_ptr<IR> ir) override;

            const std::string name() const override {{
                return "{irt_class_name_lower}";
            }}

            const std::string description() const override {{
                // TODO: Write a description of the transformation here
                return "";
            }}

            std::shared_ptr<options_description> getOptions() override {{
                // FIXME uncomment to use
                 auto desc =
                      std::make_shared<options_description>("{irt_class_name} Options");
                //  desc->add_options()("opt1", value<std::string>(),
                //       "opt1 description")("opt2","opt2 description");
                 return desc;
            }}

            bool handleOptions(variables_map &map) override {{ return false; }}

            virtual ~{irt_class_name}() {{}}
        }};
    }}
}}

#endif
