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
#define BOOST_TEST_MODULE ScaffoldCompilerTester

#include <boost/test/included/unit_test.hpp>
#include "ScaffoldCompiler.hpp"
#include "GateQIR.hpp"
#include "JsonVisitor.hpp"

using namespace xacc::quantum;

struct F {
	F() :
			compiler(std::make_shared<ScaffoldCompiler>()) {
		BOOST_TEST_MESSAGE("setup fixture");
		BOOST_VERIFY(compiler);
	}
	~F() {
		BOOST_TEST_MESSAGE("teardown fixture");
	}

	std::shared_ptr<xacc::Compiler> compiler;
};

//____________________________________________________________________________//

BOOST_FIXTURE_TEST_SUITE( s, F )

BOOST_AUTO_TEST_CASE(checkSimpleCompile) {

	const std::string src("__qpu__ teleport (qbit qreg[3]) {\n"
			"   cbit creg[2];\n"
			"   // Init qubit 0 to 1\n"
			"   X(qreg[0]);\n"
			"   // Now teleport...\n"
			"   H(qreg[1]);\n"
			"   CNOT(qreg[1],qreg[2]);\n"
			"   CNOT(qreg[0],qreg[1]);\n"
			"   H(qreg[0]);\n"
			"}\n");

	auto qir = compiler->compile(src);

	auto gateqir = std::dynamic_pointer_cast<GateQIR>(qir);

	BOOST_VERIFY(gateqir->numberOfKernels() == 1);

	auto k = gateqir->getKernel("teleport");
	BOOST_VERIFY(k->nInstructions() == 5);

	std::cout << "STR: \n\n" << k->toString("qreg") << "\n";
}

BOOST_AUTO_TEST_CASE(checkWithRzParameterized) {
	const std::string src("__qpu__ teleport (qbit qreg[3]) {\n"
			"   Rz(qreg[0], 3.1415);\n"
			"}\n");

	auto qir = compiler->compile(src);
	auto gateqir = std::dynamic_pointer_cast<GateQIR>(qir);

	BOOST_VERIFY(gateqir->numberOfKernels() == 1);

	auto k = gateqir->getKernel("teleport");
	BOOST_VERIFY(k->nInstructions() == 1);
}

BOOST_AUTO_TEST_CASE(checkWithMeasurementIf) {

	const std::string src("module teleport (qbit qreg[3]) {\n"
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

	auto qir = compiler->compile(src);
	auto gateqir = std::dynamic_pointer_cast<GateQIR>(qir);

	BOOST_VERIFY(gateqir->numberOfKernels() == 1);

	auto k = gateqir->getKernel("teleport");

}

BOOST_AUTO_TEST_CASE(checkWithParameter) {
	const std::string src("module gateWithParam (qbit qreg[3], double phi) {\n"
			"   // Init qubit 0 to 1\n"
			"   X(qreg[0]);\n"
			"   // Now teleport...\n"
			"   H(qreg[1]);\n"
			"   Rz(qreg[2], phi);\n"
			"}\n");

	auto qir = compiler->compile(src);
	auto gateqir = std::dynamic_pointer_cast<GateQIR>(qir);
	auto f = gateqir->getKernel("gateWithParam");

	BOOST_VERIFY(f->nInstructions() == 3);
	BOOST_VERIFY(f->nParameters() == 1);

	gateqir->persist(std::cout);

	xacc::InstructionParameter p(3.14);
	std::vector<xacc::InstructionParameter> params { p };
	f->evaluateVariableParameters(params);

	std::cout << "\n\n";
	gateqir->persist(std::cout);
}

BOOST_AUTO_TEST_CASE(checkTwoFunctions) {
	const std::string src("module init(qbit qreg[3], double phi) {\n"
			"   Rz(qreg[0], phi);\n"
			"}\n"
			"\n"
			"module gateWithParam (qbit qreg[3], double phi) {\n"
			"   init(qreg, phi);\n"
			"   H(qreg[1]);\n"
			"   X(qreg[0]);\n"
			"}\n");

	auto qir = compiler->compile(src);

	qir->persist(std::cout);

}
BOOST_AUTO_TEST_CASE(checkTeleportWithFunctions) {

	const std::string src2("module init(qbit qreg[3]) {\n"
			"   X(qreg[0]);\n"
			"}\n"
			"module createBellPair(qbit qreg[3]) {\n"
			"   H(qreg[1]);\n"
			"   CNOT(qreg[1], qreg[2]);\n"
			"}\n"
			"module teleport (qbit qreg[3]) {\n"
			"   cbit creg[2];\n"
			"   // Init qubit 0 to 1\n"
			"   init(qreg);\n"
			"   // create bell pair\n"
			"   createBellPair(qreg);\n"
			"   // Now teleport...\n"
			"   CNOT(qreg[0],qreg[1]);\n"
			"   H(qreg[0]);\n"
			"   creg[0] = MeasZ(qreg[0]);\n"
			"   creg[1] = MeasZ(qreg[1]);\n"
			"   if (creg[0] == 1) Z(qreg[2]);\n"
			"   if (creg[1] == 1) X(qreg[2]);\n"
			"}\n");

	auto qir2 = compiler->compile(src2);

	qir2->persist(std::cout);
}


BOOST_AUTO_TEST_CASE(checkVQEExample) {
	const std::string src(""
		"__qpu__ initializeState(qbit qreg[2], float theta) {\n"
		"   Rx(qreg[0], 1.57079);\n"
		"   Ry(qreg[1], 1.57079);\n"
		"   Rx(qreg[0], 7.8539752);\n"
		"   CNOT(qreg[1], qreg[0]);\n"
		"   Rz(qreg[0], theta);\n"
		"   CNOT(qreg[1], qreg[0]);\n"
		"   Ry(qreg[1], 7.8539752);\n"
		"   Rx(qreg[0], 1.57079);\n"
		"}\n"
		""
		"__qpu__ g1Term (qbit qreg[2], float theta) {\n"
		"   initializeState(qreg, theta);\n"
		"   cbit creg[2];\n"
		"   creg[1] = MeasZ(qreg[1]);\n"
		"   creg[0] = MeasZ(qreg[0]);\n"
		"}\n"
		""
		"__qpu__ g4Term(qbit qreg[2], float theta) {\n"
		"   initializeState(qreg, theta);\n"
		"   cbit creg[2];\n"
		"   Rx(qreg[0], -1.57079);\n"
		"   Rx(qreg[1], -1.57079);\n"
		"   creg[0] = MeasZ(qreg[0]);\n"
		"   creg[1] = MeasZ(qreg[1]);\n"
		"}\n"
		""
		"__qpu__ g5Term(qbit qreg[2], float theta) {\n"
		"   initializeState(qreg, theta);\n"
		"   cbit creg[2];\n"
		"   H(qreg[0]);\n"
		"   H(qreg[1]);\n"
		"   creg[0] = MeasZ(qreg[0]);\n"
		"   creg[1] = MeasZ(qreg[1]);\n"
		"}\n");

	ScaffoldCompiler compiler;
	auto ir = compiler.compile(src);
	JsonVisitor visitor(ir->getKernels());

	std::cout << "HI:\n" << visitor.write() << "\n";


	auto k = ir->getKernel("initializeState");

	xacc::InstructionParameter p(22.2);
	std::vector<xacc::InstructionParameter> pars{p};
	k->evaluateVariableParameters(pars);

	JsonVisitor v(k);

	std::cout << "AfterParams:\n" << v.write() << "\n";

	xacc::InstructionParameter p2(44.2);
	std::vector<xacc::InstructionParameter> pars2{p2};
	k->evaluateVariableParameters(pars2);

	JsonVisitor v2(k);

	std::cout << "AfterParams2:\n" << v2.write() << "\n";


}

/*
 BOOST_AUTO_TEST_CASE(checkMultipleFunction) {
 const std::string src(
 "module PhasePi8 (qbit bit[1]) {\n"
 "   Rz(bit[0], -1*3.1415/8);\n"
 "}\n"
 "module cT(qbit ctrl[1], qbit target[1]) {\n"
 "  PhasePi8(ctrl[0]);\n"
 //		"  Rz(target[0], 3.1415/8.0);\n"
 //		"  CNOT(target[0], ctrl[0]);\n"
 //		"  Rz(target[0], -1*3.1415/8);"
 //		"  CNOT(target[0], ctrl[0]);\n"
 "}\n"
 "module cS( qbit ctrl[1], qbit target[1]) {\n"
 //		"  T(ctrl[0]);\n"
 //		"  Rz(target[0], 3.1415/4);\n"
 //		"  CNOT(target[0], ctrl[0]);\n"
 //		"  Rz(target[0], -1*3.1415/4);\n"
 //		"  CNOT(target[0], ctrl[0]);\n"
 "}\n"
 "module cRz(qbit ctrl[1], qbit target[1], const double angle) {\n"
 //		"  Rz(target[0], -1*angle/2);\n"
 //		"  CNOT(target[0], ctrl[0]);\n"
 //		"  Rz(target[0], angle/2);\n"
 //		"  CNOT(target[0], ctrl[0]);\n"
 "}\n"
 "module qft5(qbit bit[5]) {\n"
 //		"  H(bit[0]);\n"
 //		"  cS(bit[0], bit[1]);\n"
 //		"  H(bit[1]);\n"
 //		"  cT(bit[0], bit[2]);\n"
 //		"  cS(bit[1], bit[2]);\n"
 //		"  H(bit[2]);\n"
 //		"  cRz(bit[0], bit[3], 3.1415/8);\n"
 //		"  cT(bit[1], bit[3]);\n"
 //		"  cS(bit[2], bit[3]);\n"
 //		"  H(bit[3]);\n"
 //		"  cRz(bit[0], bit[4], 3.1415/16);\n"
 //		"  cRz(bit[0], bit[4], 3.1415/8);\n"
 //		"  cT(bit[2], bit[4]);\n"
 //		"  cS(bit[3], bit[4]);\n"
 //		"  H(bit[4]);\n"
 "}");

 auto qir = compiler->compile(src);
 auto gateqir = std::dynamic_pointer_cast<GateQIR>(qir);

 std::stringstream ss;

 qir->persist(ss);
 std::cout << "HELLO:\n" << ss.str() << "\n";

 }*/

BOOST_AUTO_TEST_SUITE_END()
