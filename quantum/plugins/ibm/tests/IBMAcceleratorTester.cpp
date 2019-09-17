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
#include "IBMAccelerator.hpp"
#include "xacc_config.hpp"
#include "xacc_service.hpp"
#include "IRProvider.hpp"
#include "xacc.hpp"

#include <gtest/gtest.h>
#include <memory>

using namespace xacc;
using namespace xacc::quantum;

class FakeIBMClient : public Client {
public:
  const std::string post(const std::string &remoteUrl, const std::string &path,
                         const std::string &postStr,
                         std::map<std::string, std::string> headers) override {
    xacc::info("Fake POST: " +path +", " +postStr );
    if (path.find("loginWithToken") != std::string::npos) {
      std::string rres =
          "{\"id\":"
          "\"DyoTr43Tqj58Sz5mGeG4PEW0EoFi9u0V73iOEhZ0FZNU8bkT9e4RfCxEGGH9h1NY\""
          ",\"ttl\":1209600,\"created\":\"2019-05-28T17:13:32.471Z\","
          "\"userId\":\"5bb620a87f5037003c7a95e8\"}";
      xacc::info("Posting loginWithToken with dummy: " + rres);
      return rres;
    } else if (path.find("jobs?access_token=") != std::string::npos) {
        return "{\"id\": \"0123\"}";
    }

    return "{}";
  }

  const std::string get(const std::string &remoteUrl, const std::string &path,
                        std::map<std::string, std::string> headers) override {

    xacc::info("Fake GET PATH: " + path);
    std::string ibmTests = std::string(IBM_TEST_FILE_DIR);
    if (path.find("devices?access_token") != std::string::npos) {
      std::ifstream backendFile(ibmTests + "/backend_info.txt");
      std::string backendJson((std::istreambuf_iterator<char>(backendFile)),
                              std::istreambuf_iterator<char>());
      return backendJson;
    } else if (path.find("Jobs") != std::string::npos) {
      std::ifstream jobFile(ibmTests + "/jobs_info.txt");
      std::string jobJson((std::istreambuf_iterator<char>(jobFile)),
                          std::istreambuf_iterator<char>());
      return jobJson;
    }
    return "{}";
  }

  virtual ~FakeIBMClient() {}
};

TEST(IBMAcceleratorTester, checkTalkToRemoteServer) {

    xacc::setOption("ibm-backend", "ibmq_20_tokyo");
    auto client = std::make_shared<FakeIBMClient>();
    auto acc = xacc::getAccelerator("ibm", client);
    auto buffer = acc->createBuffer("q", 4);

    auto provider = xacc::getService<xacc::IRProvider>("gate");
    auto f = provider->createFunction("tmp", {});
    auto h = provider->createInstruction("H", {0});
    f->addInstruction(h);

    acc->execute(buffer, f);

    std::cout << buffer->getExpectationValueZ() << "\n";
}

int main(int argc, char **argv) {
    xacc::Initialize(argc, argv);
  ::testing::InitGoogleTest(&argc, argv);
  auto ret = RUN_ALL_TESTS();
//   xacc::Finalize();
  return ret;
}

// TEST(IBMAcceleratorTester, checkOpenPulseJson) {
  //   std::string file = "qcor_pulse_library.json";
  //   InstructionParameter q(std::vector<std::pair<double, double>>{
  //       {0.0, 0.0}, {0.5, 0.0}, {1.0, 0.0}});
  //   InstructionParameter q2(std::vector<std::pair<double, double>>{
  //       {0.0, 0.0}, {0.5, 0.0}, {1.0, 0.0}});
  //   xacc::appendCache(file, "sample1", q);
  //   xacc::appendCache(file, "sample2", q2);

  //   auto f = std::make_shared<GateFunction>("foo");

  //   auto x = std::make_shared<TestPulse>(std::vector<int>{0});
  //   x->setOption("pulse_id", InstructionParameter("sample1"));
  //   auto y = std::make_shared<TestPulse>(std::vector<int>{1});
  //   y->setOption("pulse_id", InstructionParameter("sample2"));

  //   f->addInstruction(x);
  //   f->addInstruction(y);

  //   IBMAccelerator acc;
  //   auto b = acc.createBuffer("q",2);

  //   auto json = acc.processInput(b, {f});

  //   std::cout << "JSON: " << json << "\n";

  //   auto expected = R"expected({\"qobj_id\": \"xacc-qobj\",
  //   \"schema_version\": \"1.0.0\", \"type\": \"PULSE\", \"header\": {
  //   \"description\": \"\", \"backend_name\": \"ibmq_qasm_simulator\" },
  //   \"config\": { \"meas_lo_freq\": [], \"qubit_lo_freq\": [], \"rep_time\":
  //   500, \"meas_level\": 1, \"shots\":1024, \"meas_return\": \"avg\",
  //   \"memory_slot_size\": 1, \"seed\": 1, \"pulse_library\": [{\"name\":
  //   \"sample1\", \"samples\": [[0,0], [0.5,0], [1,0]]},{\"name\":
  //   \"sample2\", \"samples\": [[0,0], [0.5,0], [1,0]]}] }, \"experiments\":
  //   [{ \"name\": \"foo\", \"instructions\": [{ \"ch\": \"d0\", \"name\":
  //   \"sample1\", \"t0\":0 },{ \"ch\": \"d1\", \"name\": \"sample2\",\"t0\":0
  //   }]}]})expected";

  // //   EXPECT_EQ(expected, json);

  //   std::string toRemove = xacc::getRootDirectory() + "/" + file;
  //   std::remove(toRemove.c_str());
  // }