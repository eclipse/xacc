/***********************************************************************************
 * Copyright (c) 2016, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#include "XACC.hpp"
#include "EigenAccelerator.hpp"

/**
 * FIXME For now, create a fake accelerator
 * This will later come from QVM
 */
class IBM5Qubit: public xacc::quantum::QPUGate<5> {
public:
	virtual xacc::AcceleratorType getType() {
		return xacc::AcceleratorType::qpu_gate;
	}
	virtual std::vector<xacc::IRTransformation> getIRTransformations() {
		std::vector<xacc::IRTransformation> v;
		return v;
	}
	virtual void execute(const std::shared_ptr<xacc::IR> ir) {
	}
	virtual ~IBM5Qubit() {
	}

protected:
	bool canAllocate(const int N) {
		return true;
	}
};

// Quantum Kernel executing teleportation of
// qubit state to another.
const std::string src("__qpu__ teleport () {\n"
						"   // Initialize qubit to 1\n"
						"   X(qreg[0]);\n"
						"   H(qreg[1]);\n"
						"   CNOT(qreg[1],qreg[2]);\n"
						"   CNOT(qreg[0],qreg[1]);\n"
						"   H(qreg[0]);\n"
						"   MeasZ(qreg[0]);\n"
						"   MeasZ(qreg[1]);\n"
						"   // cZ\n"
						"   H(qreg[2]);\n"
						"   CNOT(qreg[2], qreg[1]);\n"
						"   H(qreg[2]);\n"
						"   // cX = CNOT\n"
						"   CNOT(qreg[2], qreg[0]);\n"
						"}\n");

int main (int argc, char** argv) {

	// Create a reference to the IBM5Qubit Accelerator
	auto ibm_qpu = std::make_shared<xacc::quantum::EigenAccelerator<3>>();

	// Allocate some qubits, give them a unique identifier...
	auto qreg = ibm_qpu->allocate("qreg");

	// Construct a new Program
	xacc::Program quantumProgram(ibm_qpu, src);

	// Build the program
	quantumProgram.build("--compiler scaffold --writeIR teleport.xir");

	// Retrieve the constructed kernel
	auto teleport = quantumProgram.getKernel("teleport");

	// Execute the kernel!
	teleport();

	// Get the execution result
	qreg->printState(std::cout);
	auto bits = qreg->toBits();

	return 0;
}



