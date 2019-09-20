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
#include "Backends.hpp"
#include <gtest/gtest.h>
#include "xacc_config.hpp"
#include <fstream>

using namespace xacc::ibm_backend;

TEST(IBMAcceleratorBackendsTester, checkFromJson) {

  std::string ibmTests = std::string(IBM_TEST_FILE_DIR);
  std::ifstream backendFile(ibmTests + std::string("/valencia.json"));
      std::string backendJson((std::istreambuf_iterator<char>(backendFile)),
                              std::istreambuf_iterator<char>());

  using json = nlohmann::json;
  xacc::ibm_backend::Backends root;
  auto j = json::parse(backendJson);
  from_json(j, root);

  EXPECT_EQ(root.get_backends()[0].get_name(), "ibmq_valencia");
  EXPECT_EQ(root.get_backends()[0].get_specific_configuration().get_max_shots(), 8192);
  EXPECT_EQ(root.get_backends().size(),2);
  std::cout << root.get_backends().size() << "\n";
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
