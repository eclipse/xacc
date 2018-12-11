#ifndef IMPLS_{inst_class_name_upper}_HPP
#define IMPLS_{inst_class_name_upper}_HPP

#include "GateInstruction.hpp"

namespace xacc {{
    namespace quantum {{
        class {inst_class_name} : public GateInstruction {{
        public:
            {inst_class_name}() : GateInstruction(name()) {{}}
            {inst_class_name}(std::vector<int> qbits) : GateInstruction(name(),qbits) {{}}
            {inst_class_name}(int src, int tgt) : {inst_class_name}(std::vector<int>{{src,tgt}}) {{}}

            DEFINE_CLONE({inst_class_name})
            DEFINE_VISITABLE()
            
            const std::string name() const override {{
                // This should be the name you'd see in code, 
                // like CNOT or CX for a controlled not gate instruction
                return "{inst_class_name_lower}"; 
            }}

            const std::string description() const override {{
                // TODO: Write a description of the transformation here
                return "";
            }}

            virtual ~{inst_class_name}() {{}}
        }};
    }}
}}

#endif
