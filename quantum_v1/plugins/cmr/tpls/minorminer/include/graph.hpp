#pragma once

#include <random>
#include <set>
#include <vector>

#include "util.hpp"

namespace graph {

//! Represents an undirected graph as a list of edges.
//!
//! Provides methods to extract those edges into neighbor lists (with options
//! to relabel and produce directed graphs).
//!
//! As an input to the library this may be a disconnected graph,
//! but when returned from components it is a connected sub graph.
class input_graph {
  private:
    // In
    std::vector<int> edges_aside;
    std::vector<int> edges_bside;
    int _num_nodes;

    //! this method converts a std::vector of sets into a std::vector of sets, ensuring
    //! that element i is not contained in nbrs[i].  this method is called by
    //! methods which produce neighbor sets (killing parallel/overrepresented
    //! edges), in order to kill self-loops and also store each neighborhood
    //! in a contiguous memory segment.
    void _to_vectorhoods(std::vector<std::set<int> >& _nbrs, std::vector<std::vector<int> >& nbrs) const {
        nbrs.clear();
        for (int i = 0; i < _num_nodes; i++) {
 	    std::set<int>& nbrset = _nbrs[i];
            nbrset.erase(i);
            nbrs.emplace_back(std::begin(nbrset), std::end(nbrset));
        }
    }

  public:
    //! Constructs an empty graph.
    input_graph() : edges_aside(), edges_bside(), _num_nodes(0) {}
    //! Constructs a graph from the provided edges.
    //! The ends of edge ii are aside[ii] and bside[ii].
    //! @param n_v Number of nodes in the graph.
    //! @param aside List of nodes describing edges.
    //! @param bside List of nodes describing edges.
    input_graph(int n_v, const std::vector<int>& aside, const std::vector<int>& bside)
            : edges_aside(aside), edges_bside(bside), _num_nodes(n_v) {
        minorminer_assert(aside.size() == bside.size());
    }

    //! Remove all edges and nodes from a graph.
    void clear() {
        edges_aside.clear();
        edges_bside.clear();
        _num_nodes = 0;
    }

    //! Return the nodes on either end of edge `i`
    int a(const int i) const { return edges_aside[i]; }
    //! Return the nodes on either end of edge `i`
    int b(const int i) const { return edges_bside[i]; }

    //! Return the size of the graph in nodes
    int num_nodes() const { return _num_nodes; }
    //! Return the size of the graph in edges
    int num_edges() const { return edges_aside.size(); }

    //! Add an edge to the graph
    void push_back(int ai, int bi) {
        edges_aside.push_back(ai);
        edges_bside.push_back(bi);
        _num_nodes = std::max(_num_nodes, std::max(ai, bi) + 1);
    }

    //! produce the node->nodelist mapping for our graph, where certain nodes are
    //! marked as sources (no incoming edges)
    void get_neighbors_sources(std::vector<std::vector<int> >& nbrs, const std::vector<int>& sources) const {
        std::vector<std::set<int> > _nbrs(_num_nodes);
        for (int i = num_edges(); i--;) {
            int ai = a(i), bi = b(i);
            if (!sources[bi]) _nbrs[ai].insert(bi);
            if (!sources[ai]) _nbrs[bi].insert(ai);
        }
        _to_vectorhoods(_nbrs, nbrs);
    }

    //! produce the node->nodelist mapping for our graph, where certain nodes are
    //! marked as sinks (no outgoing edges)
    void get_neighbors_sinks(std::vector<std::vector<int> >& nbrs, const std::vector<int>& sinks) const {
        std::vector<std::set<int> > _nbrs(_num_nodes);
        for (int i = num_edges(); i--;) {
            int ai = a(i), bi = b(i);
            if (!sinks[ai]) _nbrs[ai].insert(bi);
            if (!sinks[bi]) _nbrs[bi].insert(ai);
        }
        _to_vectorhoods(_nbrs, nbrs);
    }

    //! produce the node->nodelist mapping for our graph, where certain nodes are
    //! marked as sinks (no outgoing edges), relabeling all nodes along the way
    void get_neighbors_sinks_relabel(std::vector<std::vector<int> >& nbrs, const std::vector<int>& sinks,
                                     const std::vector<int>& relabel) const {
        std::vector<std::set<int> > _nbrs(_num_nodes);
        for (int i = num_edges(); i--;) {
            int ai = a(i), bi = b(i);
            int rai = relabel[ai], rbi = relabel[bi];
            if (!sinks[ai]) _nbrs[rai].insert(rbi);
            if (!sinks[bi]) _nbrs[rbi].insert(rai);
        }
        _to_vectorhoods(_nbrs, nbrs);
    }

    //! produce the node->nodelist mapping for our graph, relabeling all nodes along the way
    void get_neighbors_relabel(std::vector<std::vector<int> >& nbrs, const std::vector<int>& relabel) const {
        std::vector<std::set<int> > _nbrs(_num_nodes);
        for (int i = num_edges(); i--;) {
            int ai = relabel[a(i)], bi = relabel[b(i)];
            _nbrs[ai].insert(bi);
            _nbrs[bi].insert(ai);
        }
        _to_vectorhoods(_nbrs, nbrs);
    }

    //! produce the node->nodelist mapping for our graph
    void get_neighbors(std::vector<std::vector<int> >& nbrs) const {
        std::vector<std::set<int> > _nbrs(_num_nodes);
        for (int i = num_edges(); i--;) {
            int ai = a(i), bi = b(i);
            _nbrs[ai].insert(bi);
            _nbrs[bi].insert(ai);
        }
        _to_vectorhoods(_nbrs, nbrs);
    }
};

//! Represents a graph as a series of connected components.
//!
//! The input graph may consist of many components, they will be separated
//! in the construction.
class components {
  public:
    template <class rng_t>
    components(const input_graph& g, rng_t& rng, const std::vector<int>& reserve)
            : index(g.num_nodes(), 0), label(g.num_nodes(), 0), component(g.num_nodes()), component_g() {
        /*
        STEP 1: perform union/find to compute components.

        During this stage, we use this.index and this.label, respectively,
        to store the parent and rank data for union/find operations.
        */
        std::vector<int>& parent = index;
        for (int x = g.num_nodes(); x--;) {
            parent[x] = x;
            if (reserve[x]) {
                int x0 = x;
                for (; x--;) parent[x] = reserve[x] ? x0 : x;
                break;
            }
        }
        for (int i = g.num_edges(); i--;) {
            __init_union(g.a(i), g.b(i));
        }

        for (int x = g.num_nodes(); x--;) component[__init_find(x)].push_back(x);

        sort(std::begin(component), std::end(component),
             [](const std::vector<int>& a, const std::vector<int>& b) { return a.size() > b.size(); });

        /*
        STEP 2: distribute edges to their components

        Now, all component information is contained in this.component, so
        we're free to overwrite the data left in this.label and this.index.
        The labels associated with component[c] are the numbers 0 through
        component[c].size()-1.  Shuffling component[c] randomizes the node
        labels within that component.
        */
        for (int c = 0; c < g.num_nodes(); c++) {
            std::vector<int>& comp = component[c];
            auto back = std::end(comp);
            for (auto front = std::begin(comp); front < back; front++)
                if (reserve[*front]) iter_swap(front, --back);
            if (comp.size() > 1) {
                shuffle(std::begin(comp), back, rng);
                for (int j = comp.size(); j--;) {
                    label[comp[j]] = j;
                    index[comp[j]] = c;
                }
                component_g.push_back(input_graph());
                _num_reserved.push_back(std::end(comp) - back);
            } else {
                component.resize(c);
                break;
            }
        }
        for (int i = g.num_edges(); i--;) {
            int a = g.a(i);
            int b = g.b(i);
            component_g[index[a]].push_back(label[a], label[b]);
        }
    }

    //! Get the set of nodes in a component
    const std::vector<int>& nodes(int c) const { return component[c]; }

    //! Get the number of connected components in the graph
    int size() const { return component_g.size(); }

    //! returns the number of reserved nodes in a component
    int num_reserved(int c) const { return _num_reserved[c]; }

    //! Get the size (in nodes) of a component
    int size(int c) const { return component_g[c].num_nodes(); }

    //! Get a mutable reference to the graph object of a component
    input_graph& component_graph(int c) { return component_g[c]; }

    //! translate nodes from the input graph, to their labels in component c
    template <typename T>
    bool into_component(const int c, T& nodes_in, std::vector<int>& nodes_out) const {
        for (auto& x : nodes_in) {
            if (index[x] != c) return false;
            nodes_out.push_back(label[x]);
        }
        return true;
    }

    //! translate nodes from labels in component c, back to their original input labels
    template <typename T>
    void from_component(const int c, T& nodes_in, std::vector<int>& nodes_out) {
        std::vector<int>& comp = component[c];
        for (auto& x : nodes_in) {
            nodes_out.push_back(comp[x]);
        }
    }

  private:
    int __init_find(int x) {
        // NEVER CALL AFTER INITIALIZATION
        std::vector<int>& parent = index;
        if (parent[x] != x) parent[x] = __init_find(parent[x]);
        return parent[x];
    }

    void __init_union(int x, int y) {
        // NEVER CALL AFTER INITIALIZATION
        std::vector<int>& parent = index;
        std::vector<int>& rank = label;
        int xroot = __init_find(x);
        int yroot = __init_find(y);
        if (xroot == yroot)
            return;
        else if (rank[xroot] < rank[yroot])
            parent[xroot] = yroot;
        else if (rank[xroot] > rank[yroot])
            parent[yroot] = xroot;
        else {
            parent[yroot] = xroot;
            rank[xroot]++;
        }
    }

    std::vector<int> index;  // NOTE: dual-purpose -- parent for the union/find; index-of-component later
    std::vector<int> label;  // NOTE: dual-purpose -- rank for the union/find; label-in-component later
    std::vector<int> _num_reserved;
    std::vector<std::vector<int> > component;
    std::vector<input_graph> component_g;
};
}
