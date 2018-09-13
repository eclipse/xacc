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
#include "ReadoutErrorIRPreprocessor.hpp"
#include "GateIR.hpp"
#include <boost/math/constants/constants.hpp>
#include "XACC.hpp"
#include "GateIRProvider.hpp"

using namespace xacc;

using namespace xacc::quantum;

using Term = std::map<int, std::string>;

std::shared_ptr<IR> createXACCIR(std::unordered_map<std::string, Term> terms) {
  // Create a new GateQIR to hold the spin based terms
  auto newIr = std::make_shared<xacc::quantum::GateIR>();
  int counter = 0;
  auto pi = boost::math::constants::pi<double>();

  GateIRProvider gateRegistry;

  // Populate GateQIR now...
  for (auto &inst : terms) {

    Term spinInst = inst.second;

    // Create a GateFunction and specify that it has
    // a parameter that is the Spin Instruction coefficient
    // that will help us get it to the user for their purposes.
    auto gateFunction = std::make_shared<xacc::quantum::GateFunction>(
        "term" + std::to_string(counter));

    // Loop over all terms in the Spin Instruction
    // and create instructions to run on the Gate QPU.
    std::vector<std::shared_ptr<xacc::Instruction>> measurements;

    std::vector<std::pair<int, std::string>> terms;
    for (auto &kv : spinInst) {
      if (kv.second != "I" && !kv.second.empty()) {
        terms.push_back({kv.first, kv.second});
      }
    }

    for (int i = terms.size() - 1; i >= 0; i--) {
      auto qbit = terms[i].first;
      auto gateName = terms[i].second;
      auto meas =
          gateRegistry.createInstruction("Measure", std::vector<int>{qbit});
      xacc::InstructionParameter classicalIdx(qbit);
      meas->setParameter(0, classicalIdx);
      measurements.push_back(meas);

      if (gateName == "X") {
        auto hadamard =
            gateRegistry.createInstruction("H", std::vector<int>{qbit});
        gateFunction->addInstruction(hadamard);
      } else if (gateName == "Y") {
        auto rx = gateRegistry.createInstruction("Rx", std::vector<int>{qbit});
        InstructionParameter p(pi / 2.0);
        rx->setParameter(0, p);
        gateFunction->addInstruction(rx);
      }
    }

    for (auto m : measurements) {
      gateFunction->addInstruction(m);
    }

    newIr->addKernel(gateFunction);
    counter++;
  }

  return newIr;
}

class FakeAB : public AcceleratorBuffer {
public:
  FakeAB(const std::string &str, const int N) : AcceleratorBuffer(str, N) {}
  virtual const double getExpectationValueZ() { return 1.0; }
};

TEST(ReadoutErrorIRPreprocessorTester, checkSimple) {

  xacc::Initialize();
  //	(-2.143303525+0j)*X0*X1 + (-3.91311896+0j)*X1*X2 +
  //			(-2.143303525+0j)*Y0*Y1 + (-3.91311896+0j)*Y1*Y2 +
  //(0.218290555+0j)*Z0 + (-6.125+0j)*Z1 + (-9.625+0j)*Z2 	needs x0, x1,
  // x2, y0, y1, y2

  std::unordered_map<std::string, Term> test{{"X0X1", {{0, "X"}, {1, "X"}}},
                                             {"X1X2", {{1, "X"}, {2, "X"}}},
                                             {"Y0Y1", {{0, "Y"}, {1, "Y"}}},
                                             {"Y1Y2", {{1, "Y"}, {2, "Y"}}},
                                             {"Z0", {{0, "Z"}}},
                                             {"Z1", {{1, "Z"}}},
                                             {"Z2", {{2, "Z"}}}};

  auto ir = createXACCIR(test);

  ReadoutErrorIRPreprocessor preprocessor;
  auto bufferProcessor = preprocessor.process(*ir);

  int nMeasurementKernels = 6;
  int nExtraKernels = 6;

  EXPECT_TRUE(ir->getKernels().size() ==
              test.size() + nMeasurementKernels + nExtraKernels);

  std::vector<std::shared_ptr<AcceleratorBuffer>> buffers;
  for (int i = 0; i < ir->getKernels().size(); i++) {
    buffers.push_back(std::make_shared<FakeAB>("b" + std::to_string(i), 3));
  }

  auto fixedBuffers = bufferProcessor->process(buffers);

  EXPECT_TRUE(test.size() + nExtraKernels == fixedBuffers.size());

  for (int i = 0; i < test.size(); i++) {
    EXPECT_TRUE(1.0 == fixedBuffers[i]->getExpectationValueZ());
  }

  xacc::Finalize();
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
