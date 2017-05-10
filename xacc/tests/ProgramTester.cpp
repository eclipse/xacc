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

using namespace xacc;

class FakeIR: public IR {
public:
	FakeIR() {
	}
	virtual std::string toAssemblyString(const std::string& kernelName,
			const std::string& accBufferVarName) {
		return std::string();}
	virtual void persist(std::ostream& stream) {}
	virtual void load(std::istream& inStream) {}
	virtual void addKernel(std::shared_ptr<Function> kernel) {

	}
	virtual std::shared_ptr<Function> getKernel(const std::string& name) {

	}
};

class FakeAccelerator: virtual public Accelerator {

public:

	virtual AcceleratorType getType() {
		return qpu_gate;
	}

	std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string& varId) {
		auto buffer = std::make_shared<AcceleratorBuffer>(varId);
		return buffer;
	}

	std::shared_ptr<AcceleratorBuffer> createBuffer(const std::string& varId,
			const int size) {
		if (!isValidBufferSize(size)) {
			XACCError("Invalid buffer size.");
		}
		auto buffer = std::make_shared<AcceleratorBuffer>(varId, size);
		return buffer;
	}

	virtual bool isValidBufferSize(const int NBits) {
		return NBits <= 10;
	}

	virtual std::vector<IRTransformation> getIRTransformations() {
		std::vector<IRTransformation> v;
		return v;
	}
	virtual void execute(std::shared_ptr<AcceleratorBuffer> buffer,
				const std::shared_ptr<Function> ir) {
		}
	virtual ~FakeAccelerator() {
	}
};


class DummyCompiler : public Compiler {
public:
	virtual std::shared_ptr<xacc::IR> compile(const std::string& src,
			std::shared_ptr<Accelerator> acc) {
		return std::make_shared<FakeIR>();

	}

	virtual std::shared_ptr<xacc::IR> compile(const std::string& src) {
		return std::make_shared<FakeIR>();

	}
	virtual const std::string getName() {
		return "Dummy";
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
	auto k = prog.getKernel("teleport");
//	prog.build("--compiler dummy");
}
