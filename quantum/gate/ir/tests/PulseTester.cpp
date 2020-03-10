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

TEST(PulseTester, checkBasic) {
  // TODO load from json file prototypical pulses and cmd_defs...

  std::string tests = std::string(GATEIR_TEST_FILE_DIR);

  std::ifstream backendFile(tests + "/test_backends.json");
  std::string jjson((std::istreambuf_iterator<char>(backendFile)),
                    std::istreambuf_iterator<char>());

  if (xacc::hasAccelerator("ibm")) {
    auto ibm = xacc::getService<xacc::Accelerator>("ibm");
    ibm->updateConfiguration(
        xacc::HeterogeneousMap{std::make_pair("backend", "ibmq_johannesburg")});
    ibm->contributeInstructions(jjson);

    auto cr = xacc::getContributedService<xacc::Instruction>("CR90m_u2_5e5e");
    std::vector<std::vector<double>> expectedSamples = {
        {0.03556624427437782, 0.0037936249282211065},
        {0.0851757824420929, 0.009085158817470074},
        {0.14887501299381256, 0.015879549086093903},
        {0.22359997034072876, 0.023849984630942345},
        {0.30267977714538574, 0.032284922897815704},
        {0.37640810012817383, 0.040149055421352386},
        {0.43382617831230164, 0.046273473650217056},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.4694211781024933, 0.05007016658782959},
        {0.43382617831230164, 0.046273473650217056},
        {0.37640810012817383, 0.040149055421352386},
        {0.30267977714538574, 0.032284922897815704},
        {0.22359997034072876, 0.023849984630942345},
        {0.14887501299381256, 0.015879549086093903},
        {0.0851757824420929, 0.009085158817470074},
        {0.03556624427437782, 0.0037936249282211065}};
    EXPECT_EQ(expectedSamples, cr->getSamples());
    EXPECT_EQ(expectedSamples.size(), cr->duration());
    EXPECT_EQ("CR90m_u2_5e5e", cr->name());

    auto cx_0_1 =
        xacc::getContributedService<xacc::Instruction>("pulse::cx_0_1");
    auto cx_0_1_comp =
        std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx_0_1);
    EXPECT_TRUE(cx_0_1_comp);
    
    auto sub_cr = cx_0_1_comp->getInstruction(12);
    EXPECT_EQ("u2", sub_cr->channel());
    EXPECT_EQ("CR90m_u2_5e5e", sub_cr->name());
    EXPECT_EQ(48, sub_cr->start());

    auto u3_0 = xacc::getContributedService<xacc::Instruction>("pulse::u3_0");
    auto u3_0_comp =
        std::dynamic_pointer_cast<xacc::CompositeInstruction>(u3_0);

    std::cout << "HOWDY: " << u3_0_comp->nVariables() << "\n";
    for (auto &v : u3_0_comp->getVariables()) {
      std::cout << v << "\n";
    }
  }
}
int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
