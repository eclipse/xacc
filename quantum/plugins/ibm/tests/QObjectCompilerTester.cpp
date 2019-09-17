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
#include "xacc.hpp"
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

    EXPECT_TRUE(ir->getComposites().size() == 1);

    xacc::Finalize();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
