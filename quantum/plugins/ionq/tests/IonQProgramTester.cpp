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
#include "ionq_program.hpp"
#include <gtest/gtest.h>

using namespace xacc::ionq;

TEST(IonQProgramTester, checkFromJson) {
    auto str = R"json({
    "lang": "json",
    "target": "qpu",
    "shots": 1000,
    "body": {
        "qubits": 2,
        "circuit": [
            {
                "gate": "h",
                "target": 0
            },
            {
                "gate":"cnot",
                "control": 0,
                "target": 1
            },
            {
                "gate": "h",
                "target": 1
            }
        ]
    }
})json";
  using json = nlohmann::json;
  xacc::ionq::IonQProgram root;
  auto j = json::parse(str);
  from_json(j, root);

  std::cout << "HELLO: " << root.get_target() << "\n";

  json jj;

  to_json(jj, root);

  std::cout << jj.dump(4) << "\n";

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
