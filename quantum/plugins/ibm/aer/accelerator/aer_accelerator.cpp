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
#include "aer_noise_model.hpp"
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
  connectivity.clear();

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
    connectivity = ibm->getConnectivity();
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
double AerAccelerator::calcExpectationValueZ(
    const std::vector<std::pair<double, double>> &in_stateVec,
    const std::vector<std::size_t> &in_bits) {
  const auto hasEvenParity =
      [](size_t x, const std::vector<size_t> &in_qubitIndices) -> bool {
    size_t count = 0;
    for (const auto &bitIdx : in_qubitIndices) {
      if (x & (1ULL << bitIdx)) {
        count++;
      }
    }
    return (count % 2) == 0;
  };

  double result = 0.0;
  for (uint64_t i = 0; i < in_stateVec.size(); ++i) {
    result += (hasEvenParity(i, in_bits) ? 1.0 : -1.0) *
              std::norm(std::complex<double>(in_stateVec[i].first,
                                             in_stateVec[i].second));
  }

  return result;
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
    std::vector<std::size_t> measured_bits;
    InstructionIterator iter(program);
    while (iter.hasNext()) {
      auto next = iter.next();
      if (!next->isComposite() && next->name() != "Measure") {
        tmp->addInstruction(next);
      } else if (next->name() == "Measure") {
        measured_bits.push_back(next->bits()[0]);
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

    auto exp_val = calcExpectationValueZ(state_vector, measured_bits);
    buffer->addExtraInfo("exp-val-z", exp_val);
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

void IbmqNoiseModel::initialize(const HeterogeneousMap &params) {
  m_nbQubits = 0;
  m_qubitT1.clear();
  m_qubitT2.clear();
  m_gateErrors.clear();
  m_gateDurations.clear();
  m_roErrors.clear();
  if (params.stringExists("backend")) {
    auto ibm = xacc::getAccelerator("ibm:" + params.getString("backend"));
    auto props = ibm->getProperties().get<std::string>("total-json");
    std::cout << "JSON: \n" << props << "\n";
    auto backEndJson = nlohmann::json::parse(props);
    // Parse qubit data:
    auto qubitsData = backEndJson["qubits"];
    size_t nbQubit = 0;
    for (auto qubitIter = qubitsData.begin(); qubitIter != qubitsData.end();
         ++qubitIter) {
      std::optional<double> meas0Prep1, meas1Prep0;
      // Each qubit contains an array of properties.
      for (auto probIter = qubitIter->begin(); probIter != qubitIter->end();
           ++probIter) {
        const auto probObj = *probIter;
        const std::string probName = probObj["name"].get<std::string>();
        const double probVal = probObj["value"].get<double>();
        const std::string unit = probObj["unit"].get<std::string>();
        if (probName == "T1") {
          assert(unit == "µs" || unit == "us" || unit == "ns");
          m_qubitT1.emplace_back((unit == "µs" || unit == "us") ? 1000.0 * probVal : probVal);
        }

        if (probName == "T2") {
          assert(unit == "µs" || unit == "us" || unit == "ns");
          m_qubitT2.emplace_back((unit == "µs" || unit == "us") ? 1000.0 * probVal : probVal);
        }

        if (probName == "prob_meas0_prep1") {
          assert(unit.empty());
          meas0Prep1 = probVal;
        }

        if (probName == "prob_meas1_prep0") {
          assert(unit.empty());
          meas1Prep0 = probVal;
        }
      }
      assert(meas0Prep1.has_value() && meas1Prep0.has_value());
      m_roErrors.emplace_back(std::make_pair(*meas0Prep1, *meas1Prep0));

      nbQubit++;
    }
    m_nbQubits = nbQubit;
    assert(m_qubitT1.size() == m_nbQubits);
    assert(m_qubitT2.size() == m_nbQubits);
    assert(m_roErrors.size() == m_nbQubits);

    // Parse gate data:
    auto gateData = backEndJson["gates"];
    for (auto gateIter = gateData.begin(); gateIter != gateData.end();
         ++gateIter) {
      auto gateObj = *gateIter;
      const std::string gateName = gateObj["name"].get<std::string>();
      auto gateParams = gateObj["parameters"];
      for (auto it = gateParams.begin(); it != gateParams.end(); ++it) {
        auto paramObj = *it;
        const std::string paramName = paramObj["name"].get<std::string>();
        if (paramName == "gate_length") {
          const std::string unit = paramObj["unit"].get<std::string>();
          assert(unit == "µs" || unit == "us" || unit == "ns");
          const double gateLength =
              (unit == "µs" || unit == "us") ? 1000.0 * paramObj["value"].get<double>()
                           : paramObj["value"].get<double>();
          const bool insertOk =
              m_gateDurations.insert({gateName, gateLength}).second;
          // Must not contain duplicates.
          assert(insertOk);
        }

        if (paramName == "gate_error") {
          assert(paramObj["unit"].get<std::string>().empty());
          const bool insertOk =
              m_gateErrors.insert({gateName, paramObj["value"].get<double>()})
                  .second;
          // Must not contain duplicates.
          assert(insertOk);
        }
      }
    }
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
    context.RegisterService<xacc::NoiseModel>(
        std::make_shared<xacc::quantum::IbmqNoiseModel>());
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(AerActivator)
