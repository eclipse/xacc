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

// Quantum Kernel executing teleportation of
// qubit state to another.
// test
const std::string src("__qpu__ teleport (qbit qreg) {\n"
	"   cbit creg[3];\n"
	"   // Init qubit 0 to 1\n"
	"   X(qreg[0]);\n"
	"   // Now teleport...\n"
	"   H(qreg[1]);\n"
	"   CNOT(qreg[1],qreg[2]);\n"
	"   CNOT(qreg[0],qreg[1]);\n"
	"   H(qreg[0]);\n"
	"   creg[0] = MeasZ(qreg[0]);\n"
	"   creg[1] = MeasZ(qreg[1]);\n"
	"   if (creg[0] == 1) Z(qreg[2]);\n"
	"   if (creg[1] == 1) X(qreg[2]);\n"
	"   // Check that 3rd qubit is a 1\n"
	"   creg[2] = MeasZ(qreg[2]);\n"
	"}\n");

int main (int argc, char** argv) {

	// Initialize the XACC Framework
	xacc::Initialize(argc, argv);

	// Create a reference to the Rigetti 
	// QPU at api.rigetti.com/qvm
	auto qpu = xacc::getAccelerator("ibm");

	// Allocate a register of 3 qubits
	auto qubitReg = qpu->createBuffer("qreg", 3);

	// Create a Program
	xacc::Program program(qpu, src);

	// Request the quantum kernel representing
	// the above source code
	auto teleport = program.getKernel("teleport");

	// Execute!
	teleport(qubitReg);

	qubitReg->print(std::cout);

	// Finalize the XACC Framework
	xacc::Finalize();

	return 0;
}



