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


namespace {
  // Helper functions to collect data from an *unscheduled* composite instruction (i.e. command def)
  // in order to validate the scheduler.
  std::vector<std::pair<std::string, size_t>> getComponentPulseAndStartTime(const std::shared_ptr<xacc::CompositeInstruction>& compositeInst) {
    std::vector<std::pair<std::string, size_t>> result;
    for (auto& inst: compositeInst->getInstructions()) {
      if (inst->isEnabled() && !inst->isComposite()) {
        result.emplace_back(inst->name(), inst->start());
      }
    }

    return result;
  }

  size_t calcCompositeInstructionDuration(const std::shared_ptr<xacc::CompositeInstruction>& compositeInst) {
    size_t result = 0;
    for (auto& inst: compositeInst->getInstructions()) {
      if (inst->isEnabled() && !inst->isComposite()) {
        if (inst->start() + inst->duration() > result) {
          // Just take the max of end times of all component pulses
          result = inst->start() + inst->duration();
        }
      }
    }

    return result;
  }
}

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

    auto scheduler = xacc::getService<xacc::Scheduler>("pulse");

    scheduler->schedule(program);

    std::cout << "AFTER\n" << program->toString() << "\n";
  }
}

TEST(PulseSchedulerTester, checkCompositeOfPulses) {
  // Pulse instructions should already be loaded previously
  auto provider = xacc::getIRProvider("quantum");
  auto program = provider->createComposite("test1");
  
  auto pulseInst1 = xacc::getContributedService<xacc::Instruction>("CR90m_u10_7417");
  pulseInst1->setChannel("d0");

  auto pulseInst2 = xacc::getContributedService<xacc::Instruction>("CR90m_u12_4a25");
  pulseInst2->setChannel("u0");

  auto pulseInst3 = xacc::getContributedService<xacc::Instruction>("CR90m_u12_4a25");
  pulseInst3->setChannel("d1");

  auto pulseInst4 = xacc::getContributedService<xacc::Instruction>("CR90m_d0_3cff");
  pulseInst4->setChannel("u1");

  auto pulseInst5 = xacc::getContributedService<xacc::Instruction>("CR90m_d6_874d");
  pulseInst5->setChannel("d0");

  auto pulseInst6 = xacc::getContributedService<xacc::Instruction>("CR90m_u24_5bf6");
  pulseInst6->setChannel("d1");
  
  program->addInstruction(pulseInst1);
  program->addInstruction(pulseInst2);
  program->addInstruction(pulseInst3);
  program->addInstruction(pulseInst4);
  program->addInstruction(pulseInst5);
  program->addInstruction(pulseInst6);

  auto scheduler = xacc::getService<xacc::Scheduler>("pulse");
  scheduler->schedule(program);
  
  // Pulse 5 is on the same channel (d0) as pulse 1 
  EXPECT_GE(pulseInst5->start(), pulseInst1->start() + pulseInst1->duration());
  
  // Pulse 6 is on the same channel (d1) as pulse 3 
  EXPECT_GE(pulseInst6->start(), pulseInst3->start() + pulseInst3->duration()); 
}

TEST(PulseSchedulerTester, checkCompositeOfComposites) {
  auto provider = xacc::getIRProvider("quantum");
  auto program = provider->createComposite("test2");

  auto htmp1 = xacc::ir::asComposite(xacc::getContributedService<xacc::Instruction>("pulse::u2_5"));

  auto h1 = (*htmp1)({0.0, xacc::constants::pi});
  auto x1 = xacc::getContributedService<xacc::Instruction>("pulse::x_4");
  auto cx1 = xacc::getContributedService<xacc::Instruction>("pulse::cx_1_0");
  auto cx2 = xacc::getContributedService<xacc::Instruction>("pulse::cx_1_2");

  program->addInstructions({x1, h1, cx1, cx2});
  
  // Cache data *before* scheduling for validation
  const auto h1PulseSchedule = getComponentPulseAndStartTime(std::dynamic_pointer_cast<xacc::CompositeInstruction>(h1));
  const auto h1Duration = calcCompositeInstructionDuration(std::dynamic_pointer_cast<xacc::CompositeInstruction>(h1));
  const auto x1PulseSchedule = getComponentPulseAndStartTime(std::dynamic_pointer_cast<xacc::CompositeInstruction>(x1));
  const auto x1Duration = calcCompositeInstructionDuration(std::dynamic_pointer_cast<xacc::CompositeInstruction>(x1));
  const auto cx1PulseSchedule = getComponentPulseAndStartTime(std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx1));
  const auto cx1Duration = calcCompositeInstructionDuration(std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx1));
  const auto cx2PulseSchedule = getComponentPulseAndStartTime(std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx2));
  const auto cx2Duration = calcCompositeInstructionDuration(std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx2));
    
  // Note: these composite pulses are from IBM, hence their component pulses have all been scheduled
  // appropriately (reference time 0 of the composite), we need to validate that after our scheduling,
  // the pulse timing within each composite is reserved, just shifted by the global start time of that composite.
  const auto validateCompositeAfterScheduled = [](const std::shared_ptr<xacc::CompositeInstruction>& compositeInstAfterScheduled, size_t compositeStartTime, const std::vector<std::pair<std::string, size_t>>& pulseScheduleBefore){
    EXPECT_EQ(compositeInstAfterScheduled->nInstructions(), pulseScheduleBefore.size());
    for(size_t i = 0; i < pulseScheduleBefore.size(); ++i) {
      EXPECT_EQ(compositeInstAfterScheduled->getInstruction(i)->name(), pulseScheduleBefore[i].first);
      // The pulse start within the composite is shifted by the start time of the parent (composite instruction)
      EXPECT_EQ(compositeInstAfterScheduled->getInstruction(i)->start(), pulseScheduleBefore[i].second + compositeStartTime);
    }
  };
  
  // std::cout << "BEFORE: " << program->toString() << "\n";

  auto scheduler = xacc::getService<xacc::Scheduler>("pulse");
  scheduler->schedule(program);

  // std::cout << "AFTER: " << program->toString() << "\n";

  // The program is {x1, h1, cx1, cx2}
  validateCompositeAfterScheduled(std::dynamic_pointer_cast<xacc::CompositeInstruction>(x1), 0, x1PulseSchedule);
  validateCompositeAfterScheduled(std::dynamic_pointer_cast<xacc::CompositeInstruction>(h1), x1Duration, h1PulseSchedule);
  validateCompositeAfterScheduled(std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx1), x1Duration + h1Duration, cx1PulseSchedule);
  validateCompositeAfterScheduled(std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx2), x1Duration + h1Duration + cx1Duration, cx2PulseSchedule);
}

TEST(PulseSchedulerTester, checkComplexConstruct) {
  // Same as before, but we mix and match between raw pulses and command defs 
  auto provider = xacc::getIRProvider("quantum");
  auto program = provider->createComposite("test3");

  auto htmp1 = xacc::ir::asComposite(xacc::getContributedService<xacc::Instruction>("pulse::u2_10"));

  auto h1 = (*htmp1)({0.0, xacc::constants::pi});
  auto x1 = xacc::getContributedService<xacc::Instruction>("pulse::x_10");
  auto cx1 = xacc::getContributedService<xacc::Instruction>("pulse::cx_10_5");
  auto cx2 = xacc::getContributedService<xacc::Instruction>("pulse::cx_10_11");

  auto pulseInst1 = xacc::getContributedService<xacc::Instruction>("CR90m_u6_c549");
  pulseInst1->setChannel("d10");

  auto pulseInst2 = xacc::getContributedService<xacc::Instruction>("CR90m_u2_5e5e");
  pulseInst2->setChannel("d10");

  // The program consists of command defs and pulses
  program->addInstructions({x1, h1, pulseInst1, cx1, pulseInst2, cx2});
  
  // Cache data *before* scheduling for validation
  const auto h1PulseSchedule = getComponentPulseAndStartTime(std::dynamic_pointer_cast<xacc::CompositeInstruction>(h1));
  const auto h1Duration = calcCompositeInstructionDuration(std::dynamic_pointer_cast<xacc::CompositeInstruction>(h1));
  const auto x1PulseSchedule = getComponentPulseAndStartTime(std::dynamic_pointer_cast<xacc::CompositeInstruction>(x1));
  const auto x1Duration = calcCompositeInstructionDuration(std::dynamic_pointer_cast<xacc::CompositeInstruction>(x1));
  const auto cx1PulseSchedule = getComponentPulseAndStartTime(std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx1));
  const auto cx1Duration = calcCompositeInstructionDuration(std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx1));
  const auto cx2PulseSchedule = getComponentPulseAndStartTime(std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx2));
  const auto cx2Duration = calcCompositeInstructionDuration(std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx2));
    
  // Note: these composite pulses are from IBM, hence their component pulses have all been scheduled
  // appropriately (reference time 0 of the composite), we need to validate that after our scheduling,
  // the pulse timing within each composite is reserved, just shifted by the global start time of that composite.
  const auto validateCompositeAfterScheduled = [](const std::shared_ptr<xacc::CompositeInstruction>& compositeInstAfterScheduled, size_t compositeStartTime, const std::vector<std::pair<std::string, size_t>>& pulseScheduleBefore){
    EXPECT_EQ(compositeInstAfterScheduled->nInstructions(), pulseScheduleBefore.size());
    for(size_t i = 0; i < pulseScheduleBefore.size(); ++i) {
      EXPECT_EQ(compositeInstAfterScheduled->getInstruction(i)->name(), pulseScheduleBefore[i].first);
      // The pulse start within the composite is shifted by the start time of the parent (composite instruction)
      EXPECT_EQ(compositeInstAfterScheduled->getInstruction(i)->start(), pulseScheduleBefore[i].second + compositeStartTime);
    }
  };

  // std::cout << "BEFORE: " << program->toString() << "\n";
  
  auto scheduler = xacc::getService<xacc::Scheduler>("pulse");
  scheduler->schedule(program);

  // std::cout << "AFTER: " << program->toString() << "\n";

  // The program is {x1, h1, pulseInst1, cx1, pulseInst2, cx2}
  validateCompositeAfterScheduled(std::dynamic_pointer_cast<xacc::CompositeInstruction>(x1), 0, x1PulseSchedule);
  validateCompositeAfterScheduled(std::dynamic_pointer_cast<xacc::CompositeInstruction>(h1), x1Duration, h1PulseSchedule);
  
  // Pulse1 is shifted to after x1 and h1 composites
  EXPECT_EQ(pulseInst1->start(), x1Duration + h1Duration);
  
  validateCompositeAfterScheduled(std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx1), x1Duration + h1Duration + pulseInst1->duration(), cx1PulseSchedule);
  
  // Pulse2 is also shifted appropriately
  EXPECT_EQ(pulseInst2->start(), x1Duration + h1Duration + pulseInst1->duration() + cx1Duration);
  
  validateCompositeAfterScheduled(std::dynamic_pointer_cast<xacc::CompositeInstruction>(cx2), x1Duration + h1Duration + pulseInst1->duration() + cx1Duration + pulseInst2->duration(), cx2PulseSchedule);
}

int main(int argc, char **argv) {
  xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
