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
#include "PulseQObject.hpp"
#include <gtest/gtest.h>
#include "xacc_config.hpp"
#include <fstream>
using namespace xacc::ibm_pulse;

TEST(PulseQObjectTester, checkFromJson) {
   std::string tests = std::string(IBM_TEST_FILE_DIR);

  std::ifstream backendFile(tests + "/pulse.json");
  std::string jjson((std::istreambuf_iterator<char>(backendFile)),
                    std::istreambuf_iterator<char>());
  using json = nlohmann::json;
  xacc::ibm_pulse::PulseQObject root;
  auto j = json::parse(jjson);
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
