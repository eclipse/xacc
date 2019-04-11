/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
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
#include "LocalIBMAccelerator.hpp"
#include "XACC.hpp"
#include "json.hpp"
#include "simulator.hpp"
#include <regex>

// for convenience
using json = nlohmann::json;

namespace xacc {
namespace quantum {

std::shared_ptr<AcceleratorBuffer>
LocalIBMAccelerator::createBuffer(const std::string &varId) {
  if (!isValidBufferSize(30)) {
    xacc::error("Invalid buffer size.");
  }

  std::shared_ptr<AcceleratorBuffer> buffer =
      std::make_shared<AcceleratorBuffer>(varId, 30);

  storeBuffer(varId, buffer);
  return buffer;
}

std::shared_ptr<AcceleratorBuffer>
LocalIBMAccelerator::createBuffer(const std::string &varId, const int size) {
  if (!isValidBufferSize(size)) {
    xacc::error("Invalid buffer size.");
  }

  std::shared_ptr<AcceleratorBuffer> buffer =
      std::make_shared<AcceleratorBuffer>(varId, size);

  storeBuffer(varId, buffer);
  return buffer;
}

bool LocalIBMAccelerator::isValidBufferSize(const int NBits) {
  return NBits > 0 && NBits < 31;
}

std::vector<std::shared_ptr<IRTransformation>>
LocalIBMAccelerator::getIRTransformations() {
  std::vector<std::shared_ptr<IRTransformation>> transformations;
  return transformations;
}

void LocalIBMAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<xacc::Function> kernel) {
  auto buffers =
      execute(buffer, std::vector<std::shared_ptr<xacc::Function>>{kernel});
  for (auto &kv : buffers[0]->getMeasurementCounts()) {
    for (int i = 0; i < kv.second; i++)
      buffer->appendMeasurement(kv.first);
  }
}

std::vector<std::shared_ptr<AcceleratorBuffer>> LocalIBMAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<Function>> functions) {

  std::vector<std::string> names;
  json j;
  int kernelCounter = 0;
  for (auto kernel : functions) {
    auto visitor = std::make_shared<OpenQasmVisitor>(buffer->size());
    names.push_back(kernel->name());

    json h, circuit, config;
    for (int i = 0; i < buffer->size(); i++) {
      h["qubit_labels"].push_back({"q", i});
    }

    InstructionIterator it(kernel);
    measurementSupports.insert(
        std::make_pair(kernelCounter, std::vector<int>{}));
    while (it.hasNext()) {
      auto nextInst = it.next();
      if (nextInst->isEnabled()) {
        nextInst->accept(visitor);
        if (nextInst->name() == "Measure") {
          auto qbitIdx = nextInst->bits()[0];
          measurementSupports[kernelCounter].push_back(qbitIdx);
        }
      }
    }

    auto qasmStr = visitor->getOpenQasmString();
    auto opsStr = visitor->getOperationsJsonString();

    qasmStr = std::regex_replace(qasmStr, std::regex("\n"), "\\n") ;

    // boost::replace_all(qasmStr, "\n", "\\n");

    h["clbit_labels"].push_back(
        {"c", measurementSupports[kernelCounter].size()});

    json nq, nc;
    h["number_of_qubits"] = buffer->size();
    h["number_of_clbits"] = measurementSupports[kernelCounter].size();

    circuit["compiled_circuit"]["header"] = h;

    json operations = json::parse(opsStr);

    circuit["compiled_circuit"]["compiled_circuit_qasm"] = qasmStr;
    circuit["compiled_circuit"]["name"] = kernel->name();
    circuit["compiled_circuit"]["operations"] = operations;

    config["basis_gates"] = "u1,u2,u3,cx,id,snapshot";
    config["coupling_map"] = "None";
    config["seed"] = "None";
    config["layout"] = "None";

    circuit["compiled_circuit"]["config"] = config;

    kernelCounter++;

    j["circuits"].push_back(circuit);
  }

  json config2;
  config2["backend"] = "qasm_simulator_cpp";
  config2["max_credits"] = 10;
  config2["shots"] = xacc::optionExists("ibm-shots")
                         ? std::stoi(xacc::getOption("ibm-shots"))
                         : 1024;

  if (xacc::optionExists("local-ibm-ro-error")) {
    auto probStr = xacc::getOption("local-ibm-ro-error");
    std::vector<std::string> split;
    split = xacc::split(probStr, ',');
    // boost::split(split, probStr, boost::is_any_of(","));
    config2["noise_params"]["readout_error"] = {std::stod(split[0])};
    for (int i = 1; i < split.size(); i++) {
      config2["noise_params"]["readout_error"].push_back(std::stod(split[i]));
    }
  }

  if (xacc::optionExists("u-p-depol")) {
      auto u_depol = std::stod(xacc::getOption("u-p-depol"));
      config2["noise_params"]["U"]["p_depol"] = u_depol;
  }

  if (xacc::optionExists("cx-p-depol")) {
      auto cx_depol = std::stod(xacc::getOption("cx-p-depol"));
      config2["noise_params"]["CX"]["p_depol"] = cx_depol;
  }

//   if (xacc::optionExists("cx-u-error")) {
//       auto cx_uerror_str = xacc::getOption("cx-u-error");
//       for (int i = 0; i < cx_uerror_str.size(); i++) {



//       }

//     //   config2["noise_params"]["CX"]["U_error"] = cx_uerror;
//   }
  j["config"] = config2;
  j["id"] = "fakeid";

//   std::cout << "JSON:\n" << j.dump(4) << "\n";

  std::string response = "";
  try {
    QISKIT::Simulator sim;
    sim.load_qobj_json(j);

    // Execute
    response = sim.execute();
    // xacc::info("IBM Sim Output:\n" + response);
  } catch (std::exception &e) {
    std::stringstream msg;
    msg << "Failed to execute qobj (" << e.what() << ")";
    xacc::error("local qasm sim error: " + msg.str());
  }

  std::vector<std::shared_ptr<AcceleratorBuffer>> buffers;

  json results = json::parse(response)["result"];

//   std::cout << "HELLO RESULTS: " << response << "\n\n"
            // << results.dump(4) << "\n";
  // N Measurements
  kernelCounter = 0;
  for (auto &result : results) {
    auto tmpBuffer = createBuffer(names[kernelCounter], buffer->size());
    auto measureSupports = measurementSupports[kernelCounter];

    int countInt = 0;
    auto counts = result["data"]["counts"];
    for (auto it = counts.begin(); it != counts.end(); ++it) {
      countInt = it.value().get<int>();
      auto bitString = it.key();

      if (bitString.length() < buffer->size()) {

          std::string s = "";
          for (int i = 0; i < buffer->size(); i++) s += "0";

          std::sort(measureSupports.begin(), measureSupports.end());
          int counter = 0;
          for (int i = bitString.length()-1; i >= 0; i--) {
              auto bit = bitString[i];
              s[buffer->size()-1-measureSupports[counter]] = bit;
              counter++;
          }

          bitString = s;
      }

      for (int i = 0; i < countInt; i++)
        tmpBuffer->appendMeasurement(bitString);
    }

    kernelCounter++;
    buffers.push_back(tmpBuffer);
  }

  measurementSupports.clear();
  return buffers;
}
} // namespace quantum
} // namespace xacc
