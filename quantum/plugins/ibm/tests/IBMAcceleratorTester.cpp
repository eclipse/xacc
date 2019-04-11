/***********************************************************************************
 * Copyright (c) 2018, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#include "InstructionIterator.hpp"
#include "InstructionParameter.hpp"
#include "IBMAccelerator.hpp"
#include "OpenQasmVisitor.hpp"

#include "TestPulse.hpp"
#include <gtest/gtest.h>
#include <memory>

using namespace xacc;
using namespace xacc::quantum;

TEST(IBMAcceleratorTester, checkOpenQasmJson) {
    auto f = std::make_shared<GateFunction>("foo");

	auto x = std::make_shared<X>(0);
	auto h = std::make_shared<Hadamard>(1);
	auto cn1 = std::make_shared<CNOT>(1, 2);
	auto cn2 = std::make_shared<CNOT>(0, 1);
	auto h2 = std::make_shared<Hadamard>(0);
	auto m0 = std::make_shared<Measure>(0, 0);
	auto m1 = std::make_shared<Measure>(1,1);

	f->addInstruction(x);
	f->addInstruction(h);
	f->addInstruction(cn1);
	f->addInstruction(cn2);
	f->addInstruction(h2);
	f->addInstruction(m0);
	f->addInstruction(m1);

    IBMAccelerator acc;
    auto b = acc.createBuffer("q",2);

    auto json = acc.processInput(b, {f});

    std::cout << "JSON:\n" << json << "\n";

}
TEST(IBMAcceleratorTester, checkOpenPulseJson) {
  std::string file = "qcor_pulse_library.json";
  InstructionParameter q(std::vector<std::pair<double, double>>{
      {0.0, 0.0}, {0.5, 0.0}, {1.0, 0.0}});
  InstructionParameter q2(std::vector<std::pair<double, double>>{
      {0.0, 0.0}, {0.5, 0.0}, {1.0, 0.0}});
  xacc::appendCache(file, "sample1", q);
  xacc::appendCache(file, "sample2", q2);

  auto f = std::make_shared<GateFunction>("foo");

  auto x = std::make_shared<TestPulse>(std::vector<int>{0});
  x->setOption("pulse_id", InstructionParameter("sample1"));
  auto y = std::make_shared<TestPulse>(std::vector<int>{1});
  y->setOption("pulse_id", InstructionParameter("sample2"));

  f->addInstruction(x);
  f->addInstruction(y);
  
  IBMAccelerator acc;
  auto b = acc.createBuffer("q",2);
  
  auto json = acc.processInput(b, {f});
  
  std::cout << "JSON: " << json << "\n";
  
  auto expected = R"expected({\"qobj_id\": \"xacc-qobj\", \"schema_version\": \"1.0.0\", \"type\": \"PULSE\", \"header\": { \"description\": \"\", \"backend_name\": \"ibmq_qasm_simulator\" }, \"config\": { \"meas_lo_freq\": [], \"qubit_lo_freq\": [], \"rep_time\": 500, \"meas_level\": 1, \"shots\":1024, \"meas_return\": \"avg\", \"memory_slot_size\": 1, \"seed\": 1, \"pulse_library\": [{\"name\": \"sample1\", \"samples\": [[0,0], [0.5,0], [1,0]]},{\"name\": \"sample2\", \"samples\": [[0,0], [0.5,0], [1,0]]}] }, \"experiments\": [{ \"name\": \"foo\", \"instructions\": [{ \"ch\": \"d0\", \"name\": \"sample1\", \"t0\":0 },{ \"ch\": \"d1\", \"name\": \"sample2\",\"t0\":0 }]}]})expected";

//   EXPECT_EQ(expected, json);
  
  std::string toRemove = xacc::getRootDirectory() + "/" + file;
  std::remove(toRemove.c_str());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
