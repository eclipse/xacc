
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
#define BOOST_TEST_MODULE QuilCompilerTester

#include <boost/test/included/unit_test.hpp>
#include "QuilCompiler.hpp"
#include "GateQIR.hpp"

using namespace xacc::quantum;

struct F {
	F() :
			compiler(std::make_shared<QuilCompiler>()) {
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

BOOST_AUTO_TEST_CASE(checkTeleportQuil) {

	const std::string src("__qpu__ teleport (qbit qreg[3]) {\n"
			"   # Prepare a bell state\n"
			"   H 1\n"
			"   CNOT 1 2\n"
			"   # Teleport qubit 0 to qubit 2\n"
			"   CNOT 0 1\n"
			"   H 0\n"
			"   MEASURE 0 [0]\n"
			"   MEASURE 1 [1]\n"
			"   JUMP-UNLESS @NOX [1]\n"
			"   X 2\n"
			"   LABEL @NOX\n"
			"   JUMP-UNLESS @NOZ [0]\n"
			"   Z 2\n"
			"   LABEL @NOZ\n"
			"}\n");

	auto ir = compiler->compile(src);
	auto qir = std::dynamic_pointer_cast<GateQIR>(ir);

	BOOST_VERIFY(qir->numberOfKernels() == 1);

//	qir->persist(std::cout);

}

BOOST_AUTO_TEST_SUITE_END()
