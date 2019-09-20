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
#include "Properties.hpp"
#include <gtest/gtest.h>
#include "xacc_config.hpp"
#include <fstream>

using namespace xacc::ibm_properties;

TEST(IBMAcceleratorBackendsTester, checkFromJson) {

  std::string ibmTests = std::string(IBM_TEST_FILE_DIR);
  std::ifstream propFile(ibmTests + std::string("/valencia_props.json"));
      std::string propsJson((std::istreambuf_iterator<char>(propFile)),
                              std::istreambuf_iterator<char>());

//   std::cout << propsJson << "\n";
  using json = nlohmann::json;
  xacc::ibm_properties::Properties root;
  auto j = json::parse(propsJson);
  from_json(j, root);

  std::cout << std::boolalpha << (root.get_qubits()[0][4].get_name() == xacc::ibm_properties::Name::PROB_MEAS0_PREP1) << ", " << root.get_qubits()[0][4].get_value() << "\n";
//   EXPECT_EQ(root.get_backends()[0].get_name(), "ibmq_valencia");
//   EXPECT_EQ(root.get_backends()[0].get_specific_configuration().get_max_shots(), 8192);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
