#ifndef QUANTUM_GATE_ACCELERATORS_IBMIRTransformation_HPP_
#define QUANTUM_GATE_ACCELERATORS_IBMIRTransformation_HPP_

#include "Accelerator.hpp"
#include "IRTransformation.hpp"
#include "GateQIR.hpp"
#include "CNOT.hpp"

namespace xacc {
namespace quantum {

class IBMIRTransformation: public IRTransformation,
		public BaseInstructionVisitor,
		public InstructionVisitor<CNOT> {

protected:

	std::vector<std::pair<int, int>> _couplers;

	int currentKernelInstructionIdx;

	std::vector<std::shared_ptr<Instruction>> newInstructions;

	bool isCouplingAvailable(const int src, const int tgt);

public:

	IBMIRTransformation(std::vector<std::pair<int, int>> couplers) :
			_couplers(couplers), currentKernelInstructionIdx(0) {
	}

	virtual std::shared_ptr<IR> transform(std::shared_ptr<IR> ir);

	void visit(CNOT& cnot);

	virtual const std::string name() const {
		return "ibm-ir-transformations";
	}

	virtual const std::string description() const {
		return "";
	}
};
}
}



#endif
