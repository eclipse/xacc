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

#include <zmq.hpp>

#include "json.hpp"
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
    std::vector<std::size_t> physicalMap;
    for (auto &kv : embedding) {
      if (kv.second.size() > 1) {
        xacc::error("Invalid logical to physical qubit mapping.");
      }
      physicalMap.push_back(logical2Physical[kv.second[0]]);
    }

    // std::cout << "Physical bits:\n";
    // for (auto& b : physicalMap) std::cout << b << "\n";
//    function->mapBits(physicalMap);
  }

  return ir;
}

void QCSAccelerator::execute(
    std::shared_ptr<AcceleratorBuffer> buffer,
    const std::shared_ptr<CompositeInstruction> function) {
  auto visitor = std::make_shared<QuilVisitor>(true);

  auto shots = 10;

  std::set<int> qbitIdxs;
  InstructionIterator it(function);
std::cout << "START LOOPING\n";
  while (it.hasNext()) {
    // Get the next node in the tree
    auto nextInst = it.next();
    if (nextInst->isEnabled()) {
      nextInst->accept(visitor);
         if (nextInst->name() == "Measure") {
      qbitIdxs.insert(nextInst->bits()[0]);
         }
    }
  }
  std::cout <<" MADE IT HERE\n";
  CountGatesOfTypeVisitor<Measure> count(function);
  int nMeasures = count.countGates();
  auto quilStr = visitor->getQuilString();
  quilStr =
      "DECLARE ro BIT[" + std::to_string(buffer->size()) + "]\n" + quilStr;

  const string endpoint = "tcp://10.1.149.68:5555";
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

  std::cout << "GOT THE JSON:\n" << ss.str() << "\n";
  using json = nlohmann::json;
  auto execBinaryJson = json::parse(ss.str());
  auto prog = execBinaryJson["result"]["program"].dump();
  std::cout << "GOT HTE PROGRAM:\n" << prog.substr(0, 100) << "\n";

  // QPU REQUESTS
  const string endpoint2 = "tcp://10.1.149.68:50052";
  zmq::context_t context2;
  zmq::socket_t socket2(context2, zmq::socket_type::dealer);
  socket2.connect(endpoint2);

  qcs::RewriteArithmeticRequest rewrite(quilStr);
  qcs::RewriteArithmeticParams rparams(rewrite);
  qcs::RPCRequestRewriteArithmetic r4(id, rparams);

  msgpack::sbuffer sbuf4;
  msgpack::pack(sbuf4, r4);
  zmq::message_t msg4(sbuf4.size());
  memcpy(msg4.data(), sbuf4.data(), sbuf4.size());

  std::cout << msg4 << "\n";

  socket2.send(msg4);

  zmq::message_t reply4;
  socket.recv(&reply4, 0);
  std::cout << reply4.data() << "\n";
  msgpack::unpacked unpackedData4;
  msgpack::unpack(unpackedData4, static_cast<const char *>(reply4.data()),
                  reply4.size());
  std::stringstream ss4;
  ss4 << unpackedData4.get();

  std::cout<< "RWA:\n" << ss4.str() << "\n";

  qcs::QPURequest qpuReq(prog, id);
  qcs::QPURequestParams qpuParams(qpuReq);
  qcs::RPCRequestQPURequest r2(id, qpuParams);
  msgpack::sbuffer sbuf2;
  msgpack::pack(sbuf2, r2);
  zmq::message_t msg2(sbuf2.size());
  memcpy(msg2.data(), sbuf2.data(), sbuf2.size());

  std::cout << msg2 << "\n";

  socket2.send(msg2);

  zmq::message_t reply2;
  socket2.recv(&reply2, 0);
  std::cout << reply2.data() << "\n";
  msgpack::unpacked unpackedData2;
  msgpack::unpack(unpackedData2, static_cast<const char *>(reply2.data()),
                  reply2.size());
  std::stringstream ss2;
  ss2 << unpackedData2.get();

  std::cout << "QPUReq response = \n" << ss2.str() << "\n";
  auto qpuResponseJson = json::parse(ss2.str());
  auto waitId = qpuResponseJson["result"].dump();
  waitId = waitId.substr(1, waitId.length() - 2);

  // GET BUFFERS
  qcs::GetBuffersRequest getBuffers(waitId);
  qcs::RPCRequestGetBuffers r3(id, getBuffers);

  msgpack::sbuffer sbuf3;
  msgpack::pack(sbuf3, r3);
  zmq::message_t msg3(sbuf3.size());
  memcpy(msg3.data(), sbuf3.data(), sbuf3.size());

  std::cout << "getbuffs zmq: " << msg3 << "\n";

  socket2.send(msg3);

  zmq::message_t reply3;
  socket2.recv(&reply3, 0);
  std::cout << reply3.data() << "\n";
  msgpack::unpacked unpackedData3;
  msgpack::unpack(unpackedData3, static_cast<const char *>(reply3.data()),
                  reply3.size());
  std::stringstream ss3;
  ss3 << unpackedData3.get();

    std::cout << "GETBUFS:\n" << ss3.str() << "\n";
  // now we have json results
  //auto getBuffsJson = json::parse(ss3.str());
  //std::cout << "HOWDY: " << getBuffsJson["q1"]["data"].dump() << "\n";

  ResultsDecoder().decode(buffer,ss3.str(), qbitIdxs, shots);
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

void
ResultsDecoder::decode(std::shared_ptr<AcceleratorBuffer> buffer,
                       const std::string jsonresults, std::set<int> qbitIdxs,
                       int shots) {

  auto j = std::regex_replace(jsonresults, std::regex("b'"), "\"");
  j = std::regex_replace(j, std::regex("'"), "\"");
  j.erase(std::remove(j.begin(), j.end(), '\\'), j.end());
std::cout << "parsing get bufs\n";
  using json = nlohmann::json;
  auto jj = json::parse(j);
std::cout << "parsed it\n";

  Eigen::MatrixXi bits = Eigen::MatrixXi::Zero(shots, qbitIdxs.size());
  int counter = 0;
  for (auto &i : qbitIdxs) {
    auto shape = jj["result"]["q" + std::to_string(i)]["shape"][0].get<int>();
    auto data =
        jj["result"]["q" + std::to_string(i)]["data"].get<std::string>();
    int kcounter = 0;
    for (int k = 0; k < shape * 3; k += 3) {
      std::string hex = data.substr(k, 3);
      xacc::trim(hex);
      int bit = hex == "x01" ? 1 : 0;
      bits(kcounter, counter) = bit;
      kcounter++;
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
