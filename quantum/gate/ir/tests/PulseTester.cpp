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
        xacc::HeterogeneousMap{std::make_pair("backend", "ibmq_poughkeepsie")});
    ibm->contributeInstructions(jjson);

    auto cr = xacc::getContributedService<xacc::Instruction>("CR90m_u10_3e8d");
    std::vector<std::vector<double>> expectedSamples = {
        {-0.009831064380705357, 0.00148052501026541},
        {-0.023543914780020714, 0.003545633750036359},
        {-0.04115137457847595, 0.006197257433086634},
        {-0.061806514859199524, 0.009307852014899254},
        {-0.0836654007434845, 0.01259972620755434},
        {-0.104045070707798, 0.015668835490942},
        {-0.11991630494594574, 0.018058989197015762},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.12975533306598663, 0.019540714100003242},
        {-0.11991630494594574, 0.018058989197015762},
        {-0.104045070707798, 0.015668835490942},
        {-0.0836654007434845, 0.01259972620755434},
        {-0.061806514859199524, 0.009307852014899254},
        {-0.04115137457847595, 0.006197257433086634},
        {-0.023543914780020714, 0.003545633750036359},
        {-0.009831064380705357, 0.00148052501026541}};
    EXPECT_EQ(expectedSamples, cr->getSamples());
    EXPECT_EQ(expectedSamples.size(), cr->duration());
    EXPECT_EQ("CR90m_u10_3e8d", cr->name());

    auto cx_0_5 =
        xacc::getContributedService<xacc::Instruction>("pulse::cx_0_5");
    auto cx_0_5_comp =
        std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx_0_5);
    EXPECT_TRUE(cx_0_5_comp);

    auto sub_cr = cx_0_5_comp->getInstruction(13);
    EXPECT_EQ("u10", sub_cr->channel());
    EXPECT_EQ("CR90m_u10_3e8d", sub_cr->name());
    EXPECT_EQ(90, sub_cr->start());
    EXPECT_EQ(122 - 90-1, sub_cr->getSamples().size());

    auto u3_0 =
        xacc::getContributedService<xacc::Instruction>("pulse::u3_0");
    auto u3_0_comp =
        std::dynamic_pointer_cast<xacc::CompositeInstruction>(u3_0);

    std::cout << "HOWDY: " << u3_0_comp->nVariables() << "\n";
    for (auto& v : u3_0_comp->getVariables()) {
        std::cout << v<< "\n";
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
