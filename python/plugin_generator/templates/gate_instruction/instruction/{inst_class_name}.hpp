#ifndef IMPLS_{inst_class_name_upper}_HPP
#define IMPLS_{inst_class_name_upper}_HPP

#include "GateInstruction.hpp"

namespace xacc {{
    namespace quantum {{
        class {inst_class_name} : public GateInstruction {{
        protected:

            void describeOptions() override {{
                // Here, fill the GateInstruction options map
                // std::vector<std::pair<double,double>> samples {{{{0.0,0.0}}, {{1.0,0.0}}}};
                // options.insert({{"samples", InstructionParameter(samples)}});
            }}
            
        public:
            {inst_class_name}() : GateInstruction(name()) {{}}
            {inst_class_name}(std::vector<int> qbits) : GateInstruction(name(),qbits) {{}}

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
            
            void customVisitAction(BaseInstructionVisitor& iv) override {{
                // From here you have access to the native assembly 
                // (usually JSON) string being built up for the 
                // target Accelerator...
                // iv.getNativeAssembly() += "hello";
            }}
  
            const bool isAnalog() const override {{ 
                // Indicate if this is a 
                // digital or analog gate instruction
                return false; 
            }}

            virtual ~{inst_class_name}() {{}}
        }};
    }}
}}

#endif
