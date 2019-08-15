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
#include <gtest/gtest.h>
#include "IBMIRTransformation.hpp"
#include "X.hpp"
#include "GateIR.hpp"

using namespace xacc;
using namespace xacc::quantum;

TEST(IBMIRTransformationTester,checkCreation) {

	std::vector<std::pair<int,int>> couplers {{0,1},{0,2},{1,2},{2,3},{2,4},{3,4}};
    auto t = std::make_shared<IBMIRTransformation>(couplers);
    
	auto f = std::make_shared<GateFunction>("foo");
	auto x = std::make_shared<X>(0);
	auto h = std::make_shared<Hadamard>(1);
	auto cn0 = std::make_shared<CNOT>(1, 2);
	auto cn1 = std::make_shared<CNOT>(2, 3);
	auto h2 = std::make_shared<Hadamard>(0);
	auto m0 = std::make_shared<Measure>(0, 0);
	auto m1 = std::make_shared<Measure>(1,1);
	auto m2 = std::make_shared<Measure>(2,2);

	auto cn2 = std::make_shared<CNOT>(2, 0);

	f->addInstruction(x);
	f->addInstruction(h);
	f->addInstruction(cn0);
	f->addInstruction(cn1);
	f->addInstruction(h2);
	f->addInstruction(cn2);

	f->addInstruction(m0);
	f->addInstruction(m1);
	f->addInstruction(m2);

	auto ir = std::make_shared<GateIR>();
	ir->addKernel(f);

	auto newir = t->transform(ir);

	std::stringstream ss;
	newir->persist(ss);

	const std::string expected = R"expected({
    "kernels": [
        {
            "function": "foo",
            "instructions": [
                {
                    "gate": "X",
                    "enabled": true,
                    "qubits": [
                        0
                    ]
                },
                {
                    "gate": "H",
                    "enabled": true,
                    "qubits": [
                        1
                    ]
                },
                {
                    "gate": "CNOT",
                    "enabled": true,
                    "qubits": [
                        1,
                        2
                    ]
                },
                {
                    "gate": "CNOT",
                    "enabled": true,
                    "qubits": [
                        2,
                        3
                    ]
                },
                {
                    "gate": "H",
                    "enabled": true,
                    "qubits": [
                        0
                    ]
                },
                {
                    "gate": "H",
                    "enabled": true,
                    "qubits": [
                        2
                    ]
                },
                {
                    "gate": "H",
                    "enabled": true,
                    "qubits": [
                        0
                    ]
                },
                {
                    "gate": "CNOT",
                    "enabled": true,
                    "qubits": [
                        0,
                        2
                    ]
                },
                {
                    "gate": "H",
                    "enabled": true,
                    "qubits": [
                        2
                    ]
                },
                {
                    "gate": "H",
                    "enabled": true,
                    "qubits": [
                        0
                    ]
                },
                {
                    "gate": "Measure",
                    "enabled": true,
                    "qubits": [
                        0
                    ],
                    "classicalBitIdx": 0
                },
                {
                    "gate": "Measure",
                    "enabled": true,
                    "qubits": [
                        1
                    ],
                    "classicalBitIdx": 1
                },
                {
                    "gate": "Measure",
                    "enabled": true,
                    "qubits": [
                        2
                    ],
                    "classicalBitIdx": 2
                }
            ]
        }
    ]
})expected";

	EXPECT_TRUE(expected == ss.str());
}

int main(int argc, char** argv) {
   xacc::Initialize();
   ::testing::InitGoogleTest(&argc, argv);
   auto ret = RUN_ALL_TESTS();
   xacc::Finalize();
   return ret;
}
