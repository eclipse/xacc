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

class FakeAccelerator : public Accelerator {

public:

	virtual AcceleratorType getType() { return qpu_gate; }

	virtual std::vector<IRTransformation> getIRTransformations() {std::vector<IRTransformation> v; return v;}

	virtual ~FakeAccelerator() {}

};

class FakeIR: public IR {
public:
	FakeIR() {
	}
	virtual std::string toString() { return std::string();}
	virtual void persist(std::ostream& stream) {}
};

class DummyCompiler : public Compiler<DummyCompiler> {
public:
	virtual std::shared_ptr<IR> compile() {
		return std::make_shared<FakeIR>();
	}

	virtual void modifySource() {

	}

	virtual ~DummyCompiler() {
	}
};

REGISTER_QCIOBJECT_WITH_QCITYPE(DummyCompiler, "compiler",
		"dummy");

BOOST_AUTO_TEST_CASE(checkBuildRuntimeArguments) {

	const std::string src("__qpu__ void teleport() {"
					"       qbit qs[3];"
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

	Program prog(acc, src);
	prog.build("--compiler dummy");

//	auto kernel = prog.getKernel("teleport");
}

