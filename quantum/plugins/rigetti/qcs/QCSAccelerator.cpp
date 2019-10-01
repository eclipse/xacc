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

#include <regex>
#include <chrono>
#include <uuid/uuid.h>

#include <zmq.hpp>

using namespace std;
using namespace std::chrono;

// namespace py = pybind11;
// using namespace py::literals;

namespace xacc {
namespace quantum {

std::shared_ptr<IR> MapToPhysical::transform(std::shared_ptr<IR> ir) {

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
  for (auto &function : ir->getComposites()) {
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

    //    std::cout << "\n";
    //  problemGraph->write(std::cout);

    // Compute the minor graph embedding
    auto embedding = embeddingAlgorithm->embed(problemGraph, hardwareGraph);
    //    embedding.persist(std::cout);
    std::vector<int> physicalMap;
    for (auto &kv : embedding) {
      if (kv.second.size() > 1) {
        xacc::error("Invalid logical to physical qubit mapping.");
      }
      physicalMap.push_back(logical2Physical[kv.second[0]]);
    }

    // std::cout << "Physical bits:\n";
    // for (auto& b : physicalMap) std::cout << b << "\n";
    // function->mapBits(physicalMap);
  }

  return ir;
}

void QCSAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> function) {
  auto visitor = std::make_shared<QuilVisitor>(true);

  auto shots = 1024;

      std::map<int, int>
          bitToQubit;
  std::vector<int> tobesorted;
  InstructionIterator it(function);
  while (it.hasNext()) {
    // Get the next node in the tree
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
    }
  }

  CountGatesOfTypeVisitor<Measure> count(function);
  int nMeasures = count.countGates();
  auto quilStr = visitor->getQuilString();
  quilStr =
      "DECLARE ro BIT[" + std::to_string(buffer->size()) + "]\n" + quilStr;

  const string endpoint = "tcp://127.0.0.1:5555";
  zmq::context_t context;
  zmq::socket_t socket(context, zmq::socket_type::dealer);
  socket.connect(endpoint);

  uuid_t uuid;
  uuid_generate_time_safe(uuid);
  char uuid_str[37]; // ex. "1b4e28ba-2fa1-11d2-883f-0016d3cca427" + "\0"
  uuid_unparse_lower(uuid, uuid_str);

  std::string id(uuid_str);
  qcs::BinaryExecutableRequest binExecReq(shots, quilStr);
  qcs::BinaryExecutableParams params(binExecReq);
  qcs::RPCRequestBinaryExecutable r(id, params);

  msgpack::sbuffer sbuf;
  msgpack::pack(sbuf, r);
  zmq::message_t msg(sbuf.size());
  memcpy(msg.data(), sbuf.data(), sbuf.size());

  std::cout << msg << "\n";

  socket.send(msg);

  zmq::message_t reply;
  socket.recv(&reply, 0);
  std::cout << reply.data() << "\n";
  msgpack::unpacked unpackedData;
  msgpack::unpack(unpackedData, static_cast<const char *>(reply.data()),
                  reply.size());
  std::stringstream ss;
  ss << unpackedData.get();

  auto prog = "";

  std::cout << "GOT THE JSON:\n" << ss.str() << "\n";

  // QPU REQUEST
  qcs::QPURequest qpuReq(prog, id);
  qcs::QPURequestParams qpuParams(qpuReq);
  qcs::RPCRequestQPURequest r2(id, qpuParams);
  msgpack::sbuffer sbuf2;
  msgpack::pack(sbuf2, r2);
  zmq::message_t msg2(sbuf.size());
  memcpy(msg2.data(), sbuf2.data(), sbuf2.size());

  std::cout << msg2 << "\n";

  socket.send(msg2);

  zmq::message_t reply2;
  socket.recv(&reply2, 0);
  std::cout << reply2.data() << "\n";
  msgpack::unpacked unpackedData2;
  msgpack::unpack(unpackedData2, static_cast<const char *>(reply2.data()),
                  reply2.size());
  std::stringstream ss2;
  ss2 << unpackedData2.get();

  auto waitId = "";
  // GET BUFFERS
  qcs::GetBuffersRequest getBuffers(waitId);
  qcs::RPCRequestGetBuffers r3(id, getBuffers);

  msgpack::sbuffer sbuf3;
  msgpack::pack(sbuf3, r3);
  zmq::message_t msg3(sbuf.size());
  memcpy(msg3.data(), sbuf3.data(), sbuf3.size());

  std::cout << msg3 << "\n";

  socket.send(msg3);

  zmq::message_t reply3;
  socket.recv(&reply3, 0);
  std::cout << reply3.data() << "\n";
  msgpack::unpacked unpackedData3;
  msgpack::unpack(unpackedData3, static_cast<const char *>(reply3.data()),
                  reply3.size());
  std::stringstream ss3;
  ss3 << unpackedData3.get();

  // now we have json results

  return;
}

void QCSAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::vector<std::shared_ptr<CompositeInstruction>> functions) {

  //   std::vector<std::shared_ptr<AcceleratorBuffer>> tmpBuffers;
  //   int counter = 1;
  //   for (auto f : functions) {
  //     auto tmpBuffer = createBuffer(f->name(), buffer->size());

  //     high_resolution_clock::time_point t1 = high_resolution_clock::now();
  //     // xacc::info("Execution " + std::to_string(counter) + ": " +
  //     f->name()); execute(tmpBuffer, f); high_resolution_clock::time_point t2
  //     = high_resolution_clock::now();

  //     auto duration = duration_cast<microseconds>(t2 - t1).count();
  //     tmpBuffer->addExtraInfo("exec-time", ExtraInfo(duration * 1e-6));
  //     tmpBuffers.push_back(tmpBuffer);
  //     counter++;
  //   }

  //   return tmpBuffers;
}

} // namespace quantum
} // namespace xacc
