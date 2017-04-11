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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ProgramTester

#include <boost/test/included/unit_test.hpp>
#include "Program.hpp"
#include "FakeIR.hpp"
#include "FakeAccelerator.hpp"
#include "FireTensorAccelerator.hpp"

using namespace xacc;

class DummyCompiler : public Compiler {
public:
	virtual std::shared_ptr<xacc::IR> compile(const std::string& src,
			std::shared_ptr<IAccelerator> acc) {
		return std::make_shared<FakeIR>();

	}

	virtual std::shared_ptr<xacc::IR> compile(const std::string& src) {
		return std::make_shared<FakeIR>();

	}

	virtual void modifySource() {

	}
	virtual std::string getBitType() {
		return "hello";
	}

	virtual ~DummyCompiler() {
	}
};

// Register the ScaffoldCompiler with the CompilerRegistry.
static xacc::RegisterCompiler<DummyCompiler> X("dummy");

BOOST_AUTO_TEST_CASE(checkBuildRuntimeArguments) {

	const std::string src("__qpu__ void teleport() {"
					"       qbit q[3];"
					"		H(q[1]);"
					"		CNot(q[1],q[2]);"
					"		CNot(q[0], q[1]);"
					"		H(q[1]);"
					"		if(q[1].measure()) {"
					"			X(q[2]);"
					"		}"
					"		if(q[1].measure()) {"
					"			Z(q[2]);"
					"		}"
					"}");

	auto acc = std::make_shared<FakeAccelerator>();
	acc->createBuffer("qreg");
	Program prog(acc, src);
	prog.build("--compiler dummy");
}

BOOST_AUTO_TEST_CASE(checkRuntimeGateParameter) {

	// Quantum Kernel executing a parameterized gate
//	const std::string src("__qpu__ rotate (qbit qreg, double phi) {\n"
//			"   H(qreg[0]);\n"
//			"   Rz(qreg[0], phi);\n"
//			"}\n");
//
//	// Create a convenient alias for our simulator...
//	using CircuitSimulator = xacc::quantum::FireTensorAccelerator<6>;
//
//	// Create a reference to the 6 qubit simulation Accelerator
//	auto qpu = std::make_shared<CircuitSimulator>();
//
//	// Allocate 1 qubit, give them a unique identifier...
//	auto qubitReg = qpu->createBuffer("qreg", 1);
//	using QubitRegisterType = decltype(qubitReg);
//
//	// Construct a new XACC Program
//	xacc::Program quantumProgram(qpu, src);
//
//	// Build the program using Scaffold comipler
//	// and output the Graph Intermediate Representation
//	quantumProgram.build("--compiler scaffold");
//
//	// Retrieve the created kernel. It takes a
//	// qubit register as input
//	auto rotate = quantumProgram.getKernel<QubitRegisterType, double&>(
//			"teleport");
//
//	// Execute the kernel with the qubit register!
//	double angle = std::acos(-1) / 4.0;
//	rotate(qubitReg, angle);
//
//	// Pretty print the resultant state
//	qubitReg->printBufferState(std::cout);
//
//	BOOST_VERIFY(std::real(qubitReg->getState()(0)) == (1.0/std::sqrt(2.0)));
//	BOOST_VERIFY(std::real(qubitReg->getState()(1)) == 0.5);
//	BOOST_VERIFY(std::fabs(std::imag(qubitReg->getState()(1)) - 0.5) < 1e-6);

}

BOOST_AUTO_TEST_CASE(checkTeleportScaffold) {
	// Quantum Kernel executing teleportation of
	// qubit state to another.
	const std::string src("__qpu__ teleport (qbit qreg) {\n"
			"   cbit creg[2];\n"
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
			"}\n");

	// Create a convenient alias for our simulator...
	using CircuitSimulator = xacc::quantum::FireTensorAccelerator<6>;

	// Create a reference to the 6 qubit simulation Accelerator
	auto qpu = std::make_shared<CircuitSimulator>();

	// Allocate 3 qubits, give them a unique identifier...
	auto qreg = qpu->createBuffer("qreg", 3);
	using QubitRegisterType = decltype(qreg);

	// Construct a new XACC Program
	xacc::Program quantumProgram(qpu, src);

	// Build the program using Scaffold comipler
	// and output the Graph Intermediate Representation
	quantumProgram.build("--compiler scaffold");

	// Retrieve the created kernel. It takes a
	// qubit register as input
	auto teleport = quantumProgram.getKernel<QubitRegisterType>("teleport");

	// Execute the kernel with the qubit register!
	teleport(qreg);

	// Pretty print the resultant state
	qreg->printBufferState(std::cout);

	BOOST_VERIFY(std::real(qreg->getState()(1) * qreg->getState()(1)) == 1 ||
				std::real(qreg->getState()(5) * qreg->getState()(5)) == 1 ||
				std::real(qreg->getState()(3) * qreg->getState()(3)) == 1 ||
				std::real(qreg->getState()(7) * qreg->getState()(7)) == 1);

}
