/***********************************************************************************
 * Copyright (c) 2018, UT-Battelle
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
 *   Initial implementation - H. Charles Zhao
 *
 **********************************************************************************/
#include <gtest/gtest.h>
#include "XACC.hpp"
#include "xacc_service.hpp"

using namespace xacc;


TEST(QObjectCompilerTester, checkCompileOQASM) {
    xacc::Initialize();

    const std::string src = R"src({
    "qObject": {
        "qobj_id": "0f8df015-6b7c-44a9-833f-f802e1405f54",
        "experiments": [
            {
                "config": {
                    "n_qubits": 20,
                    "memory_slots": 3
                },
                "header": {
                    "name": "circuit0",
                    "qreg_sizes": [
                        [
                            "q",
                            20
                        ]
                    ],
                    "n_qubits": 20,
                    "qubit_labels": [
                        [
                            "q",
                            0
                        ],
                        [
                            "q",
                            1
                        ],
                        [
                            "q",
                            2
                        ],
                        [
                            "q",
                            3
                        ],
                        [
                            "q",
                            4
                        ],
                        [
                            "q",
                            5
                        ],
                        [
                            "q",
                            6
                        ],
                        [
                            "q",
                            7
                        ],
                        [
                            "q",
                            8
                        ],
                        [
                            "q",
                            9
                        ],
                        [
                            "q",
                            10
                        ],
                        [
                            "q",
                            11
                        ],
                        [
                            "q",
                            12
                        ],
                        [
                            "q",
                            13
                        ],
                        [
                            "q",
                            14
                        ],
                        [
                            "q",
                            15
                        ],
                        [
                            "q",
                            16
                        ],
                        [
                            "q",
                            17
                        ],
                        [
                            "q",
                            18
                        ],
                        [
                            "q",
                            19
                        ]
                    ],
                    "memory_slots": 3,
                    "creg_sizes": [
                        [
                            "c0",
                            3
                        ]
                    ],
                    "clbit_labels": [
                        [
                            "c0",
                            0
                        ],
                        [
                            "c0",
                            1
                        ],
                        [
                            "c0",
                            2
                        ]
                    ]
                },
                "instructions": [
                    {
                        "qubits": [
                            1
                        ],
                        "name": "u3",
                        "params": [
                            3.141592653589793,
                            0,
                            3.141592653589793
                        ]
                    },
                    {
                        "qubits": [
                            2
                        ],
                        "name": "u2",
                        "params": [
                            0,
                            3.141592653589793
                        ]
                    },
                    {
                        "qubits": [
                            1,
                            2
                        ],
                        "name": "cx"
                    },
                    {
                        "qubits": [
                            2
                        ],
                        "name": "u1",
                        "params": [
                            -0.7853981633974483
                        ]
                    },
                    {
                        "qubits": [
                            6
                        ],
                        "name": "u3",
                        "params": [
                            3.141592653589793,
                            0,
                            3.141592653589793
                        ]
                    },
                    {
                        "qubits": [
                            6,
                            2
                        ],
                        "name": "cx"
                    },
                    {
                        "qubits": [
                            2
                        ],
                        "name": "u1",
                        "params": [
                            0.7853981633974483
                        ]
                    },
                    {
                        "qubits": [
                            1,
                            2
                        ],
                        "name": "cx"
                    },
                    {
                        "qubits": [
                            1
                        ],
                        "name": "u1",
                        "params": [
                            0.7853981633974483
                        ]
                    },
                    {
                        "qubits": [
                            2
                        ],
                        "name": "u1",
                        "params": [
                            -0.7853981633974483
                        ]
                    },
                    {
                        "qubits": [
                            6,
                            2
                        ],
                        "name": "cx"
                    },
                    {
                        "qubits": [
                            2
                        ],
                        "name": "u2",
                        "params": [
                            0,
                            3.9269908169872414
                        ]
                    },
                    {
                        "qubits": [
                            6,
                            1
                        ],
                        "name": "cx"
                    },
                    {
                        "qubits": [
                            1
                        ],
                        "name": "u1",
                        "params": [
                            -0.7853981633974483
                        ]
                    },
                    {
                        "qubits": [
                            6
                        ],
                        "name": "u1",
                        "params": [
                            0.7853981633974483
                        ]
                    },
                    {
                        "memory": [
                            0
                        ],
                        "qubits": [
                            6
                        ],
                        "name": "measure"
                    },
                    {
                        "memory": [
                            1
                        ],
                        "qubits": [
                            1
                        ],
                        "name": "measure"
                    },
                    {
                        "memory": [
                            2
                        ],
                        "qubits": [
                            2
                        ],
                        "name": "measure"
                    }
                ]
            }
        ],
        "config": {
            "shots": 1024,
            "n_qubits": 20,
            "memory_slots": 3,
            "memory": false,
            "parameter_binds": [],
            "meas_lo_freq": [],
            "schedule_los": [],
            "qubit_lo_freq": [],
            "qubit_lo_range": [],
            "meas_lo_range": [],
            "meas_return": "avg",
            "meas_level": 2,
            "memory_slot_size": 100
        },
        "type": "QASM",
        "schema_version": "1.1.0",
        "header": {}
    },
    "backend": {
        "name": "ibmq_20_tokyo"
    }
})src";

    auto compiler = xacc::getService<Compiler>("qobj");
    auto ir = compiler->compile(src);

    EXPECT_TRUE(ir->getKernels().size() == 1);

    xacc::Finalize();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
