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
#include "QCSAccelerator.hpp"
#include <algorithm>
#include "CountGatesOfTypeVisitor.hpp"
#include "QuilVisitor.hpp"

#include "messages.hpp"
#include "xacc.hpp"
#include "xacc_service.hpp"
#include "EmbeddingAlgorithm.hpp"
#include <Eigen/Dense>

#include <regex>
#include <chrono>
#include <uuid/uuid.h>

#include "json.hpp"

namespace xacc {
namespace quantum {
void MapToPhysical::apply(std::shared_ptr<CompositeInstruction> function,
                     const std::shared_ptr<Accelerator> accelerator,
                     const HeterogeneousMap &options) {
// std::shared_ptr<IR> MapToPhysical::transform(std::shared_ptr<IR> ir) {

  auto embeddingAlgorithm = xacc::getService<EmbeddingAlgorithm>("cmr");

  std::map<int, int> physical2Logical, logical2Physical;
  int counter = 0;
  std::set<int> nUniqueBits;
  for (auto &edge : _edges) {
    nUniqueBits.insert(edge.first);
    nUniqueBits.insert(edge.second);
  }

  for (auto &i : nUniqueBits) {
    physical2Logical.insert({i, counter});
    logical2Physical.insert({counter, i});
    counter++;
  }

  int nBits = nUniqueBits.size();

  auto hardwareGraph = xacc::getService<Graph>("boost-ugraph");
  for (int i = 0; i < nBits; i++) {
    HeterogeneousMap m{std::make_pair("bias", 1.0)};
    hardwareGraph->addVertex(m);
  }
  for (auto &edge : _edges) {
    hardwareGraph->addEdge(physical2Logical[edge.first],
                           physical2Logical[edge.second]);
  }

  // hardwareGraph->write(std::cout);
//   for (auto &function : ir->getComposites()) {
    auto logicalGraph = function->toGraph();
    InstructionIterator it(function);
    std::set<int> nUniqueProbBits;

    std::vector<std::pair<int, int>> probEdges;
    while (it.hasNext()) {
      // Get the next node in the tree
      auto nextInst = it.next();
      if (nextInst->isEnabled() && nextInst->bits().size() == 2) {
        probEdges.push_back({nextInst->bits()[0], nextInst->bits()[1]});
        nUniqueProbBits.insert(nextInst->bits()[0]);
        nUniqueProbBits.insert(nextInst->bits()[1]);
      }
    }

    auto nProbBits = nUniqueProbBits.size();
    auto problemGraph = xacc::getService<Graph>("boost-ugraph");

    for (int i = 0; i < nProbBits; i++) {
      HeterogeneousMap m{std::make_pair("bias", 1.0)};
      problemGraph->addVertex(m);
    }

    for (auto &inst : probEdges) {
      if (!problemGraph->edgeExists(inst.first, inst.second)) {
        problemGraph->addEdge(inst.first, inst.second, 1.0);
      }
    }

    // Compute the minor graph embedding
    auto embedding = embeddingAlgorithm->embed(problemGraph, hardwareGraph);
    std::vector<std::size_t> physicalMap;
    for (auto &kv : embedding) {
      if (kv.second.size() > 1) {
        xacc::error("Invalid logical to physical qubit mapping.");
      }
      physicalMap.push_back(logical2Physical[kv.second[0]]);
    }

    // std::sort(physicalMap.begin(), physicalMap.end(), std::less<>());
    function->mapBits(physicalMap);
//   }

  return;
}


void QCSAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {
  for (auto f : functions) {
    auto tmpBuffer =
        std::make_shared<AcceleratorBuffer>(f->name(), buffer->size());
    execute(tmpBuffer, f);
    buffer->appendChild(tmpBuffer->name(), tmpBuffer);
  }
  return;
}

void QCSAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> function) {

  using json = nlohmann::json;

  // Map IR to Native Quil string
  auto visitor = std::make_shared<QuilVisitor>(true);
  std::set<int> qbitIdxs;
  InstructionIterator it(function);
  while (it.hasNext()) {
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
      if (nextInst->name() == "Measure") {
        qbitIdxs.insert(nextInst->bits()[0]);
      }
    }
  }
  CountGatesOfTypeVisitor<Measure> count(function);
  int nMeasures = count.countGates();
  auto quilStr = visitor->getQuilString();
  quilStr =
      "DECLARE ro BIT[" + std::to_string(buffer->size()) + "]\n" + quilStr;

  // Now we have our quil string, we need to run
  // the binary executable request.

  zmq::context_t context1, context2;
  zmq::socket_t qpu_compiler_socket(context1, zmq::socket_type::dealer);
  zmq::socket_t qpu_socket(context2, zmq::socket_type::dealer);

  qpu_compiler_socket.connect(qpu_compiler_endpoint);
  qpu_socket.connect(qpu_endpoint);

  uuid_t uuid;
  uuid_generate_time_safe(uuid);
  char uuid_str[37];
  uuid_unparse_lower(uuid, uuid_str);
  std::string id(uuid_str);

  // Run native_quil_to_binary, get binary
  // program from returned json
  qcs::BinaryExecutableRequest binExecReq(shots, quilStr);
  qcs::BinaryExecutableParams params(binExecReq);
  qcs::RPCRequestBinaryExecutable r(id, params);
  auto unpackedData = request(r, qpu_compiler_socket);
  std::stringstream ss;
  ss << unpackedData.get();
  auto execBinaryJson = json::parse(ss.str());
  auto prog = execBinaryJson["result"]["program"].dump();

  // Run execute_qpu_request, get job-id
  qcs::QPURequest qpuReq(prog, id);
  qcs::QPURequestParams qpuParams(qpuReq);
  qcs::RPCRequestQPURequest r2(id, qpuParams);
  auto unpackedData2 = request(r2, qpu_socket);
  std::stringstream ss2;
  ss2 << unpackedData2.get();
  auto qpuResponseJson = json::parse(ss2.str());
  auto waitId = qpuResponseJson["result"].dump();
  waitId = waitId.substr(1, waitId.length() - 2);

  // Run get_buffers, convdrt to GetBuffersResponse
  qcs::GetBuffersRequest getBuffers(waitId);
  qcs::RPCRequestGetBuffers r3(id, getBuffers);
  auto unpackedData3 = request(r3, qpu_socket);
  qcs::GetBuffersResponse gbresp;
  unpackedData3.get().convert(gbresp);

  // Decode the response, update AcceleratorBuffer
  ResultsDecoder().decode(buffer, gbresp, qbitIdxs, shots);
  return;
}

void ResultsDecoder::decode(std::shared_ptr<AcceleratorBuffer> buffer,
                            qcs::GetBuffersResponse &gbresp,
                            std::set<int> qbitIdxs, int shots) {

  Eigen::MatrixXi bits = Eigen::MatrixXi::Zero(shots, qbitIdxs.size());
  int counter = 0;
  for (auto &i : qbitIdxs) {
    std::string q = "q" + std::to_string(i);
    for (int k = 0; k < shots; k++) {
      bits(k, counter) = static_cast<int>(gbresp.result[q].data[k]);
    }
    counter++;
  }

  for (int i = 0; i < bits.rows(); i++) {

    std::string bitstr = "";
    for (int j = 0; j < bits.cols(); j++) {
      bitstr += std::to_string(bits(i, j));
    }

    buffer->appendMeasurement(bitstr);
  }
  return;
}
} // namespace quantum
} // namespace xacc
