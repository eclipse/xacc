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
#include <stdexcept>
#include <fstream>

#include "Instruction.hpp"
#include "xacc.hpp"
#include "xacc_config.hpp"
#include "xacc_service.hpp"
#include "Utils.hpp"
#include "Scheduler.hpp"

TEST(PulseSchedulerTester, checkBasic) {
  // TODO load from json file prototypical pulses and cmd_defs...

  std::string tests = std::string(GATEIR_TEST_FILE_DIR);

  std::ifstream backendFile(tests + "/test_backends.json");
  std::string jjson((std::istreambuf_iterator<char>(backendFile)),
                    std::istreambuf_iterator<char>());

  if (xacc::hasAccelerator("ibm")) {
    auto ibm = xacc::getService<xacc::Accelerator>("ibm");
    ibm->updateConfiguration(
        xacc::HeterogeneousMap{std::make_pair("backend", "ibmq_poughkeepsie")});
    ibm->contributeInstructions(jjson);

    auto provider = xacc::getIRProvider("quantum");
    auto program = provider->createComposite("foo");

    auto htmp = xacc::ir::asComposite(xacc::getContributedService<xacc::Instruction>("pulse::u2_0"));

    auto h = (*htmp)({0.0, xacc::constants::pi});
    auto cx = xacc::getContributedService<xacc::Instruction>("pulse::cx_0_1");

    program->addInstructions({h,cx});

    std::cout << program->toString() << "\n";

    auto scheduler = xacc::getService<xacc::Scheduler>("default");

    scheduler->schedule(program);

    std::cout << "AFTER\n" << program->toString() << "\n";
  }
}
int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
