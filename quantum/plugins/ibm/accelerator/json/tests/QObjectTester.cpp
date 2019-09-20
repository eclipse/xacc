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
#include "QObject.hpp"
#include <gtest/gtest.h>

using namespace xacc::ibm;

TEST(IBMAcceleratorTester, checkFromJson) {
    auto str = R"json({
    "backend": {
        "name": "ibmq_boeblingen"
    },
    "qObject": {
        "config": {
            "meas_level": 2,
            "meas_lo_freq": [],
            "meas_lo_range": [],
            "meas_return": "avg",
            "memory": false,
            "memory_slot_size": 100,
            "memory_slots": 2,
            "n_qubits": 20,
            "parameter_binds": [],
            "qubit_lo_freq": [],
            "qubit_lo_range": [],
            "schedule_los": [],
            "shots": 1024
        },
        "experiments": [
            {
                "config": {
                    "memory_slots": 2,
                    "n_qubits": 20
                },
                "header": {
                    "clbit_labels": [
                        [
                            "c0",
                            0
                        ],
                        [
                            "c0",
                            1
                        ]
                    ],
                    "creg_sizes": [
                        [
                            "c0",
                            2
                        ]
                    ],
                    "memory_slots": 2,
                    "n_qubits": 20,
                    "name": "bell",
                    "qreg_sizes": [
                        [
                            "q",
                            20
                        ]
                    ],
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
                    ]
                },
                "instructions": [
                    {
                        "name": "u2",
                        "params": [
                            0.0,
                            3.141592653589793
                        ],
                        "qubits": [
                            0
                        ]
                    },
                    {
                        "name": "cx",
                        "qubits": [
                            0,
                            1
                        ]
                    },
                    {
                        "memory": [
                            0
                        ],
                        "name": "measure",
                        "qubits": [
                            0
                        ]
                    },
                    {
                        "memory": [
                            1
                        ],
                        "name": "measure",
                        "qubits": [
                            1
                        ]
                    }
                ]
            }
        ],
        "header": {
            "backend_name": "ibmq_boeblingen",
            "backend_version": "1.0.0"
        },
        "qobj_id": "xacc-qobj-id",
        "schema_version": "1.1.0",
        "type": "QASM"
    },
    "shots": 1024
}
)json";
  using json = nlohmann::json;
  xacc::ibm::QObjectRoot root;
  auto j = json::parse(str);
  from_json(j, root);

  std::cout << "HELLO: " << root.get_q_object().get_type() << "\n";

  json jj;

  to_json(jj, root);

  std::cout << jj.dump(4) << "\n";

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
