/*******************************************************************************
 * Copyright (c) 2019 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
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
