
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
#define BOOST_TEST_MODULE GateQIRTester

#include <boost/test/included/unit_test.hpp>
#include "GateFunction.hpp"
#include "GateQIR.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkCreationToString) {
	const std::string expectedQasm =
			"qubit qreg0\n"
			"qubit qreg1\n"
			"qubit qreg2\n"
			"H qreg1\n"
			"CNOT qreg1,qreg2\n"
			"CNOT qreg0,qreg1\n"
			"H qreg0\n";

	auto qir = std::make_shared<GateQIR>();
	auto f = std::make_shared<GateFunction>("foo");
	auto h = std::make_shared<Hadamard>(1);
	auto cn1 = std::make_shared<CNOT>(1, 2);
	auto cn2 = std::make_shared<CNOT>(0, 1);
	auto h2 = std::make_shared<Hadamard>(0);
	f->addInstruction(h);
	f->addInstruction(cn1);
	f->addInstruction(cn2);
	f->addInstruction(h2);

	qir->addKernel(f);
	BOOST_VERIFY(qir->toAssemblyString("foo", "qreg") == expectedQasm);
}

BOOST_AUTO_TEST_CASE(checkSerialization) {
	auto qir = std::make_shared<GateQIR>();
	auto f = std::make_shared<GateFunction>("foo");

	auto x = std::make_shared<X>(0);
	auto h = std::make_shared<Hadamard>(1);
	auto cn1 = std::make_shared<CNOT>(1, 2);
	auto cn2 = std::make_shared<CNOT>(0, 1);
	auto h2 = std::make_shared<Hadamard>(0);
	auto m0 = std::make_shared<Measure>(0, 0);
	auto m1 = std::make_shared<Measure>(1,1);

	auto rz = std::make_shared<Rz>(1, 3.1415);

	auto cond1 = std::make_shared<ConditionalFunction>(0);
	auto z = std::make_shared<Z>(2);
	cond1->addInstruction(z);
	auto cond2 = std::make_shared<ConditionalFunction>(1);
	auto x2 = std::make_shared<X>(2);
	cond2->addInstruction(x2);

	f->addInstruction(x);
	f->addInstruction(h);
	f->addInstruction(cn1);
	f->addInstruction(cn2);
	f->addInstruction(h2);
	f->addInstruction(rz);
	f->addInstruction(m0);
	f->addInstruction(m1);
	f->addInstruction(cond1);
	f->addInstruction(cond2);

	qir->addKernel(f);

	std::stringstream ss;
	qir->persist(ss);

	std::cout << "HELLO: \n" << ss.str() << "\n";

}

BOOST_AUTO_TEST_CASE(checkReadGraph) {

	// Create a graph IR modeling a
	// quantum teleportation kernel
	std::string irstr = "graph G {\n"
			"node [shape=box style=filled]\n"
			"0 [label=\"Gate=InitialState,Circuit Layer=0,Gate Vertex Id=0,Gate Acting Qubits=[0 1 2],Enabled=1\"];\n"
			"1 [label=\"Gate=x,Circuit Layer=1,Gate Vertex Id=1,Gate Acting Qubits=[0],Enabled=1\"];\n"
			"2 [label=\"Gate=h,Circuit Layer=1,Gate Vertex Id=2,Gate Acting Qubits=[1],Enabled=1\"];\n"
			"3 [label=\"Gate=cnot,Circuit Layer=2,Gate Vertex Id=3,Gate Acting Qubits=[1 2],Enabled=1\"];\n"
			"4 [label=\"Gate=cnot,Circuit Layer=3,Gate Vertex Id=4,Gate Acting Qubits=[0 1],Enabled=1\"];\n"
			"5 [label=\"Gate=h,Circuit Layer=4,Gate Vertex Id=5,Gate Acting Qubits=[0],Enabled=1\"];\n"
			"6 [label=\"Gate=measure,Circuit Layer=5,Gate Vertex Id=6,Gate Acting Qubits=[0],Enabled=1\"];\n"
			"7 [label=\"Gate=measure,Circuit Layer=5,Gate Vertex Id=7,Gate Acting Qubits=[1],Enabled=1\"];\n"
			"8 [label=\"Gate=FinalState,Circuit Layer=6,Gate Vertex Id=8,Gate Acting Qubits=[0 1 2],Enabled=1\"];\n"
			"9 [label=\"Gate=conditional,Circuit Layer=0,Gate Vertex Id=9,Gate Acting Qubits=[0],Enabled=1\"];\n"
			"10 [label=\"Gate=InitialState,Circuit Layer=7,Gate Vertex Id=10,Gate Acting Qubits=[0 1 2],Enabled=0\"];\n"
			"11 [label=\"Gate=z,Circuit Layer=8,Gate Vertex Id=11,Gate Acting Qubits=[2],Enabled=0\"];\n"
			"12 [label=\"Gate=FinalState,Circuit Layer=9,Gate Vertex Id=12,Gate Acting Qubits=[0 1 2],Enabled=0\"];\n"
			"13 [label=\"Gate=conditional,Circuit Layer=0,Gate Vertex Id=13,Gate Acting Qubits=[1],Enabled=1\"];\n"
			"14 [label=\"Gate=InitialState,Circuit Layer=10,Gate Vertex Id=14,Gate Acting Qubits=[0 1 2],Enabled=0\"];\n"
			"15 [label=\"Gate=x,Circuit Layer=11,Gate Vertex Id=15,Gate Acting Qubits=[2],Enabled=0\"];\n"
			"16 [label=\"Gate=FinalState,Circuit Layer=12,Gate Vertex Id=16,Gate Acting Qubits=[0 1 2],Enabled=0\"];\n"
			"0--1 [label=\"weight=0.000000\"];\n"
			"0--2 [label=\"weight=0.000000\"];\n"
			"2--3 [label=\"weight=0.000000\"];\n"
			"0--3 [label=\"weight=0.000000\"];\n"
			"1--4 [label=\"weight=0.000000\"];\n"
			"3--4 [label=\"weight=0.000000\"];\n"
			"4--5 [label=\"weight=0.000000\"];\n"
			"5--6 [label=\"weight=0.000000\"];\n"
			"4--7 [label=\"weight=0.000000\"];\n"
			"6--8 [label=\"weight=0.000000\"];\n"
			"7--8 [label=\"weight=0.000000\"];\n"
			"3--8 [label=\"weight=0.000000\"];\n"
			"8--9 [label=\"weight=0.000000\"];\n"
			"9--10 [label=\"weight=0.000000\"];\n"
			"10--11 [label=\"weight=0.000000\"];\n"
			"10--12 [label=\"weight=0.000000\"];\n"
			"10--12 [label=\"weight=0.000000\"];\n"
			"11--12 [label=\"weight=0.000000\"];\n"
			"8--13 [label=\"weight=0.000000\"];\n"
			"13--14 [label=\"weight=0.000000\"];\n"
			"14--15 [label=\"weight=0.000000\"];\n"
			"14--16 [label=\"weight=0.000000\"];\n"
			"14--16 [label=\"weight=0.000000\"];\n"
			"15--16 [label=\"weight=0.000000\"];\n"
			"}";
	std::istringstream iss(irstr);

	GateQIR circuit;
	circuit.read(iss);

	std::map<int, std::vector<int>> expectedQubits;
	expectedQubits.insert(std::make_pair(0, std::vector<int>{0, 1, 2}));
	expectedQubits.insert(std::make_pair(1, std::vector<int>{0}));
	expectedQubits.insert(std::make_pair(2, std::vector<int>{1}));
	expectedQubits.insert(std::make_pair(3, std::vector<int>{1, 2}));
	expectedQubits.insert(std::make_pair(4, std::vector<int>{0, 1}));
	expectedQubits.insert(std::make_pair(5, std::vector<int>{0}));
	expectedQubits.insert(std::make_pair(6, std::vector<int>{0}));
	expectedQubits.insert(std::make_pair(7, std::vector<int>{1}));
	expectedQubits.insert(std::make_pair(8, std::vector<int>{0, 1, 2}));
	expectedQubits.insert(std::make_pair(9, std::vector<int>{0}));
	expectedQubits.insert(std::make_pair(10, std::vector<int>{0, 1, 2}));
	expectedQubits.insert(std::make_pair(11, std::vector<int>{2}));
	expectedQubits.insert(std::make_pair(12, std::vector<int>{0, 1, 2}));
	expectedQubits.insert(std::make_pair(13, std::vector<int>{1}));
	expectedQubits.insert(std::make_pair(14, std::vector<int>{0, 1, 2}));
	expectedQubits.insert(std::make_pair(15, std::vector<int>{2}));
	expectedQubits.insert(std::make_pair(16, std::vector<int>{0, 1, 2}));

	BOOST_VERIFY(circuit.order() == 17);
	BOOST_VERIFY(circuit.size() == 24);

	BOOST_VERIFY(circuit.getVertexProperty<0>(0) == "InitialState");
	BOOST_VERIFY(circuit.getVertexProperty<0>(1) == "x");
	BOOST_VERIFY(circuit.getVertexProperty<0>(2) == "h");
	BOOST_VERIFY(circuit.getVertexProperty<0>(3) == "cnot");
	BOOST_VERIFY(circuit.getVertexProperty<0>(4) == "cnot");
	BOOST_VERIFY(circuit.getVertexProperty<0>(5) == "h");
	BOOST_VERIFY(circuit.getVertexProperty<0>(6) == "measure");
	BOOST_VERIFY(circuit.getVertexProperty<0>(7) == "measure");
	BOOST_VERIFY(circuit.getVertexProperty<0>(8) == "FinalState");
	BOOST_VERIFY(circuit.getVertexProperty<0>(9) == "conditional");
	BOOST_VERIFY(circuit.getVertexProperty<0>(10) == "InitialState");
	BOOST_VERIFY(circuit.getVertexProperty<0>(11) == "z");
	BOOST_VERIFY(circuit.getVertexProperty<0>(12) == "FinalState");
	BOOST_VERIFY(circuit.getVertexProperty<0>(13) == "conditional");
	BOOST_VERIFY(circuit.getVertexProperty<0>(14) == "InitialState");
	BOOST_VERIFY(circuit.getVertexProperty<0>(15) == "x");
	BOOST_VERIFY(circuit.getVertexProperty<0>(16) == "FinalState");

	for (int i = 0; i < 17; i++) {
		BOOST_VERIFY(circuit.getVertexProperty<2>(i) == i);
		BOOST_VERIFY(circuit.getVertexProperty<3>(i) == expectedQubits[i]);
	}

	BOOST_VERIFY(circuit.getVertexProperty<1>(0) == 0);
	BOOST_VERIFY(circuit.getVertexProperty<1>(1) == 1);
	BOOST_VERIFY(circuit.getVertexProperty<1>(2) == 1);
	BOOST_VERIFY(circuit.getVertexProperty<1>(3) == 2);
	BOOST_VERIFY(circuit.getVertexProperty<1>(4) == 3);
	BOOST_VERIFY(circuit.getVertexProperty<1>(5) == 4);
	BOOST_VERIFY(circuit.getVertexProperty<1>(6) == 5);
	BOOST_VERIFY(circuit.getVertexProperty<1>(7) == 5);
	BOOST_VERIFY(circuit.getVertexProperty<1>(8) == 6);
	BOOST_VERIFY(circuit.getVertexProperty<1>(9) == 0);
	BOOST_VERIFY(circuit.getVertexProperty<1>(10) == 7);
	BOOST_VERIFY(circuit.getVertexProperty<1>(11) == 8);
	BOOST_VERIFY(circuit.getVertexProperty<1>(12) == 9);
	BOOST_VERIFY(circuit.getVertexProperty<1>(13) == 0);
	BOOST_VERIFY(circuit.getVertexProperty<1>(14) == 10);
	BOOST_VERIFY(circuit.getVertexProperty<1>(15) == 11);
	BOOST_VERIFY(circuit.getVertexProperty<1>(16) == 12);

	BOOST_VERIFY(circuit.getVertexProperty<4>(0));
	BOOST_VERIFY(circuit.getVertexProperty<4>(1));
	BOOST_VERIFY(circuit.getVertexProperty<4>(2));
	BOOST_VERIFY(circuit.getVertexProperty<4>(3));
	BOOST_VERIFY(circuit.getVertexProperty<4>(4));
	BOOST_VERIFY(circuit.getVertexProperty<4>(5));
	BOOST_VERIFY(circuit.getVertexProperty<4>(6));
	BOOST_VERIFY(circuit.getVertexProperty<4>(7));
	BOOST_VERIFY(circuit.getVertexProperty<4>(8));
	BOOST_VERIFY(circuit.getVertexProperty<4>(9));
	BOOST_VERIFY(!circuit.getVertexProperty<4>(10));
	BOOST_VERIFY(!circuit.getVertexProperty<4>(11));
	BOOST_VERIFY(!circuit.getVertexProperty<4>(12));
	BOOST_VERIFY(circuit.getVertexProperty<4>(13));
	BOOST_VERIFY(!circuit.getVertexProperty<4>(14));
	BOOST_VERIFY(!circuit.getVertexProperty<4>(15));
	BOOST_VERIFY(!circuit.getVertexProperty<4>(16));
}

BOOST_AUTO_TEST_CASE(checkGenerateGraph) {
	auto qir = std::make_shared<GateQIR>();

	auto f = std::make_shared<GateFunction>("foo");
	auto h = std::make_shared<Hadamard>(1);
	auto cn1 = std::make_shared<CNOT>(1, 2);
	auto cn2 = std::make_shared<CNOT>(0, 1);
	auto h2 = std::make_shared<Hadamard>(0);
	f->addInstruction(h);
	f->addInstruction(cn1);
	f->addInstruction(cn2);
	f->addInstruction(h2);

	qir->addKernel(f);

	qir->generateGraph("foo");

	std::stringstream ss;
	qir->write(ss);

	std::string expected = "graph G {\n"
			"node [shape=box style=filled]\n"
			"0 [label=\"Gate=InitialState,Circuit Layer=0,Gate Vertex Id=0,Gate Acting Qubits=[0 1 2],Enabled=1,RuntimeParameters=[]\"];\n"
			"1 [label=\"Gate=h,Circuit Layer=1,Gate Vertex Id=1,Gate Acting Qubits=[1],Enabled=1,RuntimeParameters=[]\"];\n"
			"2 [label=\"Gate=cnot,Circuit Layer=2,Gate Vertex Id=2,Gate Acting Qubits=[1 2],Enabled=1,RuntimeParameters=[]\"];\n"
			"3 [label=\"Gate=cnot,Circuit Layer=3,Gate Vertex Id=3,Gate Acting Qubits=[0 1],Enabled=1,RuntimeParameters=[]\"];\n"
			"4 [label=\"Gate=h,Circuit Layer=4,Gate Vertex Id=4,Gate Acting Qubits=[0],Enabled=1,RuntimeParameters=[]\"];\n"
			"5 [label=\"Gate=FinalState,Circuit Layer=5,Gate Vertex Id=5,Gate Acting Qubits=[0 1 2],Enabled=1,RuntimeParameters=[]\"];\n"
			"0--1 [label=\"weight=0.000000\"];\n"
			"1--2 [label=\"weight=0.000000\"];\n"
			"0--2 [label=\"weight=0.000000\"];\n"
			"0--3 [label=\"weight=0.000000\"];\n"
			"2--3 [label=\"weight=0.000000\"];\n"
			"3--4 [label=\"weight=0.000000\"];\n"
			"4--5 [label=\"weight=0.000000\"];\n"
			"3--5 [label=\"weight=0.000000\"];\n"
			"2--5 [label=\"weight=0.000000\"];\n"
			"}";

	std::cout << ss.str() << "\n\n" << expected << "\n";
	BOOST_VERIFY(expected == ss.str());

}
