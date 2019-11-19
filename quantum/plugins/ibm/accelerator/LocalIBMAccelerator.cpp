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
#include "LocalIBMAccelerator.hpp"
#include "xacc.hpp"
#include "json.hpp"

#include "simulator.hpp"
#include <regex>

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

using namespace rapidjson;

// for convenience
using json = nlohmann::json;

namespace xacc {
namespace quantum {

HeterogeneousMap LocalIBMAccelerator::getProperties() {
    std::vector<double> p01s(20, global_p01), p10s(20, global_p10);
    HeterogeneousMap m;
    // std::cout << p01s << "\n";
    m.insert("p01s", p01s);
    m.insert("p10s", p10s);
    return m;
}

void LocalIBMAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<xacc::CompositeInstruction> kernel) {
  execute(buffer, std::vector<std::shared_ptr<xacc::CompositeInstruction>>{kernel});
  for (auto &kv : buffer->getChildren()[0]->getMeasurementCounts()) {
    for (int i = 0; i < kv.second; i++)
      buffer->appendMeasurement(kv.first);
  }

  buffer->removeChild(0);
}

void LocalIBMAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {

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

    // std::cout << "LOCAL NQ: " << buffer->size() << "\n";
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
  config2["shots"] = shots;

  if (readout_errors) {//cc::optionExists("local-ibm-ro-error")) {
    // auto probStr = xacc::getOption("local-ibm-ro-error");
    // std::vector<std::string> split;
    // split = xacc::split(probStr, ',');
    // boost::split(split, probStr, boost::is_any_of(","));
    config2["noise_params"]["readout_error"] = {global_p10, global_p01};
    // for (int i = 1; i < split.size(); i++) {
    //   config2["noise_params"]["readout_error"].push_back(std::stod(split[i]));
    // }
  }

  if (u_depol_noise) {
      config2["noise_params"]["U"]["p_depol"] = udepol;
  }

  if (cx_depol_noise) {
      config2["noise_params"]["CX"]["p_depol"] = cxdepol;
  }

  j["config"] = config2;
  j["id"] = "fakeid";

//   std::cout << "JSON:\n" << j.dump(4) << "\n";
// std::cout << "Execing\n";
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
    auto tmpBuffer = std::make_shared<AcceleratorBuffer>(names[kernelCounter],buffer->size());
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

    buffer->appendChild(names[kernelCounter],tmpBuffer);
    kernelCounter++;

    // buffers.push_back(tmpBuffer);
  }

  measurementSupports.clear();
  return;
}
} // namespace quantum
} // namespace xacc
