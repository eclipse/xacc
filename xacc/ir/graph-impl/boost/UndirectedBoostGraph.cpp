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
#include "UndirectedBoostGraph.hpp"

#include <numeric>
#include "Utils.hpp"
#include <utility>
#include <regex>

#include "xacc.hpp"
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/random/linear_congruential.hpp>

using namespace boost;

namespace xacc {

UndirectedBoostGraph::UndirectedBoostGraph() {
  _graph = std::make_shared<u_adj_list>();
}

UndirectedBoostGraph::UndirectedBoostGraph(const int numberOfVertices) {
  _graph = std::make_shared<u_adj_list>(numberOfVertices);
}

UndirectedBoostGraph::UndirectedBoostGraph(const UndirectedGraphType &in_graph)
    : _graph(in_graph) {}

void UndirectedBoostGraph::addEdge(const int srcIndex, const int tgtIndex,
                                   const double edgeWeight) {
  auto edgeBoolPair = add_edge(vertex(srcIndex, *_graph.get()),
                               vertex(tgtIndex, *_graph.get()), *_graph.get());
  if (!edgeBoolPair.second) {
    XACCLogger::instance()->error("Failed to add an edge between " +
                                  std::to_string(srcIndex) + " and " +
                                  std::to_string(tgtIndex));
  }

  boost::put(boost::edge_weight_t(), *_graph.get(), edgeBoolPair.first,
             edgeWeight);
}

void UndirectedBoostGraph::addEdge(const int srcIndex,
                                 const int tgtIndex) {
  add_edge(vertex(srcIndex, *_graph.get()), vertex(tgtIndex, *_graph.get()),
           *_graph.get());
}

void UndirectedBoostGraph::removeEdge(const int srcIndex,
                                    const int tgtIndex) {
  auto v = vertex(srcIndex, *_graph.get());
  auto u = vertex(tgtIndex, *_graph.get());
  remove_edge(v, u, *_graph.get());
}

void UndirectedBoostGraph::addVertex() { add_vertex(*_graph.get()); }

void UndirectedBoostGraph::addVertex(
    HeterogeneousMap &&properties) {
  auto v = add_vertex(*_graph.get());
  (*_graph.get())[v].properties = properties;
}

void UndirectedBoostGraph::addVertex(
    HeterogeneousMap &properties) {
  auto v = add_vertex(*_graph.get());
  (*_graph.get())[v].properties = properties;
}

void UndirectedBoostGraph::setVertexProperties(
    const int index,
    HeterogeneousMap &&properties) {
  auto v = vertex(index, *_graph.get());
  (*_graph.get())[v].properties = properties;
}
void UndirectedBoostGraph::setVertexProperties(
    const int index, HeterogeneousMap &properties) {
  auto v = vertex(index, *_graph.get());
  (*_graph.get())[v].properties = properties;
}

// void UndirectedBoostGraph::setVertexProperty(const int index,
//                                            const std::string prop,
//                                            InstructionParameter &&p) {
//   auto v = vertex(index, *_graph.get());
//   if ((*_graph.get())[v].properties.count(prop)) {
//     (*_graph.get())[v].properties[prop] = p;
//   } else {
//     (*_graph.get())[v].properties.insert({prop,p});
//   }
//   return;
// }
// void UndirectedBoostGraph::setVertexProperty(const int index,
//                                            const std::string prop,
//                                            InstructionParameter &p) {
//   auto v = vertex(index, *_graph.get());
//   if ((*_graph.get())[v].properties.count(prop)) {
//     (*_graph.get())[v].properties[prop] = p;
//   } else {
//     (*_graph.get())[v].properties.insert({prop,p});
//   }
//   return;
// }
HeterogeneousMap&
UndirectedBoostGraph::getVertexProperties(const int index) {
  return (*_graph.get())[index].properties;
}

// InstructionParameter &
// UndirectedBoostGraph::getVertexProperty(const int index,
//                                       const std::string property) {
//   auto v = vertex(index, *_graph.get());
//   if ((*_graph.get())[v].properties.count(property)) {
//     return (*_graph.get())[v].properties[property];
//   } else {
//     xacc::error("Invalid Graph vertex property name: " + property);
//   }
// }

void UndirectedBoostGraph::setEdgeWeight(const int srcIndex, const int tgtIndex,
                                       const double weight) {
  auto e = edge(vertex(srcIndex, *_graph.get()),
                vertex(tgtIndex, *_graph.get()), *_graph.get());
  boost::put(boost::edge_weight_t(), *_graph.get(), e.first, weight);
}

double UndirectedBoostGraph::getEdgeWeight(const int srcIndex,
                                         const int tgtIndex) {
  auto e = edge(vertex(srcIndex, *_graph.get()),
                vertex(tgtIndex, *_graph.get()), *_graph.get());
  if (e.second) {
    auto weight = get(boost::edge_weight_t(), *_graph.get(), e.first);
    return weight;
  } else {
    return 0.0;
  }
}

bool UndirectedBoostGraph::edgeExists(const int srcIndex,
                                    const int tgtIndex) {
  auto v1 = vertex(srcIndex, *_graph.get());
  auto v2 = vertex(tgtIndex, *_graph.get());
  auto p = edge(v1, v2, *_graph.get());
  return p.second;
}

int UndirectedBoostGraph::degree(const int index) {
  return getNeighborList(index).size();//boost::degree(vertex(index, *_graph.get()), *_graph.get());
}

int UndirectedBoostGraph::diameter() {
  // Set some aliases we'll need
  using DistanceProperty = boost::exterior_vertex_property<u_adj_list, int>;
  using DistanceMatrix = typename DistanceProperty::matrix_type;
  using DistanceMatrixMap = typename DistanceProperty::matrix_map_type;
  using EccentricityProperty = boost::exterior_vertex_property<u_adj_list, int>;
  using EccentricityContainer = typename EccentricityProperty::container_type;
  using EccentricityMap = typename EccentricityProperty::map_type;

  // Construct the distance mapping
  DistanceMatrix distances(order());
  DistanceMatrixMap dm(distances, *_graph.get());
  constant_property_map<u_edge_type, int> wm(1);

  // Compute the shortest paths
  floyd_warshall_all_pairs_shortest_paths(*_graph.get(), dm, weight_map(wm));

  // Now get diameter information
  EccentricityContainer eccs(order());
  EccentricityMap em(eccs, *_graph.get());

  // Return the diameter
  return all_eccentricities(*_graph.get(), dm, em).second;
}

int UndirectedBoostGraph::size() { return num_edges(*_graph.get()); }

int UndirectedBoostGraph::order() { return num_vertices(*_graph.get()); }

std::vector<int> UndirectedBoostGraph::getNeighborList(const int index) {
  std::vector<int> l;

  typedef typename boost::property_map<u_adj_list, boost::vertex_index_t>::type
      IndexMap;
  IndexMap indexMap = get(boost::vertex_index, *_graph.get());

  typedef typename boost::graph_traits<u_adj_list>::adjacency_iterator
      adjacency_iterator;

  std::pair<adjacency_iterator, adjacency_iterator> neighbors =
      boost::adjacent_vertices(vertex(index, *_graph.get()), *_graph.get());

  for (; neighbors.first != neighbors.second; ++neighbors.first) {
    int neighborIdx = indexMap[*neighbors.first];
    l.push_back(neighborIdx);
  }

  return l;
}

void UndirectedBoostGraph::write(std::ostream &stream) { std::stringstream ss;
  UndirectedVertexPropertiesWriter writer(*_graph.get());
  boost::write_graphviz(ss, *_graph.get(), writer);
  auto str = ss.str();
  // Modify the style...
  str = str.insert(11,
                   "\nnode [shape=box style=filled]");

  std::vector<std::string> splitVec = xacc::split(str, '\n');
  splitVec.insert(splitVec.begin() + 2 + order(), "}\n");

  for (auto s : splitVec) {
    if (s.find("--") != std::string::npos) {
      // THis is an edge
      // std::cout << "S: " << s << "\n";
      s = std::regex_replace(s, std::regex("--"), ";");
              // std::cout << "Safter: " << s << "\n";

      std::vector<std::string> splitEdge = xacc::split(s , ';');
      // for (auto& i : splitEdge) {std::cout << "splitedge; " << i << "\n";}
      auto e1Str = splitEdge[0];
      auto e2Str = splitEdge[1];//.substr(0, splitEdge[2].size() - 2);
      auto e1Idx = std::stoi(e1Str);
      auto e2Idx = std::stoi(e2Str);
      auto news = e1Str + "--" + e2Str + "[label=\"weight=" +
                  std::to_string(getEdgeWeight(e1Idx, e2Idx)) + "\"]";
      s = std::regex_replace(s, std::regex(";"), "--");
      s = s.substr(0,s.length()-3) + " ";
      // std::cout << "snow : " << s << "\n";
      str = std::regex_replace(str, std::regex(s),
      news);//boost::replace_all(str, s, news);
      // std::cout << "STRING: " << str << "\n";
    }
  }

  stream << str;
}

/**
 * Read in a graphviz dot graph from the given input
 * stream. This is left for subclasses.
 *
 * @param stream
 */
void UndirectedBoostGraph::read(std::istream &stream) {
  stream.fail();
  XACCLogger::instance()->error("Reading must be implemented by subclasses.");
}

void UndirectedBoostGraph::computeShortestPath(int startIndex,
                                             std::vector<double> &distances,
                                             std::vector<int> &paths) {
  typename property_map<u_adj_list, edge_weight_t>::type weightmap =
      get(edge_weight, *_graph.get());
  std::vector<u_vertex_type> p(num_vertices(*_graph.get()));
  std::vector<int> d(num_vertices(*_graph.get()));
  u_vertex_type s = vertex(startIndex, *_graph.get());

  dijkstra_shortest_paths(
      *_graph.get(), s,
      predecessor_map(boost::make_iterator_property_map(
                          p.begin(), get(boost::vertex_index, *_graph.get())))
          .distance_map(boost::make_iterator_property_map(
              d.begin(), get(boost::vertex_index, *_graph.get()))));

  for (auto &di : d)
    distances.push_back(di);
  for (auto &pi : p)
    paths.push_back(pi);
}

const int UndirectedBoostGraph::depth() {
    XACCLogger::instance()->error("Cannot compute depth on unUndirected graph.");
    return 0;
}

std::shared_ptr<Graph>
UndirectedBoostGraph::gen_random_graph(int nbNodes, double edgeProb) const {
  // Erdos-Renyi graph generator
  using ERGen = boost::erdos_renyi_iterator<boost::minstd_rand, u_adj_list>;
  boost::minstd_rand gen;
  auto randGraph = std::make_shared<u_adj_list>(ERGen(gen, nbNodes, edgeProb),
                                                ERGen(), nbNodes);
  return std::make_shared<UndirectedBoostGraph>(randGraph);
}
} // namespace xacc
