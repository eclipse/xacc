
#ifndef QUANTUM_GATE_ACCELERATORS_RIGETTI_QUILVISITOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_RIGETTI_QUILVISITOR_HPP_

#include <memory>
#include "Hadamard.hpp"
#include "Measure.hpp"
#include "CNOT.hpp"
#include "Rz.hpp"
#include "Z.hpp"
#include "X.hpp"
#include "ConditionalFunction.hpp"
#include "Z.hpp"

namespace xacc {
namespace quantum {

class QuilVisitor: public BaseInstructionVisitor,
		public InstructionVisitor<CNOT>,
		public InstructionVisitor<Hadamard>,
		public InstructionVisitor<X>,
		public InstructionVisitor<Z>,
		public InstructionVisitor<Measure>,
		public InstructionVisitor<ConditionalFunction> {
protected:

	std::string quilStr;
	std::string classicalAddresses;
public:
	QuilVisitor() {
	}

	void visit(Hadamard& h) {
		quilStr += "H " + std::to_string(h.bits()[0]) + "\n";
	}

	void visit(CNOT& cn) {
		quilStr += "CNOT " + std::to_string(cn.bits()[0]) + " " + std::to_string(cn.bits()[1]) + "\n";
	}

	void visit(X& x) {
		quilStr += "X " + std::to_string(x.bits()[0]) + "\n";
	}

	void visit(Z& z) {
		quilStr += "Z " + std::to_string(z.bits()[0]) + "\n";
	}

	void visit(Measure& m) {
		int classicalBitIdx = m.getParameter(0);
		quilStr += "MEASURE " + std::to_string(m.bits()[0]) + " [" + std::to_string(classicalBitIdx) + "]\n";
		classicalAddresses += std::to_string(classicalBitIdx) + ", ";
	}

	void visit(ConditionalFunction& c) {
		auto visitor = std::make_shared<QuilVisitor>();
		quilStr += "JUMP-UNLESS @" + c.getName() + " [" + std::to_string(c.getConditionalQubit()) + "]\n";
		for (auto inst : c.getInstructions()) {
			inst->accept(visitor);
		}
		quilStr += visitor->getQuilString();
		quilStr += "LABEL @" + c.getName() + "\n";
	}

	std::string getQuilString() {
		return quilStr;
	}

	std::string getClassicalAddresses() {
		auto retStr = classicalAddresses.substr(0, classicalAddresses.size() - 2);
		return "[" + retStr + "]";
	}

	virtual ~QuilVisitor() {}
};


}
}

#endif /* QUANTUM_GATE_ACCELERATORS_RIGETTI_QUILVISITOR_HPP_ */
