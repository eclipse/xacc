
#include "IRToGraphVisitor.hpp"

#include "xacc_service.hpp"
#include <numeric>

namespace xacc {
namespace quantum {

void IRToGraphVisitor::addSingleQubitGate(Gate &inst) {
  auto bit = inst.bits()[0];
  auto lastNode = qubitToLastNode[bit];
  auto lastBit = lastNode.get<std::vector<std::size_t>>("bits")[0];

  id++;

  CircuitNode newNode{std::make_pair("name", inst.name()),
                      std::make_pair("id", id),
                      std::make_pair("bits", inst.bits())};

  graph->addVertex(newNode);
  graph->addEdge(lastNode.get<int>("id"), newNode.get<int>("id"), 1);

  qubitToLastNode[bit] = newNode;
}

void IRToGraphVisitor::addTwoQubitGate(Gate &inst) {
  auto srcbit = inst.bits()[0];
  auto tgtbit = inst.bits()[1];

  auto lastsrcnodeid = qubitToLastNode[srcbit].get<int>("id");
  auto lasttgtnodeid = qubitToLastNode[tgtbit].get<int>("id");

  id++;
  CircuitNode newNode{std::make_pair("name", inst.name()),
                      std::make_pair("id", id),
                      std::make_pair("bits", inst.bits())};
  graph->addVertex(newNode);
  graph->addEdge(lastsrcnodeid, id, 1);
  graph->addEdge(lasttgtnodeid, id, 1);

  qubitToLastNode[srcbit] = newNode;
  qubitToLastNode[tgtbit] = newNode;
}

IRToGraphVisitor::IRToGraphVisitor(const int nQubits) {
  graph = xacc::getService<Graph>(
      "boost-digraph"); // std::make_shared<DirectedBoostGraph>(nQubits);
  std::vector<std::size_t> allQbitIds(nQubits);
  std::iota(std::begin(allQbitIds), std::end(allQbitIds), 0);
   CircuitNode initNode{std::make_pair("name", std::string("InitialState")),
                      std::make_pair("id", 0),
                      std::make_pair("bits", allQbitIds)};
  for (int i = 0; i < nQubits; i++) {
    qubitToLastNode[i] = initNode;
  }
  graph->addVertex(initNode);
}

std::shared_ptr<Graph> IRToGraphVisitor::getGraph() {
   CircuitNode finalNode{std::make_pair("name", std::string("FinalState")),
                      std::make_pair("id", id+1),
                      std::make_pair("bits", graph->getVertexProperties(0).get<std::vector<std::size_t>>("bits"))};
  graph->addVertex(finalNode);

  for (auto &kv : qubitToLastNode) {
    graph->addEdge(kv.second.get<int>("id"), finalNode.get<int>("id"), 1.0);
  }

  return graph;
}
} // namespace quantum
} // namespace xacc
