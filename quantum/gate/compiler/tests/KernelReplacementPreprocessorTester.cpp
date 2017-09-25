/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE KernelReplacementPPTester
#include <boost/test/included/unit_test.hpp>
#include "KernelReplacementPreprocessor.hpp"
#include "Compiler.hpp"
#include "Accelerator.hpp"
#include "GateQIR.hpp"
#include "XACC.hpp"
#include "ServiceRegistry.hpp"

using namespace xacc;

using namespace xacc::quantum;

class DummyCompiler: public xacc::Compiler {

public:
	virtual std::shared_ptr<IR> compile(const std::string& src,
			std::shared_ptr<Accelerator> acc) {
		return std::make_shared<GateQIR>();
	}

	virtual std::shared_ptr<IR> compile(const std::string& src) {
		return std::make_shared<GateQIR>();
	}

	virtual const std::string translate(const std::string& bufferVariable,
			std::shared_ptr<Function> function) {
		std::string translated = "H(qreg[2]);\n"
				"// BEGIN CPHASE GATE\n"
				"Rz(qreg[2],0.785398);\n"
				"CNOT(qreg[2],qreg[1]);\n"
				"Rz(qreg[2],-0.785398);\n"
				"CNOT(qreg[2],qreg[1]);\n"
				"// END CPHASE GATE\n"
				"H(qreg[2]);\n"
				"// BEGIN CPHASE GATE\n"
				"Rz(qreg[2],0.392699);\n"
				"CNOT(qreg[2],qreg[0]);\n"
				"Rz(qreg[2],-0.392699);\n"
				"CNOT(qreg[2],qreg[0]);\n"
				"// END CPHASE GATE\n"
				"// BEGIN CPHASE GATE\n"
				"Rz(qreg[1],0.785398);\n"
				"CNOT(qreg[1],qreg[0]);\n"
				"Rz(qreg[1],-0.785398);\n"
				"CNOT(qreg[1],qreg[0]);\n"
				"// END CPHASE GATE\n"
				"H(qreg[0]);\n"
				"// BEGIN SWAP 0 2\n"
				"CNOT(qreg[2],qreg[0]);\n"
				"CNOT(qreg[0],qreg[2]);\n"
				"CNOT(qreg[2],qreg[0]);\n"
				"// END SWAP 0 2\n";

		return translated;
	}

	virtual const std::string getName() {
		return "Dummy";
	}

	virtual const std::string name() const {
		return "";
	}

	virtual const std::string description() const {
		return "";
	}


};

class DummyAccelerator : public xacc::Accelerator {
public:

	virtual AcceleratorType getType() {
		return AcceleratorType::qpu_gate;
	}

	virtual void initialize() {

	}

	virtual std::vector<std::shared_ptr<IRTransformation>> getIRTransformations() {
		std::vector<std::shared_ptr<IRTransformation>> ts;
		return ts;
	}

	virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
				const std::shared_ptr<Function> function) {

	}

	virtual std::shared_ptr<AcceleratorBuffer> createBuffer(
			const std::string& varId, const int size) {
		auto b = std::make_shared<AcceleratorBuffer>(varId, 3);
		storeBuffer(varId, b);
		return b;
	}

	virtual std::shared_ptr<AcceleratorBuffer> createBuffer(
				const std::string& varId) {
			auto b = std::make_shared<AcceleratorBuffer>(varId, 3);
			storeBuffer(varId, b);
			return b;
		}
	virtual bool isValidBufferSize(const int NBits) {
		return true;
	}

	virtual const std::string name() const {
		return "";
	}

	virtual const std::string description() const {
		return "";
	}


};

BOOST_AUTO_TEST_CASE(checkSimple) {
	auto testPath = std::string(XACC_BUILD_DIR) + std::string("/stage/usr/local/xacc/lib/libxacc-quantum-gate.so");

        xacc::Initialize(std::vector<std::string>{"--load",testPath});
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
			"   xacc::QFT(qreg);\n"
			"}\n");

	KernelReplacementPreprocessor preprocessor;
	auto compiler = std::make_shared<DummyCompiler>();
	auto acc = std::make_shared<DummyAccelerator>();
	acc->createBuffer("qreg", 3);

	auto newSrc = preprocessor.process(src, compiler, acc);

	std::string expected("module teleport (qbit qreg[3]) {\n"
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
			"   H(qreg[2]);\n"
			"   // BEGIN CPHASE GATE\n"
			"   Rz(qreg[2],0.785398);\n"
			"   CNOT(qreg[2],qreg[1]);\n"
			"   Rz(qreg[2],-0.785398);\n"
			"   CNOT(qreg[2],qreg[1]);\n"
			"   // END CPHASE GATE\n"
			"   H(qreg[2]);\n"
			"   // BEGIN CPHASE GATE\n"
			"   Rz(qreg[2],0.392699);\n"
			"   CNOT(qreg[2],qreg[0]);\n"
			"   Rz(qreg[2],-0.392699);\n"
			"   CNOT(qreg[2],qreg[0]);\n"
			"   // END CPHASE GATE\n"
			"   // BEGIN CPHASE GATE\n"
			"   Rz(qreg[1],0.785398);\n"
			"   CNOT(qreg[1],qreg[0]);\n"
			"   Rz(qreg[1],-0.785398);\n"
			"   CNOT(qreg[1],qreg[0]);\n"
			"   // END CPHASE GATE\n"
			"   H(qreg[0]);\n"
			"   // BEGIN SWAP 0 2\n"
			"   CNOT(qreg[2],qreg[0]);\n"
			"   CNOT(qreg[0],qreg[2]);\n"
			"   CNOT(qreg[2],qreg[0]);\n"
			"   // END SWAP 0 2\n"
			"   \n"
			"}\n");

	BOOST_VERIFY(newSrc == expected);
        xacc::Finalize();
}
