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
#include "Program.hpp"

/**
 * FIXME For now, create a fake accelerator
 * This will later come from QVM
 */
class IBM5Qubit: public xacc::Accelerator {
public:
	virtual AcceleratorType getType() {
		return AcceleratorType::qpu_gate;
	}
	virtual std::vector<xacc::IRTransformation> getIRTransformations() {
		std::vector<xacc::IRTransformation> v;
		return v;
	}
	virtual ~IBM5Qubit() {
	}
};

// Quantum Kernel executing teleportation of
// qubit state to another.
const std::string src("__qpu__ teleport () {\n"
						"   qbit q[3];\n"
						"   cbit c[2];\n"
						"   H(q[1]);\n"
						"   CNOT(q[1],q[2]);\n"
						"   CNOT(q[0],q[1]);\n"
						"   H(q[0]);\n"
						"   MeasZ(q[0]);\n"
						"   MeasZ(q[1]);\n"
						"   // cZ\n"
						"   H(q[2]);\n"
						"   CNOT(q[2], q[1]);\n"
						"   H(q[2]);\n"
						"   // cX = CNOT\n"
						"   CNOT(q[2], q[0]);\n"
						"}\n");

int main (int argc, char** argv) {

	auto ibm_qpu = std::make_shared<IBM5Qubit>();
	xacc::Program quantumProgram(ibm_qpu, src);
	quantumProgram.build("--compiler scaffold --writeIR teleport.xir");

	// FIXME Get Kernel, execute, get result

	return 0;
}



