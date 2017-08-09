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

const std::string src(""
	"__qpu__ initializeState(qbit qreg, float theta) {\n"
	"   Rx(qreg[0], 3.1415926);\n"
	"   Ry(qreg[1], 1.57079);\n"
	"   Rx(qreg[0], 7.8539752);\n"
	"   CNOT(qreg[1], qreg[0]);\n"
	"   Rz(qreg[0], theta);\n"
	"   CNOT(qreg[1], qreg[0]);\n"
	"   Ry(qreg[1], 7.8539752);\n"
	"   Rx(qreg[0], 1.57079);\n"
	"}\n"
	""
	"__qpu__ g1Term (qbit qreg, float theta) {\n"
	"   initializeState(qreg, theta);\n"
	"   cbit creg[1];\n"
	"   creg[0] = MeasZ(qreg[0]);\n"
	"}\n"
	""
	"__qpu__ g2Term (qbit qreg, float theta) {\n"
	"   initializeState(qreg, theta);\n"
	"   cbit creg[1];\n"
	"   creg[0] = MeasZ(qreg[1]);\n"
	"}\n"
	"__qpu__ g3Term (qbit qreg, float theta) {\n"
	"   initializeState(qreg, theta);\n"
	"   cbit creg[2];\n"
	"   creg[1] = MeasZ(qreg[1]);\n"
	"   creg[0] = MeasZ(qreg[0]);\n"
	"}\n"
	"__qpu__ g4Term(qbit qreg, float theta) {\n"
	"   initializeState(qreg, theta);\n"
	"   cbit creg[2];\n"
	"   Rx(qreg[1], 1.57079);\n"
	"   Rx(qreg[0], 1.57079);\n"
	"   creg[1] = MeasZ(qreg[1]);\n"
	"   creg[0] = MeasZ(qreg[0]);\n"
	"}\n"
	""
	"__qpu__ g5Term(qbit qreg, float theta) {\n"
	"   initializeState(qreg, theta);\n"
	"   cbit creg[2];\n"
	"   H(qreg[1]);\n"
	"   creg[1] = MeasZ(qreg[1]);\n"
	"   H(qreg[0]);\n"
	"   creg[0] = MeasZ(qreg[0]);\n"
	"}\n"
	"");

int main (int argc, char** argv) {

	// Initialize the XACC Framework
	xacc::Initialize(argc, argv);

	// Create a reference to the Rigetti 
	// QPU at api.rigetti.com/qvm
	auto qpu = xacc::getAccelerator("rigetti");

	// Allocate a register of 3 qubits
	auto qubitReg = qpu->createBuffer("qreg", 2);

	// Create a Program
	xacc::Program program(qpu, src);

	// Request the quantum kernel representing
	// the above source code
	auto g1Term = program.getKernel<float>("g1Term");
	auto g2Term = program.getKernel<float>("g2Term");
	auto g3Term = program.getKernel<float>("g3Term");
	auto g4Term = program.getKernel<float>("g4Term");
	auto g5Term = program.getKernel<float>("g5Term");

	std::ofstream file("out.csv");
	file << "Angle, Z0, Z1, Z0Z1, Y0Y1, X0X1\n";
	auto pi = 3.14159265359;
	double g0 = 1.0, g1 = 1.0, g2 = 1.0, g3 = 1.0, g4 = 1.0, g5 = 1.0;
	for (float theta = -pi; theta <= pi; theta += .2) {

		g1Term(qubitReg, theta);
		auto e1 = qubitReg->getExpectationValueZ();
		qubitReg->resetBuffer();

		// G2 Term, same as g1 so just use it
		g2Term(qubitReg, theta);
		auto e2 = qubitReg->getExpectationValueZ();
		qubitReg->resetBuffer();

		// G3 Term, same as g1 so just use it
		g3Term(qubitReg, theta);
		auto e3 = qubitReg->getExpectationValueZ();
		qubitReg->resetBuffer();

		g4Term(qubitReg, theta);
		auto e4 = qubitReg->getExpectationValueZ();
		qubitReg->resetBuffer();

		g5Term(qubitReg, theta);
		auto e5 = qubitReg->getExpectationValueZ();
		qubitReg->resetBuffer();

		file << theta << ", " << e1 << ", " << e2 << ", " << e3 << ", " << e4 << ", " << e5 << "\n";

	}
	
	file.close();

	// Finalize the XACC Framework
	xacc::Finalize();

	return 0;
}



