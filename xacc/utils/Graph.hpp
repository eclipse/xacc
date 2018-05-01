/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_UTILS_IGRAPH_HPP_
#define XACC_UTILS_IGRAPH_HPP_

#include <boost/graph/adjacency_list.hpp>
#include <type_traits>
#include <boost/graph/exterior_property.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include <boost/graph/eccentricity.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <numeric>
#include "Utils.hpp"
#include <utility>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>


using namespace boost;

namespace xacc {

/**
 * Utility structs to help determine if
 * we have been given valid Vertices.
 */
template<typename T, typename = void>
struct is_valid_vertex: std::false_type {
};
template<typename T>
struct is_valid_vertex<T, decltype(std::declval<T>().properties, void())> : std::true_type {
};

/**
 * The base class of all QCI Vertices for the
 * QCI Common Graph class. All Vertices must keep
 * track of a set of properties, stored as a tuple.
 */
template<typename ... Properties>
class XACCVertex {
public:
	std::tuple<Properties...> properties;
	std::vector<std::string> propertyNames;
	XACCVertex() : propertyNames(sizeof...(Properties)) {}
	std::string getPropertyName(const int index) {
		return propertyNames[index];
	}
};

/**
 * For now, we only allow Edges with weight property.
 */
struct DefaultEdge {
	double weight = 0.0;
};
typedef boost::property<boost::edge_weight_t, double> EdgeWeightProperty;

enum GraphType {
	Undirected, Directed
};

// Overload stream operator for vector
template<class T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& v) {
	os << "[";
	for (std::size_t i = 0; i < v.size(); i++) {
		os << v[i] << (i == v.size() - 1 ? "" : " ");
	}
	os << "]";
	return os;
}

template<std::size_t> struct int_{};

template <class Tuple, size_t Pos>
std::ostream& print_tuple(std::ostream& out, const Tuple& t, int_<Pos> ) {
  out << std::get< std::tuple_size<Tuple>::value-Pos >(t) << ',';
  return print_tuple(out, t, int_<Pos-1>());
}

template <class Tuple>
std::ostream& print_tuple(std::ostream& out, const Tuple& t, int_<1> ) {
  return out << std::get<std::tuple_size<Tuple>::value-1>(t);
}

template <class... Args>
std::ostream& operator<<(std::ostream& out, const std::tuple<Args...>& t) {
  out << std::string(" [label=\"");
  print_tuple(out, t, int_<sizeof...(Args)>());
  return out << "\"]";
}

/**
 * The Graph class provides a generic data structure modeling
 * mathematical graph structures. It is templated on the vertex
 * type, allowing for graphs with a wide variety of graph nodes
 * (for example, in quantum computing - graph of tensors, graph of
 * Ising parameters, etc.)
 *
 * All provided Vertex types must be a subclass of the
 * QCIVertex in order to properly provide a tuple of vertex
 * properties.
 * s
 */
template<typename Vertex, GraphType type = Undirected>
class Graph {

	// Make sure we've been given a valid Vertex
	static_assert(is_valid_vertex<Vertex>::value, "XACC Graph can only take Vertices that are derived from XACCVertex, or have a tuple properties member.");

	using graph_type = typename std::conditional<(type == GraphType::Undirected), undirectedS, directedS>::type;

	// Setup some easy to use aliases
	using adj_list = adjacency_list<vecS, vecS, graph_type, Vertex, EdgeWeightProperty>;
	using BoostGraph = std::shared_ptr<adj_list>;
	using vertex_type = typename boost::graph_traits<adjacency_list<vecS, vecS, undirectedS, Vertex, EdgeWeightProperty>>::vertex_descriptor;
	using edge_type = typename boost::graph_traits<adjacency_list<vecS, vecS, undirectedS, Vertex, EdgeWeightProperty>>::edge_descriptor;

protected:

	/**
	 * This is a custom utility class for writing
	 * XACCVertices with user-defined properties.
	 */
	class XACCVertexPropertiesWriter {
	protected:
		adj_list graph;
	public:
		XACCVertexPropertiesWriter(adj_list& list) :
				graph(list) {
		}
		template<class BoostVertex>
		void operator()(std::ostream& out, const BoostVertex& v) const {
			auto node = vertex(v, graph);
			std::stringstream ss;
			ss << graph[node].properties;

			// Now we have a string of comma separated parameters
			std::string result;
			auto vertexString = ss.str();
			boost::trim(vertexString);
			std::vector<std::string> splitVec;
			boost::split(splitVec, vertexString, boost::is_any_of(","));

			int counter = 0;
			for (std::size_t i = 0; i < splitVec.size(); i++) {
				if (!graph[node].propertyNames[counter].empty()) {
					auto s = splitVec[i];
					if (i == 0) {
						s.insert(8, graph[node].propertyNames[counter] + "=");
					} else {
						s.insert(0, graph[node].propertyNames[counter] + "=");
					}
					counter++;
					result += s + ",";
				}
			}

			result = result.substr(0,result.size() - 1);
			if (result.substr(result.size() - 2, result.size()) != "\"]") {
				result += "\"]";
			}
			out << " " << result;
		}
	};

	/**
	 * The actual graph data structure we are
	 * delegating to.
	 */
	BoostGraph _graph;

public:

	/**
	 * The nullary constructor
	 */
	Graph() {
		_graph = std::make_shared<adj_list>();
	}

	/**
	 * The constructor, constructs a graph with
	 * specified number of vertices.
	 *
	 * @param numberOfVertices The number of vertices
	 */
	Graph(const int numberOfVertices) {
		_graph = std::make_shared<adj_list>(
				numberOfVertices);
	}

	/**
	 * Add an edge between the vertices with given provided
	 * indices and edge weight.
	 *
	 * @param srcIndex Index of the starting vertex
	 * @param tgtIndex Index of the ending vertex
	 * @param edgeWeight The edge weight
	 */
	void addEdge(const int srcIndex, const int tgtIndex, const double edgeWeight) {
		auto edgeBoolPair = add_edge(vertex(srcIndex, *_graph.get()),
				vertex(tgtIndex, *_graph.get()), *_graph.get());
		if (!edgeBoolPair.second) {
			XACCLogger::instance()->error("Failed to add an edge between " + std::to_string(srcIndex) + " and " + std::to_string(tgtIndex));
		}

		boost::put(boost::edge_weight_t(), *_graph.get(), edgeBoolPair.first, edgeWeight);
//		(*_graph.get())[edgeBoolPair.first].weight = edgeWeight;
//		std::cout << "EDGE WEIGHT SET\n";
	}

	/**
	 * Add an edge with default edge weight between the
	 * vertices at the provided indices.
	 *
	 * @param srcIndex Index of the starting vertex
	 * @param tgtIndex Index of the ending vertex
	 */
	void addEdge(const int srcIndex, const int tgtIndex) {
		add_edge(vertex(srcIndex, *_graph.get()),
				vertex(tgtIndex, *_graph.get()), *_graph.get());
	}

	void removeEdge(const int srcIndex, const int tgtIndex) {
		auto v = vertex(srcIndex, *_graph.get());
		auto u = vertex(tgtIndex, *_graph.get());
		remove_edge(v, u, *_graph.get());

	}
	/**
	 * Add a vertex to this Graph.
	 */
	void addVertex() {
		add_vertex(*_graph.get());
	}

	/**
	 * Add a vertex to this graph with the provided properties.
	 * s
	 * @param properties
	 */
	template<typename ... Properties>
	void addVertex(Properties ... properties) {
		auto v = add_vertex(*_graph.get());
		(*_graph.get())[v].properties = std::make_tuple(properties...);
	}

	void addVertex(Vertex& vertex) {
		auto v = add_vertex(*_graph.get());
		(*_graph.get())[v].properties = vertex.properties;
	}

	/**
	 * Set an existing vertices properties.
	 *
	 * @param index The index of the vertex
	 * @param properties The new properties for the vertex
	 */
	template<typename... Properties>
	void setVertexProperties(const int index, Properties... properties) {
		auto v = vertex(index, *_graph.get());
		(*_graph.get())[v].properties = std::make_tuple(properties...);
	}

	/**
	 * Set a specific vertex property for the vertex at given index.
	 *
	 * @param index The index of the vertex
	 * @param prop The property to set.
	 */
	template<const int PropertyIndex, typename DeducedPropertyType> //const int PropertyIndex>
	void setVertexProperty(const int index, DeducedPropertyType prop) {
//			decltype(std::get<PropertyIndex>(std::declval<Vertex>().properties)) prop) {
		auto v = vertex(index, *_graph.get());
		std::get<PropertyIndex>((*_graph.get())[v].properties) = prop;
		return;
	}

	Vertex& getVertex(const int index) {
		auto v = vertex(index, *_graph.get());
		return (*_graph.get())[v];
	}

	auto getVertexProperties(const int index) -> decltype((*_graph.get())[index].properties) {
		return (*_graph.get())[index].properties;
	}

	/**
	 * Return the vertex property of the vertex
	 * at the given index and at the provided
	 * valid vertex property template index.
	 *
	 * @param index The index of the vertex
	 * @return property The property value.
	 */
	template<const int PropertyIndex>
	decltype(std::get<PropertyIndex>(std::declval<Vertex>().properties))& getVertexProperty(
			const int index) {
		auto v = vertex(index, *_graph.get());
		return std::get<PropertyIndex>((*_graph.get())[v].properties);
	}

	/**
	 * Set the weight on the edge between the vertices at the
	 * provided indices.
	 *
	 * @param srcIndex The starting vertex index
	 * @param tgtIndex The ending vertex index
	 * @param weight The weight to set.
	 */
	void setEdgeWeight(const int srcIndex, const int tgtIndex, const double weight) {
		auto e = edge(vertex(srcIndex, *_graph.get()), vertex(tgtIndex, *_graph.get()), *_graph.get());
		boost::put(boost::edge_weight_t(), *_graph.get(), e.first, weight);
//		(*_graph.get())[e.first].weight = weight;
	}

	/**
	 * Return the edge weight at the edge between
	 * the provided vertices.
	 *
	 * @param srcIndex The starting vertex index
	 * @param tgtIndex The ending vertex index
	 * @return The edge weight
	 */
	double getEdgeWeight(const int srcIndex, const int tgtIndex) {
		auto e = edge(vertex(srcIndex, *_graph.get()), vertex(tgtIndex, *_graph.get()), *_graph.get());
		if (e.second) {
			auto weight = get(boost::edge_weight_t(), *_graph.get(), e.first);
			return weight;
		} else {
			return 0.0;
		}
	}

	/**
	 * Return true if there is an edge between the
	 * two vertices at the given vertex indices.
	 *
	 * @param srcIndex The starting vertex index
	 * @param tgtIndex The ending vertex index
	 * @return exists Boolean indicating if edge exists or not
	 */
	bool edgeExists(const int srcIndex, const int tgtIndex) {
		auto v1 = vertex(srcIndex, *_graph.get());
		auto v2 = vertex(tgtIndex, *_graph.get());
		auto p = edge(v1, v2, *_graph.get());
		return p.second;
	}

	/**
	 * Return the vertex degree at the given vertex index.
	 *
	 * @param index The index of the vertex
	 * @return degree The degree of the vertex
	 */
	int degree(const int index) {
		return boost::degree(vertex(index, *_graph.get()), *_graph.get());
	}

	/**
	 * Return the diameter of this Graph.
	 *
	 * @return diameter The graph diameter
	 */
	int diameter() {
		// Set some aliases we'll need
		using DistanceProperty = boost::exterior_vertex_property<adj_list, int>;
		using DistanceMatrix = typename DistanceProperty::matrix_type;
		using DistanceMatrixMap = typename DistanceProperty::matrix_map_type;
		using EccentricityProperty = boost::exterior_vertex_property<adj_list, int>;
		using EccentricityContainer = typename EccentricityProperty::container_type;
		using EccentricityMap = typename EccentricityProperty::map_type;

		// Construct the distance mapping
		DistanceMatrix distances(order());
		DistanceMatrixMap dm(distances, *_graph.get());
		constant_property_map<edge_type, int> wm(1);

		// Compute the shortest paths
		floyd_warshall_all_pairs_shortest_paths(*_graph.get(), dm, weight_map(wm));

		// Now get diameter information
		EccentricityContainer eccs(order());
		EccentricityMap em(eccs, *_graph.get());

		// Return the diameter
		return all_eccentricities(*_graph.get(), dm, em).second;
	}

	/**
	 * Return the number of edges.
	 * @return nEdges The number of edges.
	 */
	int size() {
		return num_edges(*_graph.get());
	}

	/**
	 * Return the number of vertices in this graph
	 *
	 * @return nVerts The number of vertices.
	 */
	int order() {
		return num_vertices(*_graph.get());
	}

	std::list<int> getNeighborList(const int index) {
		std::list<int> l;

		typedef typename boost::property_map<adj_list, boost::vertex_index_t>::type IndexMap;
		IndexMap indexMap = get(boost::vertex_index, *_graph.get());

		typedef typename boost::graph_traits<adj_list>::adjacency_iterator adjacency_iterator;

		std::pair<adjacency_iterator, adjacency_iterator> neighbors =
				boost::adjacent_vertices(vertex(index, *_graph.get()),
						*_graph.get());

		for (; neighbors.first != neighbors.second; ++neighbors.first) {
//			std::cout << indexMap[*neighbors.first] << " ";
			int neighborIdx = indexMap[*neighbors.first];
			l.push_back(neighborIdx);
		}

		return l;
	}
	/**
	 * Write this graph in a graphviz dot format to the
	 * provided ostream.
	 *
	 * @param stream
	 */
	void write(std::ostream& stream) {
		std::stringstream ss;
		XACCVertexPropertiesWriter writer(*_graph.get());
		boost::write_graphviz(ss, *_graph.get(), writer);
		auto str = ss.str();
		// Modify the style...
		str = str.insert(9, "\nnode [shape=box style=filled]");

		std::vector<std::string> splitVec;
		boost::split(splitVec, str, boost::is_any_of("\n"));
		splitVec.insert(splitVec.begin() + 2 + order(), "}\n");

//		std::cout << "HELLO:\n " << str << "\n";
		for (auto s : splitVec) {
			if (boost::contains(s, "--")) {
				// THis is an edge
				std::vector<std::string> splitEdge;
				boost::split(splitEdge, s, boost::is_any_of("--"));
				auto e1Str = splitEdge[0];
				auto e2Str = splitEdge[2].substr(0, splitEdge[2].size() - 2);
				auto e1Idx = std::stod(e1Str);
				auto e2Idx = std::stod(e2Str);
				auto news = e1Str + "--" + e2Str + " [label=\"weight=" + std::to_string(getEdgeWeight(e1Idx, e2Idx))+ "\"];";
				boost::replace_all(str, s, news);
			}
		}

		splitVec.clear();
		boost::split(splitVec, str, boost::is_any_of("\n"));
		std::stringstream combine;
		std::for_each(splitVec.begin(), splitVec.end(), [&](const std::string& elem) { combine << elem << "\n"; });
		stream << combine.str().substr(0, combine.str().size() - 2);
	}

	/**
	 * Read in a graphviz dot graph from the given input
	 * stream. This is left for subclasses.
	 *
	 * @param stream
	 */
	virtual void read(std::istream& stream) {
		stream.fail();
		XACCLogger::instance()->error("Reading must be implemented by subclasses.");
	}

	void computeShortestPath(int startIndex,
			std::vector<double>& distances, std::vector<int>& paths) {
		typename property_map<adj_list, edge_weight_t>::type weightmap = get(edge_weight,
				*_graph.get());
		std::vector<vertex_type> p(num_vertices(*_graph.get()));
		std::vector<int> d(num_vertices(*_graph.get()));
		vertex_type s = vertex(startIndex, *_graph.get());

		dijkstra_shortest_paths(*_graph.get(), s,
				predecessor_map(
						boost::make_iterator_property_map(p.begin(),
								get(boost::vertex_index, *_graph.get()))).distance_map(
						boost::make_iterator_property_map(d.begin(),
								get(boost::vertex_index, *_graph.get()))));

		for (int i = 0; i < p.size(); i++) {
			std::cout << p[i] << "\n";
		}

	}


};

}

#endif
