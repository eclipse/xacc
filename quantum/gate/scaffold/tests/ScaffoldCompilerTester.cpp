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
 *   * Neither the name of the <organization> nor the
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
#include "AbstractFactory.hpp"
#include "Compiler.hpp"
#include "QasmToGraph.hpp"
#include "GraphIR.hpp"
#include "ScaffoldCompiler.hpp"

using namespace qci::common;
using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkSimpleCompile) {
	using GraphType = Graph<CircuitNode>;

	auto compiler = qci::common::AbstractFactory::createAndCast<xacc::ICompiler>("compiler", "scaffold");
	BOOST_VERIFY(compiler);

	const std::string src("__qpu__ eprCreation () {\n"
						"   qbit qs[2];\n"
						"   H(qs[0]);\n"
						"   CNOT(qs[0],qs[1]);\n"
						"}\n");

	auto ir = compiler->compile(src);
	BOOST_VERIFY(ir);
	auto graphir = std::dynamic_pointer_cast<xacc::GraphIR<GraphType>>(ir);
	BOOST_VERIFY(graphir);

	// The above code should produce a graph
	// with 3 nodes (initial qubits state, Hadamard, and CNot),
	// with 3 edges (q0 lifeline to H, q0 lifeline from H to CNot,
	// and q1 lifeline to CNot)
	BOOST_VERIFY(graphir->order() == 3);
	BOOST_VERIFY(graphir->size() == 3);

}

BOOST_AUTO_TEST_CASE(checkAnotherSimpleCompile) {
	using GraphType = Graph<CircuitNode>;

	auto compiler = qci::common::AbstractFactory::createAndCast<xacc::ICompiler>("compiler", "scaffold");
	BOOST_VERIFY(compiler);

	const std::string src("__qpu__ threeBitQFT () {\n"
						"   qbit q[3];\n"
						"   H(q[0]);\n"
						"   T(q[0]);\n"
						"   Rz(q[1],3.1415 / 4.0);\n"
						"   CNOT(q[1], q[0]);\n"
						"   Rz(q[1], -1.0 * 3.1415 / 4.0);\n"
						"   CNOT(q[1], q[0]);\n"
						"   Rz(q[0], 3.1415 / 8.0);\n"
						"   Rz(q[2], 3.1415 / 8.0);\n"
						"   CNOT(q[2], q[0]);\n"
						"   Rz(q[2], -1.0 * 3.1415 / 4.0);\n"
						"   CNOT(q[2], q[0]);\n"
						"   H(q[1]);\n"
						"   T(q[1]);\n"
						"   Rz(q[2],3.1415 / 4.0);\n"
						"   CNOT(q[2], q[1]);\n"
						"   Rz(q[2], -1.0 * 3.1415 / 4.0);\n"
						"   CNOT(q[2], q[1]);\n"
						"   H(q[2]);\n"
						"}\n");

	auto ir = compiler->compile(src);
	BOOST_VERIFY(ir);
	auto graphir = std::dynamic_pointer_cast<xacc::GraphIR<GraphType>>(ir);
	BOOST_VERIFY(graphir);

	// Scaffold decomposes Rz into H and T gates.
	BOOST_VERIFY(graphir->order() > 1400);

}
