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
 *   Thien Nguyen - initial API and implementation
 *******************************************************************************/
#include "aer_accelerator.hpp"

#include "CommonGates.hpp"
#include "CountGatesOfTypeVisitor.hpp"
#include "InstructionIterator.hpp"

#include "controllers/controller.hpp"
#include "controllers/controller_execute.hpp"
#include "controllers/qasm_controller.hpp"
#include "controllers/statevector_controller.hpp"
#include "noise/readout_error.hpp"

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"
#include "xacc.hpp"

#include <bitset>

namespace xacc {
namespace quantum {

#define IS_INTEGRAL(T)                                                         \
  typename std::enable_if<std::is_integral<T>::value>::type * = 0

template <class T>
std::string integral_to_binary_string(T byte, IS_INTEGRAL(T)) {
  std::bitset<sizeof(T) * 8> bs(byte);
  return bs.to_string();
}
std::string hex_string_to_binary_string(std::string hex) {
  return integral_to_binary_string((int)strtol(hex.c_str(), NULL, 0));
}

void AerAccelerator::initialize(const HeterogeneousMap &params) {

  noise_model.clear();
  m_simtype = "qasm";

  xacc_to_qobj = xacc::getCompiler("qobj");
  if (params.keyExists<int>("shots")) {
    m_shots = params.get<int>("shots");
  }
  if (params.stringExists("sim-type")) {
    if (!xacc::container::contains(
            std::vector<std::string>{"qasm", "statevector"},
            params.getString("sim-type"))) {
      xacc::warning("[Aer] warning, invalid sim-type (" +
                    params.getString("sim-type") +
                    "), must be qasm or statevector.");
    } else {
      m_simtype = params.getString("sim-type");
    }
  }

  if (params.stringExists("backend")) {
    auto ibm = xacc::getAccelerator("ibm:" + params.getString("backend"));
    auto props = ibm->getProperties().get<std::string>("total-json");
    auto props_json = nlohmann::json::parse(props);

    // nlohmann::json errors_json;
    std::vector<nlohmann::json> elements;
    std::size_t qbit = 0;
    for (auto it = props_json["qubits"].begin();
         it != props_json["qubits"].end(); ++it) {

      std::vector<double> value{(*(it->begin() + 5))["value"].get<double>(),
                                (*(it->begin() + 4))["value"].get<double>()};
      std::vector<std::vector<double>> probs{{1 - value[0], value[0]},
                                             {value[1], 1 - value[1]}};

      nlohmann::json element;
      element["type"] = "roerror";
      element["operations"] = std::vector<std::string>{"measure"};
      element["probabilities"] = probs;
      element["gate_qubits"] = std::vector<std::vector<std::size_t>>{{qbit}};

      elements.push_back(element);

      qbit++;
    }

    noise_model["errors"] = elements;
    // noise_model["x90_gates"] = std::vecto

    std::cout << "NoiseModelJson:\n" << noise_model.dump(4) << "\n";
  }
}

void AerAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> program) {

  if (m_simtype == "qasm") {
    auto qobj_str = xacc_to_qobj->translate(program);

    nlohmann::json j = nlohmann::json::parse(qobj_str)["qObject"];
    j["config"]["shots"] = m_shots;
    j["config"]["noise_model"] = noise_model;

    auto results_json = nlohmann::json::parse(
        AER::controller_execute_json<AER::Simulator::QasmController>(j.dump()));

    auto results = *results_json["results"].begin();

    auto counts = results["data"]["counts"].get<std::map<std::string, int>>();
    for (auto &kv : counts) {
      std::string hexStr = kv.first;
      int nOccurrences = kv.second;
      auto bitStr = hex_string_to_binary_string(hexStr);

      // Process bitStr to be an n-Measure string in msb
      std::string actual = "";
      CountGatesOfTypeVisitor<Measure> cc(program);
      int nMeasures = cc.countGates();
      for (int i = 0; i < nMeasures; i++)
        actual += "0";
      for (int i = 0; i < nMeasures; i++)
        actual[actual.length() - 1 - i] = bitStr[bitStr.length() - i - 1];

      buffer->appendMeasurement(actual, nOccurrences);
    }
  } else {
    // statevector
    // remove all measures, don't need them
    auto tmp = xacc::ir::asComposite(program->clone());
    tmp->clear();
    InstructionIterator iter(program);
    while (iter.hasNext()) {
      auto next = iter.next();
      if (!next->isComposite() && next->name() != "Measure") {
        tmp->addInstruction(next);
      }
    }
    auto qobj_str = xacc_to_qobj->translate(tmp);

    nlohmann::json j = nlohmann::json::parse(qobj_str)["qObject"];

    auto results_json = nlohmann::json::parse(
        AER::controller_execute_json<AER::Simulator::StatevectorController>(
            j.dump()));

    auto results = *results_json["results"].begin();

    auto state_vector = results["data"]["statevector"]
                            .get<std::vector<std::pair<double, double>>>();

    buffer->addExtraInfo("state", state_vector);

    // std::cout << "SV measure:\n";
    // for (auto& [k,v] : counts) std::cout << k << " : " << v << "\n";
    
    // const auto hasEvenParity =
    //     [](size_t x, const std::vector<size_t> &in_qubitIndices) -> bool {
    //   size_t count = 0;
    //   for (const auto &bitIdx : in_qubitIndices) {
    //     if (x & (1ULL << bitIdx)) {
    //       count++;
    //     }
    //   }
    //   return (count % 2) == 0;
    // };

    // double result = 0.0;
    // for (uint64_t i = 0; i < m_stateVec.size(); ++i) {
    //   result += (hasEvenParity(i, m_measureBits) ? 1.0 : -1.0) *
    //             std::norm(std::complex<double>(state_vector[i][0], state_vector[i][1]));
    // }
    // buffer->addExtraInfo("exp-val-z", result);
  }
}

void AerAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>>
        compositeInstructions) {
  for (auto &f : compositeInstructions) {
    auto tmpBuffer =
        std::make_shared<xacc::AcceleratorBuffer>(f->name(), buffer->size());
    execute(tmpBuffer, f);
    buffer->appendChild(f->name(), tmpBuffer);
  }
}
} // namespace quantum
} // namespace xacc

using namespace cppmicroservices;

namespace {

/**
 */
class US_ABI_LOCAL AerActivator : public BundleActivator {

public:
  AerActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto xt = std::make_shared<xacc::quantum::AerAccelerator>();
    context.RegisterService<xacc::Accelerator>(xt);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(AerActivator)
