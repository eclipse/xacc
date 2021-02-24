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

#include "AcceleratorBuffer.hpp"
#include <cmath>
using namespace xacc;

TEST(AcceleratorBufferTester, checkGetExpectationValueZ) {

  AcceleratorBuffer buffer("qreg", 5);

  std::string measurement1 = "00000";
  std::string measurement2 = "00001";

  buffer.appendMeasurement(measurement1, 500);
  buffer.appendMeasurement(measurement2, 504);

  EXPECT_TRUE(std::fabs(buffer.getExpectationValueZ() + 0.00398406) < 1e-6);
  EXPECT_TRUE(std::fabs(buffer.computeMeasurementProbability("00000") -
                        0.4980079681) < 1e-6);
  EXPECT_TRUE(std::fabs(buffer.computeMeasurementProbability("00001") -
                        0.5019920319) < 1e-6);

  std::string m1 = "000000000000";
  std::string m2 = "000000000000";
  std::string m3 = "000001000000";

  AcceleratorBuffer bigBuffer("qreg", 12);

  bigBuffer.appendMeasurement(m1, 1);
  bigBuffer.appendMeasurement(m2, 7827);
  bigBuffer.appendMeasurement(m3, 364);

  std::cout << "HELLO: " << bigBuffer.getExpectationValueZ() << "\n";
  EXPECT_TRUE(std::fabs(bigBuffer.getExpectationValueZ() - 0.911122) < 1e-6);

  AcceleratorBuffer b("qreg", 7);

  b.appendMeasurement("0000000", 3513);
  b.appendMeasurement("0000001", 904);
  b.appendMeasurement("0000010", 2459);
  b.appendMeasurement("0000011", 1316);

  EXPECT_TRUE(std::fabs(b.getExpectationValueZ() - 0.178955078125) < 1e-6);
}

TEST(AcceleratorBufferTester, checkLoad) {
  const std::string bufferStr = R"bufferStr({
    "AcceleratorBuffer": {
        "name": "q",
        "size": 2,
        "Information": {
            "vqe-angles": [
                0.5945312500000002
            ],
            "vqe-energy": -1.7491552234943562,
            "vqe-nQPU-calls": 0
        },
        "Measurements": {},
        "Children": [
            {
                "name": "Z1",
                "Information": {
                    "kernel": "Z1",
                    "parameters": [
                        0.5
                    ]
                },
                "Measurements": {
                    "00": 323,
                    "01": 701
                }
            },
            {
                "name": "I",
                "Information": {
                    "kernel": "I",
                    "parameters": [
                        0.5
                    ]
                },
                "Measurements": {
                    "00": 323,
                    "01": 701
                }
            }
        ]
    }
}
)bufferStr";

  AcceleratorBuffer buffer;
  std::istringstream s(bufferStr);
  buffer.load(s);
  EXPECT_EQ("q", buffer.name());
  EXPECT_EQ(2, buffer.size());
  std::stringstream ss;
  buffer.print(ss);
//   std::cout << "HELLO: " << ss.str() << "\n";
  EXPECT_EQ(ss.str(), bufferStr);
}
TEST(AcceleratorBufferTester, checkLoadDwave) {

  const std::string dwaveBuffer = R"dwaveBuffer({
    "AcceleratorBuffer": {
        "name": "q",
        "size": 2048,
        "Information": {
            "active-vars": [
                1944,
                1946,
                1947,
                1948,
                1949,
                1951
            ],
            "analysis-results": [
                3,
                5
            ],
            "embedding": {
                "0": [
                    1948,
                    1947
                ],
                "1": [
                    1949
                ],
                "2": [
                    1946,
                    1951
                ],
                "3": [
                    1944
                ]
            },
            "energies": [
                -999.75
            ],
            "execution-time": 0.023974,
            "ir-generator": "dwave-factoring",
            "num-occurrences": [
                100
            ]
        },
        "Measurements": {
            "010001": 100
        }
    }
}
)dwaveBuffer";

  AcceleratorBuffer buffer;
  std::istringstream s(dwaveBuffer);
  buffer.load(s);
  // EXPECT_EQ("q",buffer.name());
  // EXPECT_EQ(2,buffer.size());
  std::stringstream ss;

  buffer.print(ss);
  EXPECT_EQ(ss.str(), dwaveBuffer);
  std::cout << ss.str() << "\n";
  std::cout << dwaveBuffer << std::endl;

}

TEST(AcceleratorBufferTester, checkEmptyParametersBug) {

  const std::string str = R"str({
    "AcceleratorBuffer": {
        "name": "q",
        "size": 4,
        "Information": {
            "accelerator": "ibm",
            "algorithm": "vqe-energy",
            "vqe-energy": -1.135636239214126,
            "vqe-nQPU-calls": 1,
            "x-gates": "[0,1]"
        },
        "Measurements": {},
        "Children": [
            {
                "name": "Z2Z3",
                "Information": {
                    "coefficient": 0.17407289234592944,
                    "exp-val-z": 0.932373046875,
                    "kernel": "Z2Z3",
                    "parameters": [],
                    "ro-fixed-exp-val-z": 1.0342163557701439,
                    "time": 11.214285714285714
                },
                "Measurements": {
                    "0000": 15821,
                    "0100": 334,
                    "1000": 220,
                    "1100": 9
                }
            }
        ]
    }
}
)str";

  AcceleratorBuffer b;
  std::istringstream s(str);

  b.load(s);

  b.print();
}

TEST(AcceleratorBufferTester, checkMarginalCount) {
  {
    const std::string buffer_str = R"({
    "AcceleratorBuffer": {
        "name": "qreg_0x7fde3f773710",
        "size": 2,
        "Information": {},
        "Measurements": {
            "00": 499,
            "11": 525
        }
    }
    }
  )";

    AcceleratorBuffer buffer;
    std::istringstream s(buffer_str);
    buffer.load(s);
    buffer.print();
    auto marginal_count_q0 = buffer.getMarginalCounts({0});
    auto marginal_count_q1 = buffer.getMarginalCounts({1});
    EXPECT_EQ(marginal_count_q0["0"], marginal_count_q1["0"]);
    EXPECT_EQ(marginal_count_q0["1"], marginal_count_q1["1"]);
    EXPECT_EQ(marginal_count_q0["0"] + marginal_count_q0["1"], 1024);
    EXPECT_EQ(marginal_count_q1["0"] + marginal_count_q1["1"], 1024);
  }

  {
    const std::string buffer_str = R"({
    "AcceleratorBuffer": {
        "name": "qreg_0x7fde3f773710",
        "size": 3,
        "Information": {},
        "Measurements": {
            "001": 499,
            "111": 525
        }
    }
    }
  )";

    AcceleratorBuffer buffer;
    std::istringstream s(buffer_str);
    buffer.load(s);
    buffer.print();
    auto marginal_count_q0 = buffer.getMarginalCounts({0});
    auto marginal_count_q1q2 = buffer.getMarginalCounts({1, 2});
    EXPECT_EQ(marginal_count_q0.size(), 1);
    EXPECT_EQ(marginal_count_q0["1"], 1024);
    EXPECT_EQ(marginal_count_q1q2.size(), 2);

    EXPECT_EQ(marginal_count_q1q2["00"], 499);
    EXPECT_EQ(marginal_count_q1q2["11"], 525);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
