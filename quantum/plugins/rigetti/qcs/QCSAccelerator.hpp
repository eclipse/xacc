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
#include <zmq.hpp>
#include <fstream>
#include "IRTransformation.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
using namespace rapidjson;

using namespace xacc;

namespace xacc {
namespace quantum {

// ResultsDecoder is a utility class for
// taking the raw binary results from the QPU
// and converting to bit strings and pushing to the
// AcceleratorBuffer
class ResultsDecoder {
public:
  void decode(std::shared_ptr<AcceleratorBuffer> buffer,
              qcs::GetBuffersResponse &results, std::set<int> qbitIdxs,
              int shots);
};

// MapToPhysical is an IRTransformation that maps
// the logical program IR qubit indices to the
// physical qubits available on the given lattice
class MapToPhysical : public xacc::IRTransformation {
protected:
  std::vector<std::pair<int, int>> _edges;
public:
  MapToPhysical(std::vector<std::pair<int, int>> &edges) : _edges(edges) {}
  void apply(std::shared_ptr<CompositeInstruction> program,
                     const std::shared_ptr<Accelerator> accelerator,
                     const HeterogeneousMap &options = {}) override;
  const IRTransformationType type() const override {return IRTransformationType::Placement;}
  const std::string name() const override { return "qcs-map-qubits"; }
  const std::string description() const override { return ""; }
};

class QCSAccelerator : virtual public Accelerator {
protected:
  std::vector<int> physicalQubits;
  std::vector<std::pair<int, int>> latticeEdges;
  Document latticeJson;
  std::string backend;
  int shots = 1024;
  std::string qpu_compiler_endpoint;
  std::string qpu_endpoint;

  template <typename T>
  msgpack::unpacked request(T &requestType, zmq::socket_t &socket) {
    msgpack::sbuffer sbuf;
    msgpack::pack(sbuf, requestType);
    zmq::message_t msg(sbuf.size());
    memcpy(msg.data(), sbuf.data(), sbuf.size());
    socket.send(msg);
    zmq::message_t reply;
    socket.recv(&reply, 0);
    msgpack::unpacked unpackedData;
    msgpack::unpack(unpackedData, static_cast<const char *>(reply.data()),
                    reply.size());
    return unpackedData;
  }

  void getSocketURLs() {
    std::ifstream stream(std::getenv("HOME") + std::string("/.forest_config"));
    std::string contents((std::istreambuf_iterator<char>(stream)),
                         std::istreambuf_iterator<char>());

    std::vector<std::string> lines;
    lines = xacc::split(contents, '\n');
    for (auto l : lines) {
      if (l.find("qpu_compiler_address") != std::string::npos) {
        std::vector<std::string> split = xacc::split(l, '=');
        auto key = split[1];
        xacc::trim(key);
        qpu_compiler_endpoint = key;
      } else if (l.find("qpu_endpoint_address") != std::string::npos) {
        std::vector<std::string> split;
        split = xacc::split(l, '=');
        auto key = split[1];
        xacc::trim(key);
        qpu_endpoint = key;
      }
    }

    if (qpu_compiler_endpoint.empty() || qpu_endpoint.empty()) {
      xacc::error("QCS Error: Cannot find qpu_compiler or qpu endpoint.");
    }
  }

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

    // Read .forest_config
    getSocketURLs();

    updateConfiguration(params);

    auto irt = std::make_shared<MapToPhysical>(latticeEdges);
    xacc::contributeService("qcs-map-qubits", irt);
  }

  const std::string defaultPlacementTransformation() override {return "qcs-map-qubits";}

  void updateConfiguration(const HeterogeneousMap &config) override {
    if (config.keyExists<int>("shots")) {
      shots = config.get<int>("shots");
    }
    if (config.stringExists("backend")) {
      backend = config.getString("backend");
    }
  }

  const std::vector<std::string> configurationKeys() override {
    return {"shots", "backend"};
  }

  HeterogeneousMap getProperties() override { return HeterogeneousMap(); }

  const std::string getSignature() override { return "qcs:" + backend; }

  std::vector<std::pair<int, int>> getConnectivity() override {
    if (!latticeEdges.empty()) {
      return latticeEdges;
    }
    return std::vector<std::pair<int, int>>{};
  }

  const std::string name() const override { return "qcs"; }
  const std::string description() const override { return ""; }

  virtual ~QCSAccelerator() {}
};

} // namespace quantum
} // namespace xacc

#endif
