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
#define BOOST_TEST_MODULE FireTensorAcceleratorTester

#include <memory>
#include <boost/test/included/unit_test.hpp>
#include "FireTensorAccelerator.hpp"
#include "GraphIR.hpp"

using namespace xacc::quantum;

BOOST_AUTO_TEST_CASE(checkConstruction) {

	FireTensorAccelerator<3> acc;
	auto qreg = acc.createBuffer("qreg", 3);

	// Create a graph IR modeling a
	// quantum teleportation kernel
	std::string irstr = "graph G {\n"
			"{\n"
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
			"}\n"
			"0--1 ;\n"
			"0--2 ;\n"
			"2--3 ;\n"
			"0--3 ;\n"
			"1--4 ;\n"
			"3--4 ;\n"
			"4--5 ;\n"
			"5--6 ;\n"
			"4--7 ;\n"
			"6--8 ;\n"
			"7--8 ;\n"
			"3--8 ;\n"
			"8--9 ;\n"
			"9--10 ;\n"
			"10--11 ;\n"
			"10--12 ;\n"
			"10--12 ;\n"
			"11--12 ;\n"
			"8--13 ;\n"
			"13--14 ;\n"
			"14--15 ;\n"
			"14--16 ;\n"
			"14--16 ;\n"
			"15--16 ;\n"
			"}";
	std::istringstream iss(irstr);

	auto graphir = std::make_shared<xacc::GraphIR<QuantumCircuit>>();
	graphir->read(iss);
	acc.execute("qreg", graphir);

	BOOST_VERIFY(qreg->getState()(1) * qreg->getState()(1) == 1 ||
			qreg->getState()(5) * qreg->getState()(5) == 1 ||
			qreg->getState()(3) * qreg->getState()(3) == 1 ||
			qreg->getState()(7) * qreg->getState()(7) == 1);
}

