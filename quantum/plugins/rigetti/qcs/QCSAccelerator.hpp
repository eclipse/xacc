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
#ifndef QUANTUM_GATE_ACCELERATORS_QCSACCELERATOR_HPP_
#define QUANTUM_GATE_ACCELERATORS_QCSACCELERATOR_HPP_

#include "InstructionIterator.hpp"
#include "RemoteAccelerator.hpp"
#include "messages.hpp"

// #include <pybind11/embed.h>

// #include <dlfcn.h>
#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
using namespace rapidjson;

using namespace xacc;

namespace xacc {
namespace quantum {

class MapToPhysical : public xacc::IRTransformation {
protected:
  std::vector<std::pair<int, int>> _edges;

public:
  MapToPhysical(std::vector<std::pair<int, int>> &edges) : _edges(edges) {}
  std::shared_ptr<IR> transform(std::shared_ptr<IR> ir) override;
  bool hardwareDependent() override { return true; }
  const std::string name() const override { return "qcs-map-qubits"; }
  const std::string description() const override { return ""; }
};

// [Rigetti Forest]
// qvm_address = http://localhost:5000
// quilc_address = tcp://localhost:5555
// qpu_compiler_address = tcp://10.1.149.68:5555
// qpu_endpoint_address = tcp://10.1.149.68:50052

class QCSAccelerator : virtual public Accelerator {
protected:
  std::vector<int> physicalQubits;
  std::vector<std::pair<int, int>> latticeEdges;
  Document latticeJson;
  std::string backend;


public:
  QCSAccelerator() : Accelerator() {}

  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::shared_ptr<CompositeInstruction> function) override;
  void execute(std::shared_ptr<AcceleratorBuffer> buffer,
               const std::vector<std::shared_ptr<CompositeInstruction>>
                   functions) override;

  void initialize(const HeterogeneousMap &params = {}) override {
    if (params.stringExists("qcs-backend")) {
      backend = params.getString("qcs-backend");

      if (backend.find("-qvm") != std::string::npos) {
        backend.erase(backend.find("-qvm"), 4);
      }

      Client client;
      auto response = client.get("https://forest-server.qcs.rigetti.com",
                                 "/lattices/" + backend);

      latticeJson.Parse(response);
      const auto &oneq = latticeJson["lattice"]["isa"]["1Q"];
      auto &twoq = latticeJson["lattice"]["isa"]["2Q"];

      for (auto itr = oneq.MemberBegin(); itr != oneq.MemberEnd(); ++itr) {
        physicalQubits.push_back(std::stoi(itr->name.GetString()));
      }
      for (auto itr = twoq.MemberBegin(); itr != twoq.MemberEnd(); ++itr) {
        auto connStr = itr->name.GetString();
        auto split = xacc::split(connStr, '-');
        latticeEdges.push_back({std::stoi(split[0]), std::stoi(split[1])});
      }
    }

    //   const std::string endpoint = "tcp://127.0.0.1:5555";

    //   zmq::socket_t socket(context, zmq::socket_type::dealer);
    //   socket.connect(endpoint);
  }

  void updateConfiguration(const HeterogeneousMap &config) override {
    // if (config.keyExists<int>("shots")) {
    //   shots = config.get<int>("shots");
    // }
    // if (config.stringExists("backend")) {
    //   backend = config.getString("backend");
    // }
  }

  const std::vector<std::string> configurationKeys() override {
    return {"shots", "backend"};
  }

  HeterogeneousMap getProperties() override;

  const std::string getSignature() override { return "qcs:" + backend; }

  std::vector<std::pair<int, int>> getConnectivity() override {
    if (!latticeEdges.empty()) {
      return latticeEdges;
    }
    return std::vector<std::pair<int, int>>{};
  }

  std::vector<std::shared_ptr<IRTransformation>>
  getIRTransformations() override {
    std::vector<std::shared_ptr<IRTransformation>> v;
    if (!latticeEdges.empty()) {
      v.push_back(std::make_shared<MapToPhysical>(latticeEdges));
    }
    return v;
  }

  const std::string name() const override { return "qcs"; }
  const std::string description() const override { return ""; }

  /**
   * The destructor
   */
  virtual ~QCSAccelerator() {}
};

} // namespace quantum
} // namespace xacc

#endif
