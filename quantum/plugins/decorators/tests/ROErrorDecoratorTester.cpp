/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
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
#include "xacc_config.hpp"
#include "PauliOperator.hpp"
#include <fstream>

#include "xacc_service.hpp"
#include "AcceleratorDecorator.hpp"

using namespace xacc;

using namespace xacc::quantum;

TEST(ROErrorDecoratorTester, checkRaphaelBug) {
  std::string testFilesDir = std::string(ROERROR_TEST_FILE_DIR);

  xacc::Initialize();

  xacc::setOption("ro-error-file", testFilesDir + "/ibm_roerrors.json");

  auto roerror = xacc::getService<AcceleratorDecorator>("ro-error");
  std::ifstream testab(testFilesDir + "/nah_ibm.ab");
  auto buffer = std::make_shared<AcceleratorBuffer>();
  std::cout << testFilesDir << "/nah_ibm.ab"
            << "\n";

  buffer->load(testab);
  auto hamString =
      mpark::get<std::string>(buffer->getInformation("hamiltonian"));
  PauliOperator op(hamString);

  auto ir = op.toXACCIR();
  auto functions = ir->getComposites();

  roerror->execute(buffer, functions);

  buffer->getChildren()[2]->print(std::cout);

  xacc::Finalize();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
