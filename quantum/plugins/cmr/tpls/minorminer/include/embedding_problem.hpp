#pragma once

#include <algorithm>
#include <chrono>
#include <limits>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "util.hpp"

namespace find_embedding {

enum VARORDER { VARORDER_SHUFFLE, VARORDER_DFS, VARORDER_BFS, VARORDER_PFS, VARORDER_RPFS, VARORDER_KEEP };

// This file contains component classes for constructing embedding problems.  Presently, an embedding_problem class is
// constructed by combining the embedding_problem_base class with a domain_handler class and a fixed_handler class.
// This is used to accomplish dynamic dispatch for code in/around the inner loops without fouling their performance.

// Domain handlers are used to control which qubits are available for use for which variables.
// they perform three operations:
//   * marking unavailable qubits as "visited" so that we don't visit them in Dijkstra's algorithm
//   * setting unavailable qubits to an effectively infinite cost for root selection
//   * checking if a particular qubit is available for a particular variable

//! this is the trivial domain handler, where every variable is allowed to use every qubit
class domain_handler_universe {
  public:
    domain_handler_universe(optional_parameters & /*p*/, int /*n_v*/, int /*n_f*/, int /*n_q*/, int /*n_r*/) {}
    virtual ~domain_handler_universe() {}

    static inline void prepare_visited(vector<int> &visited, int /*u*/, int /*v*/) {
        std::fill(std::begin(visited), std::end(visited), 0);
    }

    static inline void prepare_distances(vector<distance_t> &distance, const int /*u*/, const distance_t & /*mask_d*/) {
        std::fill(std::begin(distance), std::end(distance), 0);
    }

    static inline void prepare_distances(vector<distance_t> &distance, const int /*u*/, const distance_t & /*mask_d*/,
                                         const int start, const int stop) {
        std::fill(std::begin(distance) + start, std::begin(distance) + stop, 0);
    }

    static inline bool accepts_qubit(int /*u*/, int /*q*/) { return 1; }
};

//! this domain handler stores masks for each variable so that prepare_visited and prepare_distances are barely more
//! expensive than a memcopy
class domain_handler_masked {
    optional_parameters &params;
    vector<vector<int>> masks;

  public:
    domain_handler_masked(optional_parameters &p, int n_v, int n_f, int n_q, int n_r)
            : params(p), masks(n_v + n_f, vector<int>()) {
#ifndef NDEBUG
        for (auto &vC : params.restrict_chains)
            for (auto &q : vC.second) minorminer_assert(0 <= q && q < n_q + n_r);
#endif
        auto nostrix = std::end(params.restrict_chains);
        for (int v = n_v + n_f; v--;) {
            auto chain = params.restrict_chains.find(v);
            if (chain != nostrix) {
                masks[v].resize(n_q + n_r, -1);
                auto vC = *chain;
                for (auto &q : vC.second) masks[v][q] = 0;
            } else {
                masks[v].resize(n_q + n_r, 0);
            }
        }
    }
    virtual ~domain_handler_masked() {}

    inline void prepare_visited(vector<int> &visited, const int u, const int v) {
        vector<int> &uMask = masks[u];
        vector<int> &vMask = masks[v];
        int *umask = uMask.data();
        int *vmask = vMask.data();
        int *vis = visited.data();
        for (int *stop = vis + visited.size(); vis < stop; ++vis, ++umask, ++vmask) *vis = (*umask) & (*vmask);
    }

    inline void prepare_distances(vector<distance_t> &distance, const int u, const distance_t &mask_d) {
        vector<int> &uMask = masks[u];
        int *umask = uMask.data();
        distance_t *dist = distance.data();
        distance_t *dend = dist + distance.size();
        for (; dist < dend; dist++, umask++) *dist = (-(*umask)) * mask_d;
    }

    inline void prepare_distances(vector<distance_t> &distance, const int u, const distance_t &mask_d, const int start,
                                  const int stop) {
        vector<int> &uMask = masks[u];
        int *umask = uMask.data() + start;
        distance_t *dist = distance.data() + start;
        distance_t *dend = distance.data() + stop;
        for (; dist < dend; dist++, umask++) *dist = (-(*umask)) * mask_d;
    }

    inline bool accepts_qubit(const int u, const int q) { return !(masks[u][q]); }
};

// Fixed handlers are used to control which variables are allowed to be torn up and replaced.  Currently, there is no
// option to fix/unfix variables after an embedding problem has been instantiated, but TODO that will be implemented in
// the future in fixed_handler_list.  Fixed variables are assumed to have chains; reserved qubits are not available for
// use in producing new chains (currently TODO this only happens because they belong to the chains of fixed variables).

//! This fixed handler is used when there are no fixed variables.
class fixed_handler_none {
  public:
    fixed_handler_none(optional_parameters & /*p*/, int /*n_v*/, int /*n_f*/, int /*n_q*/, int /*n_r*/) {}
    virtual ~fixed_handler_none() {}

    static inline bool fixed(int /*u*/) { return false; }

    static inline bool reserved(int /*u*/) { return false; }
};

//! This fixed handler is used when the fixed variables are processed before instantiation and relabeled such that
//! variables v >= num_v are fixed and qubits q >= num_q are reserved
class fixed_handler_hival {
  private:
    int num_v, num_q;

  public:
    fixed_handler_hival(optional_parameters & /*p*/, int n_v, int /*n_f*/, int n_q, int /*n_r*/)
            : num_v(n_v), num_q(n_q) {}
    virtual ~fixed_handler_hival() {}

    inline bool fixed(const int u) { return u >= num_v; }

    inline bool reserved(const int q) { return q >= num_q; }
};

//! This fixed handler is used when variables are allowed to be fixed after instantiation.  For that functionality, we
//! probably need...
//! * dynamic modification of var_neighbors and qubit_neighbors to maintain speed gains: fixed variables are sinks,
//! reserved qubits are sources.
//! * access to / ownership of var_neighbors and qubit_neighbors in this data structure
//! * move existing initialization code from find_embedding.hpp into fixed_handler_hival (note the interplay with
//! shuffling qubit labels, this might get gross)
class fixed_handler_list {
  private:
    vector<int> var_fixed;

  public:
    fixed_handler_list(optional_parameters &p, int n_v, int n_f, int /*n_q*/, int n_r) : var_fixed(n_v, 0) {
        minorminer_assert(n_f == 0);
        minorminer_assert(n_r == 0);
        for (auto &vC : p.fixed_chains) var_fixed[vC.first] = 1;
    }
    virtual ~fixed_handler_list() {}

    inline bool fixed(const int u) { return static_cast<bool>(var_fixed[u]); }

    inline bool reserved(const int) { return 0; }
};

//! Common form for all embedding problems.
//!
//! Needs to be extended with a fixed handler and domain handler to be complete.
class embedding_problem_base {
  protected:
    int num_v, num_f, num_q, num_r;

    //! Mutable references to qubit numbers and variable numbers
    vector<vector<int>> &qubit_nbrs, &var_nbrs;

    //! distribution over [0, 0xffffffff]
    uniform_int_distribution<> rand;

    vector<int> var_order_space;
    vector<int> var_order_visited;
    vector<int> var_order_shuffle;

    int_queue var_order_pq;

  public:
    //! A mutable reference to the user specified parameters
    optional_parameters &params;

    int alpha, initialized, embedded, desperate, target_chainsize, improved, weight_bound;

    embedding_problem_base(optional_parameters &p_, int n_v, int n_f, int n_q, int n_r, vector<vector<int>> &v_n,
                           vector<vector<int>> &q_n)
            : num_v(n_v),
              num_f(n_f),
              num_q(n_q),
              num_r(n_r),
              qubit_nbrs(q_n),
              var_nbrs(v_n),
              rand(0, 0xffffffff),
              var_order_space(n_v),
              var_order_visited(n_v, 0),
              var_order_shuffle(n_v),
              var_order_pq(n_v + n_f),
              params(p_),
              initialized(0),
              embedded(0),
              desperate(0),
              target_chainsize(0),
              improved(0) {
        alpha = 8 * sizeof(distance_t);
        int N = 2 * num_q;
        while (N /= 2) alpha--;
        alpha = max(1, alpha);
        weight_bound = min(params.max_fill, alpha);
    }
    virtual ~embedding_problem_base() {}

    //! a vector of neighbors for the variable `u`
    const vector<int> &var_neighbors(int u) const { return var_nbrs[u]; }

    //! a vector of neighbors for the qubit `q`
    const vector<int> &qubit_neighbors(int q) const { return qubit_nbrs[q]; }

    //! number of variables which are not fixed
    inline int num_vars() const { return num_v; }

    //! number of qubits which are not reserved
    inline int num_qubits() const { return num_q; }

    //! number of fixed variables
    inline int num_fixed() const { return num_f; }

    //! number of reserved qubits
    inline int num_reserved() const { return num_r; }

    //! printf regardless of the verbosity level
    template <typename... Args>
    void error(const char *format, Args... args) const {
        params.error(format, args...);
    }

    //! printf at the major_info verbosity level
    template <typename... Args>
    void major_info(const char *format, Args... args) const {
        params.major_info(format, args...);
    }

    //! print at the minor_info verbosity level
    template <typename... Args>
    void minor_info(const char *format, Args... args) const {
        params.minor_info(format, args...);
    }

    //! print at the extra_info verbosity level
    template <typename... Args>
    void extra_info(const char *format, Args... args) const {
        params.extra_info(format, args...);
    }

    //! print at the debug verbosity level (only works when `CPPDEBUG` is set)
    template <typename... Args>
    void debug(const char *ONDEBUG(format), Args... ONDEBUG(args)) const {
        ONDEBUG(params.debug(format, args...));
    }

    //! make a random integer between 0 and `m-1`
    int randint(int m) { return rand(params.rng, typename decltype(rand)::param_type(0, m - 1)); }

    //! shuffle the data bracketed by iterators `a` and `b`
    template <typename A, typename B>
    void shuffle(A a, B b) {
        std::shuffle(a, b, params.rng);
    }

    //! compute the connected component of the subset `component` of qubits,
    //! containing `q0`, and using`visited` as an indicator for which qubits
    //! have been explored
    void qubit_component(int q0, vector<int> &component, vector<int> &visited) {
        dfs_component(q0, qubit_nbrs, component, visited);
    }

    //! compute a variable ordering according to the `order` strategy
    const vector<int> &var_order(VARORDER order = VARORDER_SHUFFLE) {
        if (order == VARORDER_KEEP) {
            minorminer_assert(var_order_space.size() > 0);
            return var_order_space;
        }
        var_order_space.clear();
        var_order_shuffle.clear();
        for (int v = num_v; v--;) var_order_shuffle.push_back(v);
        shuffle(std::begin(var_order_shuffle), std::end(var_order_shuffle));
        if (order == VARORDER_SHUFFLE) {
            var_order_shuffle.swap(var_order_space);
        } else {
            var_order_visited.assign(num_v, 0);
            var_order_visited.resize(num_v + num_f, 1);
            for (auto v : var_order_shuffle)
                if (!var_order_visited[v]) switch (order) {
                        case VARORDER_DFS:
                            dfs_component(v, var_nbrs, var_order_space, var_order_visited);
                            break;
                        case VARORDER_BFS:
                            bfs_component(v, var_nbrs, var_order_space, var_order_visited);
                            break;
                        case VARORDER_PFS:
                            pfs_component(v, var_nbrs, var_order_space, var_order_visited);
                            break;
                        case VARORDER_RPFS:
                            rpfs_component(v, var_nbrs, var_order_space, var_order_visited);
                            break;
                        default:
                            throw - 1;
                    }
        }
        return var_order_space;
    }

    //! Perform a depth first search
    void dfs_component(int x, const vector<vector<int>> &neighbors, vector<int> &component, vector<int> &visited) {
        size_t front = component.size();
        component.push_back(x);
        visited[x] = 1;
        while (front < component.size()) {
            int x = component[front++];
            unsigned int lastback = component.size();
            for (auto &y : neighbors[x]) {
                if (!visited[y]) {
                    visited[y] = 1;
                    component.push_back(y);
                }
            }
            if (lastback != component.size()) shuffle(std::begin(component) + lastback, std::end(component));
        }
    }

  private:
    //! Perform a priority first search (priority = #of visited neighbors)
    void pfs_component(int x, const vector<vector<int>> &neighbors, vector<int> &component, vector<int> &visited) {
        int_queue::value_type d;
        var_order_pq.reset();
        minorminer_assert(var_order_pq.has(x));
        var_order_pq.set_value(x, 0);
        while (var_order_pq.pop_min(x, d)) {
            visited[x] = 1;
            component.push_back(x);
            for (auto adjacent_var : neighbors[x]) {
                if (!visited[adjacent_var]) {
                    if (!var_order_pq.check_decrease_value(adjacent_var, 0)) {
                        d = var_order_pq.get_value(adjacent_var) >> 8;
                        var_order_pq.decrease_value(adjacent_var, ((d - 1) << 8) + randint(256));
                    }
                }
            }
        }
    }

    //! Perform a reverse priority first search (reverse priority = #of unvisited neighbors)
    void rpfs_component(int x, const vector<vector<int>> &neighbors, vector<int> &component, vector<int> &visited) {
        int_queue::value_type d;
        var_order_pq.reset();
        minorminer_assert(var_order_pq.has(x));
        var_order_pq.set_value(x, 0);
        while (var_order_pq.pop_min(x, d)) {
            visited[x] = 1;
            component.push_back(x);
            for (auto &y : neighbors[x]) {
                if (!visited[y]) {
                    int z = 0;
                    for (auto &w : neighbors[z])
                        if (!visited[w]) z++;
                    var_order_pq.set_value(y, z * 256 + randint(256));
                }
            }
        }
    }

    //! Perform a breadth first search, shuffling level sets
    void bfs_component(int x, const vector<vector<int>> &neighbors, vector<int> &component, vector<int> &visited) {
        size_t front = component.size();
        int_queue::value_type d = 0, d0 = 0;
        var_order_pq.reset();
        minorminer_assert(var_order_pq.has(x));
        var_order_pq.set_value(x, 0);
        while (var_order_pq.pop_min(x, d)) {
            if (d0 && d > d0) {
                shuffle(std::begin(component) + front, std::end(component));
                d0 = d;
                front = component.size();
            }
            visited[x] = 1;
            component.push_back(x);
            for (auto &y : neighbors[x])
                if (!visited[y]) var_order_pq.check_decrease_value(y, d + 1);
        }
        shuffle(std::begin(component) + front, std::end(component));
    }
};

//! A template to construct a complete embedding problem by combining
//! `embedding_problem_base` with fixed/domain handlers.
template <class fixed_handler, class domain_handler>
class embedding_problem : public embedding_problem_base, public fixed_handler, public domain_handler {
  private:
    using ep_t = embedding_problem_base;
    using fh_t = fixed_handler;
    using dh_t = domain_handler;

  public:
    embedding_problem(optional_parameters &p, int n_v, int n_f, int n_q, int n_r, vector<vector<int>> &v_n,
                      vector<vector<int>> &q_n)
            : embedding_problem_base(p, n_v, n_f, n_q, n_r, v_n, q_n),
              fixed_handler(p, n_v, n_f, n_q, n_r),
              domain_handler(p, n_v, n_f, n_q, n_r) {}
    virtual ~embedding_problem() {}
};
}
