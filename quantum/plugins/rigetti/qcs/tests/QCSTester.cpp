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
#include <memory>
#include <gtest/gtest.h>
#include <regex>
#include <vector>

#include "json.hpp"
#include <Eigen/Dense>

#include "xacc_service.hpp"
#include "xacc.hpp"
using namespace xacc;

class ResultsDecoder {
public:

  std::shared_ptr<AcceleratorBuffer> decode(const std::string jsonresults, std::vector<int> qbitIdxs, int shots) {

   auto j = std::regex_replace(jsonresults, std::regex("b'"), "\"");
   j = std::regex_replace(j, std::regex("'"), "\"");
   j.erase(std::remove(j.begin(), j.end(), '\\'), j.end());

   std::cout << "jsonresults: " << j << "\n";

  using json = nlohmann::json;

  auto jj = json::parse(j);

  Eigen::MatrixXi bits = Eigen::MatrixXi::Zero(shots, qbitIdxs.size());
  int counter = 0;
  for (auto& i : qbitIdxs) {
      auto shape = jj["result"]["q"+std::to_string(i)]["shape"][0].get<int>();
      auto data = jj["result"]["q"+std::to_string(i)]["data"].get<std::string>();
      std::cout << "SHAPE: " << shape << "\n";
      int kcounter = 0;
      for (int k = 0; k < shape*3; k+=3) {
          std::cout << "BIT: " << data.substr(k, 3) << "\n";
          std::string hex = data.substr(k,3);
          xacc::trim(hex);
          int bit = hex == "x01" ? 1 : 0;
          bits(kcounter, counter) = bit;
          kcounter++;
      }
      counter++;
  }

  std::cout << bits << "\n";
auto buffer = xacc::qalloc(qbitIdxs.size());
  for (int i = 0; i < bits.rows(); i++) {

      std::string bitstr = "";
      for (int j = 0; j < bits.cols(); j++) {
          bitstr += std::to_string(bits(i,j));
      }

      buffer->appendMeasurement(bitstr);
  }

  buffer->print();
  return buffer;

  }
};
TEST(ResultsDecoderTester, checkSimple) {
auto src = R"src({"result":{ 'q1': {'shape': [5], 'dtype': 'int8', 'data': b'\x01\x01\x01\x01\x01'}, 'q2': {'shape': [5], 'dtype': 'int8', 'data': b'\x00\x00\x00\x00\x00'}}})src";

ResultsDecoder decoder;
auto buffer = decoder.decode(src, {1,2}, 5);

}

int main(int argc, char **argv) {
    xacc::Initialize();
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
  xacc::Finalize();
  return ret;
}
