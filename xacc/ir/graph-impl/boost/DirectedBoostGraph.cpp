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
#include "DirectedBoostGraph.hpp"

#include <numeric>
#include "Utils.hpp"
#include <utility>
#include <regex>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/random/linear_congruential.hpp>

using namespace boost;

namespace xacc {

DirectedBoostGraph::DirectedBoostGraph() {
  _graph = std::make_shared<d_adj_list>();
}

DirectedBoostGraph::DirectedBoostGraph(const int numberOfVertices) {
  _graph = std::make_shared<d_adj_list>(numberOfVertices);
}

DirectedBoostGraph::DirectedBoostGraph(const DirectedGraphType &in_graph)
    : _graph(in_graph) {}

void DirectedBoostGraph::addEdge(const int srcIndex, const int tgtIndex,
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

void DirectedBoostGraph::addEdge(const int srcIndex,
                                 const int tgtIndex) {
  add_edge(vertex(srcIndex, *_graph.get()), vertex(tgtIndex, *_graph.get()),
           *_graph.get());
}

void DirectedBoostGraph::removeEdge(const int srcIndex,
                                    const int tgtIndex) {
  auto v = vertex(srcIndex, *_graph.get());
  auto u = vertex(tgtIndex, *_graph.get());
  remove_edge(v, u, *_graph.get());
}

void DirectedBoostGraph::addVertex() { add_vertex(*_graph.get()); }

void DirectedBoostGraph::addVertex(
    HeterogeneousMap &&properties) {
  auto v = add_vertex(*_graph.get());
  (*_graph.get())[v].properties = properties;
}

void DirectedBoostGraph::addVertex(
    HeterogeneousMap &properties) {
  auto v = add_vertex(*_graph.get());
  (*_graph.get())[v].properties = properties;
}

void DirectedBoostGraph::setVertexProperties(
    const int index,
    HeterogeneousMap &&properties) {
  auto v = vertex(index, *_graph.get());
  (*_graph.get())[v].properties = properties;
}
void DirectedBoostGraph::setVertexProperties(
    const int index, HeterogeneousMap &properties) {
  auto v = vertex(index, *_graph.get());
  (*_graph.get())[v].properties = properties;
}

// void DirectedBoostGraph::setVertexProperty(const int index,
//                                            const std::string prop,
//                                            InstructionParameter &&p) {
//   auto v = vertex(index, *_graph.get());
//   if ((*_graph.get())[v].properties.count(prop)) {
//     (*_graph.get())[v].properties[prop] = p;
//   } else {
//     (*_graph.get())[v].properties.insert({prop,p});
//   }
// }
// void DirectedBoostGraph::setVertexProperty(const int index,
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
DirectedBoostGraph::getVertexProperties(const int index) {
  return (*_graph.get())[index].properties;
}

// InstructionParameter &
// DirectedBoostGraph::getVertexProperty(const int index,
//                                       const std::string property) {
//   auto v = vertex(index, *_graph.get());
//   if ((*_graph.get())[v].properties.count(property)) {
//     return (*_graph.get())[v].properties[property];
//   } else {
//       XACCLogger::instance()->error("Invalid Graph vertex property name: " + property);
//   }
// }

void DirectedBoostGraph::setEdgeWeight(const int srcIndex, const int tgtIndex,
                                       const double weight) {
  auto e = edge(vertex(srcIndex, *_graph.get()),
                vertex(tgtIndex, *_graph.get()), *_graph.get());
  boost::put(boost::edge_weight_t(), *_graph.get(), e.first, weight);
}

double DirectedBoostGraph::getEdgeWeight(const int srcIndex,
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

bool DirectedBoostGraph::edgeExists(const int srcIndex,
                                    const int tgtIndex) {
  auto v1 = vertex(srcIndex, *_graph.get());
  auto v2 = vertex(tgtIndex, *_graph.get());
  auto p = edge(v1, v2, *_graph.get());
  return p.second;
}

int DirectedBoostGraph::degree(const int index) {
  return getNeighborList(index).size();//boost::degree(vertex(index, *_graph.get()), *_graph.get());
}

int DirectedBoostGraph::diameter() {
  // Set some aliases we'll need
  using DistanceProperty = boost::exterior_vertex_property<d_adj_list, int>;
  using DistanceMatrix = typename DistanceProperty::matrix_type;
  using DistanceMatrixMap = typename DistanceProperty::matrix_map_type;
  using EccentricityProperty = boost::exterior_vertex_property<d_adj_list, int>;
  using EccentricityContainer = typename EccentricityProperty::container_type;
  using EccentricityMap = typename EccentricityProperty::map_type;

  // Construct the distance mapping
  DistanceMatrix distances(order());
  DistanceMatrixMap dm(distances, *_graph.get());
  constant_property_map<d_edge_type, int> wm(1);

  // Compute the shortest paths
  floyd_warshall_all_pairs_shortest_paths(*_graph.get(), dm, weight_map(wm));

  // Now get diameter information
  EccentricityContainer eccs(order());
  EccentricityMap em(eccs, *_graph.get());

  // Return the diameter
  return all_eccentricities(*_graph.get(), dm, em).second;
}

int DirectedBoostGraph::size() { return num_edges(*_graph.get()); }

int DirectedBoostGraph::order() { return num_vertices(*_graph.get()); }

std::vector<int> DirectedBoostGraph::getNeighborList(const int index) {
  std::vector<int> l;

  typedef typename boost::property_map<d_adj_list, boost::vertex_index_t>::type
      IndexMap;
  IndexMap indexMap = get(boost::vertex_index, *_graph.get());

  typedef typename boost::graph_traits<d_adj_list>::adjacency_iterator
      adjacency_iterator;

  std::pair<adjacency_iterator, adjacency_iterator> neighbors =
      boost::adjacent_vertices(vertex(index, *_graph.get()), *_graph.get());

  for (; neighbors.first != neighbors.second; ++neighbors.first) {
    int neighborIdx = indexMap[*neighbors.first];
    l.push_back(neighborIdx);
  }

  return l;
}

void DirectedBoostGraph::write(std::ostream &stream) {
  std::stringstream ss;
  DirectedVertexPropertiesWriter writer(*_graph.get());
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
      str = std::regex_replace(str, std::regex(s),news);//boost::replace_all(str, s, news);
      // std::cout << "STRING: " << str << "\n";
    }
  }

//    std::cout << "HOWDY\n" << str << "\nHOWDY2\n";
  stream << str;
//   splitVec.clear();
//   splitVec = xacc::split(str, '\n');
//   std::stringstream combine;
//   std::for_each(splitVec.begin(), splitVec.end(),
//                 [&](const std::string &elem) { combine << elem << "\n"; });
//   stream << combine.str().substr(0, combine.str().size() - 2) << "}";
}

/**
 * Read in a graphviz dot graph from the given input
 * stream. This is left for subclasses.
 *
 * @param stream
 */
void DirectedBoostGraph::read(std::istream &stream) {
  stream.fail();
  XACCLogger::instance()->error("Reading must be implemented by subclasses.");
}

void DirectedBoostGraph::computeShortestPath(int startIndex,
                                             std::vector<double> &distances,
                                             std::vector<int> &paths) {
  typename property_map<d_adj_list, edge_weight_t>::type weightmap =
      get(edge_weight, *_graph.get());
  std::vector<d_vertex_type> p(num_vertices(*_graph.get()));
  std::vector<int> d(num_vertices(*_graph.get()));
  d_vertex_type s = vertex(startIndex, *_graph.get());

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

const int DirectedBoostGraph::depth() {
//   if (type == Undirected) {
//     XACCLogger::instance()->error("Cannot compute depth on undirected graph.");
//   }

  // Make a copy of this graph with negative edge weights
  d_adj_list g(order());
  for (int i = 0; i < order(); i++) {
    for (int j = 0; j < order(); j++) {
      if (edgeExists(i, j)) {
        auto edgeBoolPair = add_edge(vertex(i, g), vertex(j, g), g);
        if (!edgeBoolPair.second) {
          XACCLogger::instance()->error("Failed to add an edge between " +
                                        std::to_string(i) + " and " +
                                        std::to_string(j));
        }

        boost::put(boost::edge_weight_t(), g, edgeBoolPair.first,
                   -1.0 * getEdgeWeight(i, j));
      }
    }
  }

  std::vector<d_vertex_type> p(order());
  std::vector<int> d(order());

  boost::dag_shortest_paths(
      g, vertex(0, g),
      predecessor_map(boost::make_iterator_property_map(
                          p.begin(), get(boost::vertex_index, g)))
          .distance_map(boost::make_iterator_property_map(
              d.begin(), get(boost::vertex_index, g))));


  return -1 * (*std::min_element(d.begin(), d.end())) - 1;
}

std::shared_ptr<Graph>
DirectedBoostGraph::gen_random_graph(int nbNodes, double edgeProb) const {
  // Erdos-Renyi graph generator
  using ERGen = boost::erdos_renyi_iterator<boost::minstd_rand, d_adj_list>;
  boost::minstd_rand gen;
  auto randGraph = std::make_shared<d_adj_list>(ERGen(gen, nbNodes, edgeProb),
                                                ERGen(), nbNodes);
  return std::make_shared<DirectedBoostGraph>(randGraph);
}
} // namespace xacc
