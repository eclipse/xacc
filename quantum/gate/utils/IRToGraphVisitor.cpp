
#include "IRToGraphVisitor.hpp"

#include "xacc_service.hpp"
#include <numeric>

namespace xacc {
namespace quantum {

void IRToGraphVisitor::addSingleQubitGate(GateInstruction &inst) {
  auto bit = inst.bits()[0];
  auto lastNode = qubitToLastNode[bit];
  auto lastBit = lastNode["bits"].as<std::vector<int>>()[0];

  id++;

  CircuitNode newNode{{"name", inst.name()}, {"id", id}, {"bits", inst.bits()}};

  graph->addVertex(newNode);
  graph->addEdge(lastNode["id"].as<int>(), newNode["id"].as<int>(), 1);

  qubitToLastNode[bit] = newNode;
}

void IRToGraphVisitor::addTwoQubitGate(GateInstruction &inst) {
  auto srcbit = inst.bits()[0];
  auto tgtbit = inst.bits()[1];

  auto lastsrcnodeid = qubitToLastNode[srcbit]["id"].as<int>();
  auto lasttgtnodeid = qubitToLastNode[tgtbit]["id"].as<int>();

  id++;
  CircuitNode newNode{{"name", inst.name()}, {"id", id}, {"bits", inst.bits()}};

  graph->addVertex(newNode);
  graph->addEdge(lastsrcnodeid, id, 1);
  graph->addEdge(lasttgtnodeid, id, 1);

  qubitToLastNode[srcbit] = newNode;
  qubitToLastNode[tgtbit] = newNode;
}

IRToGraphVisitor::IRToGraphVisitor(const int nQubits) {
  graph = xacc::getService<Graph>(
      "boost-digraph"); // std::make_shared<DirectedBoostGraph>(nQubits);
  std::vector<int> allQbitIds(nQubits);
  std::iota(std::begin(allQbitIds), std::end(allQbitIds), 0);
  CircuitNode initNode{
      {"name", "InitialState"}, {"id", 0}, {"bits", allQbitIds}};
  for (int i = 0; i < nQubits; i++) {
    qubitToLastNode.insert({i, initNode});
  }
  graph->addVertex(initNode);
}

std::shared_ptr<Graph> IRToGraphVisitor::getGraph() {
  CircuitNode finalNode{{"name", "FinalState"},
                        {"id", id + 1},
                        {"bits", graph->getVertexProperty(0, "bits")}};
  graph->addVertex(finalNode);

  for (auto &kv : qubitToLastNode) {
    graph->addEdge(kv.second["id"].as<int>(), finalNode["id"].as<int>(), 1.0);
  }

  return graph;
}
} // namespace quantum
} // namespace xacc
