#include "GateFunction.hpp"
#include "GateInstructionVisitor.hpp"

namespace xacc {
namespace quantum {
void GateFunction::accept(std::shared_ptr<InstructionVisitor> visitor) {
	auto v = std::dynamic_pointer_cast<GateInstructionVisitor>(visitor);
	if (v) {
		v->visit(*this);
	} else {
		visitor->visit(*this);
	}
}
}
}
