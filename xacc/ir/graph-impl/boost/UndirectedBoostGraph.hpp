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
#ifndef XACC_QUANTUM_UTILS_UNDIRECTEDBOOSTGRAPH_HPP_
#define XACC_QUANTUM_UTILS_UNDIRECTEDBOOSTGRAPH_HPP_

#include "Graph.hpp"
#include <memory>
#include <sstream>
#include "Utils.hpp"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/exterior_property.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/eccentricity.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/dag_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>

using namespace boost;
namespace xacc {


class UndirectedBoostVertex {
public:
  HeterogeneousMap properties;
};
using u_adj_list = adjacency_list<vecS, vecS, undirectedS, UndirectedBoostVertex,
                                boost::property<boost::edge_weight_t, double>>;

using UndirectedGraphType = std::shared_ptr<u_adj_list>;

using u_vertex_type = typename boost::graph_traits<adjacency_list<
    vecS, vecS, undirectedS, UndirectedBoostVertex,
    boost::property<boost::edge_weight_t, double>>>::vertex_descriptor;

using u_edge_type = typename boost::graph_traits<adjacency_list<
    vecS, vecS, undirectedS, UndirectedBoostVertex,
    boost::property<boost::edge_weight_t, double>>>::edge_descriptor;


class UndirectedBoostGraph : public Graph {

protected:
  UndirectedGraphType _graph;

class UndirectedVertexPropertiesWriter {
  protected:
    u_adj_list graph;

  class print_visitor : public xacc::visitor_base<int,double,float,std::string,std::vector<double>> {
  protected:
   std::stringstream& ss;
  public:
    print_visitor(std::stringstream& s) : ss(s) {}
    template<typename T>
    void operator()(const std::string& key, const T& t) {
        ss << key << "=" << t << ";";
    }
  };
  public:
    UndirectedVertexPropertiesWriter(u_adj_list &list) : graph(list) {}
    void operator()(std::ostream &out, const u_vertex_type &v) const {
      auto node = vertex(v, graph);
      std::stringstream ss;
      ss << " [label=\"";
      int counter = 0;
      print_visitor vis(ss);
      graph[node].properties.visit(vis);

      out << ss.str().substr(0,ss.str().length()-1) + "\"]";
    }
};
// class UndirectedVertexPropertiesWriter {
//   protected:
//     u_adj_list graph;

//   public:
//     UndirectedVertexPropertiesWriter(u_adj_list &list) : graph(list) {}
//     void operator()(std::ostream &out, const u_vertex_type &v) const {
//       auto node = vertex(v, graph);
//       std::stringstream ss;
//       ss << " [label=\"";
//       int counter = 0;
//       for (auto& kv : graph[node].properties) {
//           if (counter == graph[node].properties.size()-1) {
//            ss << kv.first <<"=" << kv.second.toString();
//           } else {
//               ss << kv.first << "=" << kv.second.toString() << ";";
//           }
//           counter++;
//       }
//       ss << "\"]";
//       out << ss.str();
//     }
// };
public:
  UndirectedBoostGraph();
  UndirectedBoostGraph(const int numberOfVertices);
  UndirectedBoostGraph(const UndirectedGraphType &in_graph);
  void addEdge(const int srcIndex, const int tgtIndex,
               const double edgeWeight) override;
  void addEdge(const int srcIndex, const int tgtIndex) override;
  void removeEdge(const int srcIndex, const int tgtIndex) override;

  void addVertex() override;
  void
  addVertex(HeterogeneousMap &&properties) override;
  void
  addVertex(HeterogeneousMap &properties) override;

  void setVertexProperties(
      const int index,
      HeterogeneousMap &&properties) override;
  void
  setVertexProperties(const int index,
                      HeterogeneousMap &properties) override;
//   void setVertexProperty(const int index, const std::string prop,
//                          InstructionParameter &&p) override;
//   void setVertexProperty(const int index, const std::string prop,
//                          InstructionParameter &p) override;

  HeterogeneousMap&
  getVertexProperties(const int index) override;
//   virtual InstructionParameter &
//   getVertexProperty(const int index, const std::string property) override;

  void setEdgeWeight(const int srcIndex, const int tgtIndex,
                     const double weight) override;
  double getEdgeWeight(const int srcIndex, const int tgtIndex) override;
  bool edgeExists(const int srcIndex, const int tgtIndex) override;

  int degree(const int index) override;
  int diameter() override;
  int size() override;
  int order() override;

  std::vector<int> getNeighborList(const int index) override;

  void write(std::ostream &stream) override;
  void read(std::istream &stream) override;

  void computeShortestPath(int startIndex, std::vector<double> &distances,
                           std::vector<int> &paths) override;
  const int depth() override;

  const std::string name() const override { return "boost-ugraph"; }
  const std::string description() const override { return ""; }
  std::shared_ptr<Graph> clone() override {
      return std::make_shared<UndirectedBoostGraph>();
  }
  std::shared_ptr<Graph> gen_random_graph(int nbNodes,
                                          double edgeProb) const override;
};

} // namespace xacc

#endif
