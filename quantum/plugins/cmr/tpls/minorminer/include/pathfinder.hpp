#pragma once

#include <algorithm>
#include <functional>
#include <future>
#include <iostream>
#include <limits>
#include <mutex>
#include <random>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "embedding.hpp"
#include "embedding_problem.hpp"
#include "pairing_queue.hpp"
#include "util.hpp"

namespace find_embedding {

// The pathfinder_base object manages the heuristic embedding process.  It contains various heuristic improvement
// strategies, shared data structures used for those strategies, and a master heuristic that dispatches them.
// The most basic operation is to tear out and replace a single chain.  Chain placement for a variable u is accomplished
// by initiating Dijkstra's algorithm at each embedded neighbor's chain, to compute the distance from each qubit to the
// chains in question.  Then, the sum of these distances is used as a fitness metric, to select a root qubit q for u.
// Shortest paths from each neighboring chain to u, found in Dijkstra's algorithm, are taken together to form the new
// chain for u.
// The execution of Dijkstra's algorithm is responsible for 99% of our runtime.  It's easily parallelized when variables
// have a large number of neighbors.  The serial/parallel versions occur below.

// look at me, forward-declaring liek an adult
template <typename T>
class pathfinder_base;
template <typename T>
class pathfinder_serial;
template <typename T>
class pathfinder_parallel;

template <typename embedding_problem_t>
class pathfinder_base {
    friend class pathfinder_serial<embedding_problem_t>;
    friend class pathfinder_parallel<embedding_problem_t>;

  public:
    using embedding_t = embedding<embedding_problem_t>;

  protected:
    embedding_problem_t ep;

    optional_parameters &params;

    embedding_t bestEmbedding;
    embedding_t lastEmbedding;
    embedding_t currEmbedding;
    embedding_t initEmbedding;

    int num_qubits, num_reserved;
    int num_vars, num_fixed;

    vector<vector<int>> parents;
    vector<distance_t> total_distance;

    vector<int> tmp_space;
    vector<int> min_list;

    int_queue intqueue;

    vector<distance_t> qubit_weight;

    vector<int> tmp_stats;
    vector<int> best_stats;

    int pushback;

    clock::time_point stoptime;

    vector<distance_queue> dijkstras;

  public:
    pathfinder_base(optional_parameters &p_, int &n_v, int &n_f, int &n_q, int &n_r, vector<vector<int>> &v_n,
                    vector<vector<int>> &q_n)
            : ep(p_, n_v, n_f, n_q, n_r, v_n, q_n),
              params(p_),
              bestEmbedding(ep),
              lastEmbedding(ep),
              currEmbedding(ep),
              initEmbedding(ep, params.fixed_chains, params.initial_chains),
              num_qubits(ep.num_qubits()),
              num_reserved(ep.num_reserved()),
              num_vars(ep.num_vars()),
              num_fixed(ep.num_fixed()),
              parents(num_vars + num_fixed, vector<int>(num_qubits + num_reserved, 0)),
              total_distance(num_qubits, 0),
              tmp_space(num_qubits + num_reserved, 0),
              min_list(num_qubits, 0),
              intqueue(num_qubits),
              qubit_weight(num_qubits, 0),
              tmp_stats(),
              best_stats(),
              dijkstras(num_vars + num_fixed, distance_queue(num_qubits)) {}
    virtual ~pathfinder_base() {}

    //! nonzero return if this is an improvement on our previous best embedding
    int check_improvement(const embedding_t &emb) {
        int better = 0;
        int embedded = emb.statistics(tmp_stats);
        if (embedded > ep.embedded) {
            params.major_info("embedding found.\n");
            better = ep.embedded = 1;
        }
        if (embedded < ep.embedded) return 0;
        int minorstat = tmp_stats.back();
        int major = best_stats.size() - tmp_stats.size();
        int minor = (best_stats.size() == 0) ? 0 : best_stats.back() - minorstat;

        better |= (major > 0) || (best_stats.size() == 0);
        if (better) {
            if (ep.embedded) {
                params.major_info("max chain length %d; num max chains=%d\n", tmp_stats.size() - 1, minorstat);
                ep.target_chainsize = tmp_stats.size() - 1;
            } else {
                params.major_info("max qubit fill %d; num maxfull qubits=%d\n", tmp_stats.size() + 1, minorstat);
            }
        }
        if ((!better) && (major == 0) && (minor > 0)) {
            if (ep.embedded) {
                params.minor_info("    num max chains=%d\n", minorstat);
            } else {
                params.minor_info("    num max qubits=%d\n", minorstat);
            }
            better = 1;
        }
        if (!better && (major == 0) && (minor == 0)) {
            for (int i = tmp_stats.size(); i--;) {
                if (tmp_stats[i] == best_stats[i]) continue;
                if (tmp_stats[i] < best_stats[i]) better = 1;
                break;
            }
        }
        if (better) {
            bestEmbedding = emb;
            tmp_stats.swap(best_stats);
        }
        return better;
    }

    //! chain accessor
    const chain &get_chain(int u) const { return bestEmbedding.get_chain(u); }

  protected:
    //! tear out and replace the chain in `emb` for variable `u`
    int find_chain(embedding_t &emb, const int u) {
        if (ep.embedded || ep.desperate) emb.steal_all(u);
        emb.tear_out(u);
        return find_chain(emb, u, ep.target_chainsize);
    }

    //! sweep over all variables, either keeping them if they are pre-initialized and connected,
    //! and otherwise finding new chains for them (each, in turn, seeking connection only with
    //! neighbors that already have chains)
    int initialization_pass(embedding_t &emb) {
        for (auto &u : ep.var_order((params.restrict_chains.size()) ? VARORDER_DFS : VARORDER_PFS)) {
            if (emb.chainsize(u) && emb.linked(u)) {
                ep.debug("chain for %d kept during initialization\n", u);
            } else {
                ep.debug("finding a new chain for %d\n", u);
                if (!find_chain(emb, u)) return -1;
            }
        }
        if (params.localInteractionPtr->cancelled(stoptime))
            return -2;
        else
            return 1;
    }

    //! tear up and replace each variable
    int improve_overfill_pass(embedding_t &emb) {
        bool improved = false;
        for (auto &u : ep.var_order(VARORDER_PFS)) {
            ep.debug("finding a new chain for %d\n", u);
            if (!find_chain(emb, u)) return -1;

            improved |= check_improvement(emb);
            if (ep.embedded) break;
        }
        if (params.localInteractionPtr->cancelled(stoptime))
            return -2;
        else {
            return improved;
        }
    }

    //! tear up and replace each chain, strictly improving or maintaining the
    //! maximum qubit fill seen by each chain
    int pushdown_overfill_pass(embedding_t &emb) {
        int oldbound = ep.weight_bound;

        bool improved = false;
        for (auto &u : ep.var_order()) {
            int r = 0;
            if (pushback < num_vars) {
                ep.debug("finding a new chain for %d (pushdown)\n", u);
                int maxfill = 0;
                emb.steal_all(u);
                for (auto &q : emb.get_chain(u)) maxfill = max(maxfill, emb.weight(q));

                ep.weight_bound = max(0, maxfill);

                emb.tear_out(u);
                r = find_chain(emb, u, 0);
                if (!r) pushback += 3;
            }
            if (!r) {
                ep.debug("finding a new chain for %d (pushdown pause/repeat)\n", u);
                ep.weight_bound = oldbound;
                if (!find_chain(emb, u)) return -1;
            }
            improved |= check_improvement(emb);
            if (ep.embedded) break;
        }
        ep.weight_bound = oldbound;
        if (params.localInteractionPtr->cancelled(stoptime))
            return -2;
        else {
            if (!improved) pushback += (num_vars * 2) / params.inner_rounds;
            return improved;
        }
    }

    //! tear up and replace each chain, attempting to rebalance the chains and
    //! lower the maximum chainlength
    int improve_chainlength_pass(embedding_t &emb) {
        bool improved = false;
        for (auto &u : ep.var_order(ep.improved ? VARORDER_KEEP : VARORDER_PFS)) {
            ep.debug("finding a new chain for %d\n", u);
            if (!find_chain(emb, u)) return -1;

            improved |= check_improvement(emb);
        }
        if (params.localInteractionPtr->cancelled(stoptime))
            return -2;
        else {
            return improved;
        }
    }

    //! incorporate the qubit weights associated with the chain for `v` into
    //! `total_distance`
    void accumulate_distance_at_chain(const embedding_t &emb, const int v) {
        if (!ep.fixed(v)) {
            for (auto &q : emb.get_chain(v)) {
                auto w = qubit_weight[q];
                if ((total_distance[q] != max_distance) && !(ep.reserved(q)) && (w != max_distance) &&
                    emb.weight(q) < ep.weight_bound && w > 0)
                    total_distance[q] += w;
                else
                    total_distance[q] = max_distance;
            }
        }
    }

    //! incorporate the distances associated with the chain for `v` into
    //! `total_distance`
    void accumulate_distance(const embedding_t &emb, const int v, vector<int> &visited, const int start,
                             const int stop) {
        auto &distqueue = dijkstras[v];
        for (int q = start; q < stop; q++) {
            auto dist = distqueue.get_value(q);
            if ((visited[q] == 1) && (total_distance[q] != max_distance) && !(ep.reserved(q)) &&
                (dist != max_distance) && emb.weight(q) < ep.weight_bound && dist > 0) {
                total_distance[q] += dist;
            } else {
                total_distance[q] = max_distance;
            }
        }
    }

    //! a wrapper for `accumulate_distance` and `accumulate_distance_at_chain`
    inline void accumulate_distance(const embedding_t &emb, const int v, vector<int> &visited) {
        accumulate_distance_at_chain(emb, v);
        accumulate_distance(emb, v, visited, 0, num_qubits);
    }

  private:
    //! compute the distances from all neighbors of `u` to all qubits
    virtual void prepare_root_distances(const embedding_t &emb, const int u) = 0;

    //! after `u` has been torn out, perform searches from each neighboring chain,
    //! select a minimum-distance root, and construct the chain
    int find_chain(embedding_t &emb, const int u, int target_chainsize) {
        prepare_root_distances(emb, u);

        // select a random root among those qubits at minimum heuristic distance
        collectMinima(total_distance, min_list);

        int q0 = min_list[ep.randint(min_list.size())];
        if (total_distance[q0] == max_distance) return 0;  // oops all qubits were overfull or unreachable

        emb.construct_chain(u, q0, parents);
        emb.flip_back(u, target_chainsize);

        return 1;
    }

  protected:
    //! run dijkstra's algorithm, seeded at the chain for `v`, using the `visited` vector
    //! note: qubits are only visited if `visited[q] = 1`.  the value `-1` is used to prevent
    //! searching of overfull qubits
    void compute_distances_from_chain(const embedding_t &emb, const int &v, vector<int> &visited) {
        auto &pq = dijkstras[v];
        auto &parent = parents[v];
        int q;
        distance_t d;

        pq.reset();
        // scan through the qubits.
        // * qubits in the chain of v have distance 0,
        // * overfull qubits are tagged as visited with a special value of -1
        if (ep.fixed(v)) {
            for (auto &q : emb.get_chain(v)) {
                parent[q] = -1;
                for (auto &p : ep.qubit_neighbors(q)) {
                    pq.set_value(p, qubit_weight[p]);
                    parent[p] = q;
                }
            }
        } else {
            for (auto &q : emb.get_chain(v)) {
                pq.set_value(q, 0);
                parent[q] = -1;
            }
        }

        for (q = num_qubits; q--;)
            if (emb.weight(q) >= ep.weight_bound) visited[q] = -1;

        // this is a vanilla implementation of node-weight dijkstra -- probably where we spend the most time.
        while (pq.pop_min(q, d)) {
            visited[q] = 1;
            for (auto &p : ep.qubit_neighbors(q))
                if (!visited[p])
                    if (pq.check_decrease_value(p, d + qubit_weight[p])) parent[p] = q;
        }
    }

    //! compute the weight of each qubit, first selecting `alpha`
    void compute_qubit_weights(const embedding_t &emb) {
        // first, find the maximum value of alpha that won't result in arithmetic overflow
        int maxwid = min(emb.max_weight(), ep.alpha);
        if (maxwid > ep.weight_bound) maxwid = ep.weight_bound;
        int alpha = maxwid > 1 ? ep.alpha / maxwid : ep.alpha - 1;
        compute_qubit_weights(emb, alpha, maxwid, 0, num_qubits);
    }

    //! compute the weight of each qubit in the range from `start` to `stop`,
    //! where the weight is `2^(alpha*fill)` where `fill` is the number of
    //! chains which use that qubit
    void compute_qubit_weights(const embedding_t &emb, const int alpha, const int maxwid, const int start,
                               const int stop) {
        for (int q = start; q < stop; q++)
            qubit_weight[q] = static_cast<const distance_t>(1) << (alpha * min(maxwid, emb.weight(q)));
    }

  public:
    //! perform the heuristic embedding, returning 1 if an embedding was found and 0 otherwise
    int heuristicEmbedding() {
        auto timeout0 = duration<double>(params.timeout);
        auto timeout = duration_cast<clock::duration>(timeout0);
        stoptime = clock::now() + timeout;
        ep.target_chainsize = 0;
        if (params.skip_initialization) {
            if (initEmbedding.linked()) {
                currEmbedding = initEmbedding;
            } else {
                ep.error(
                        "cannot bootstrap from initial embedding.  stopping.  disable skip_initialization or throw "
                        "this embedding away\n");
                return 0;
            }
        } else {
            currEmbedding = initEmbedding;
            if (initialization_pass(currEmbedding) <= 0) {
                ep.error("failed during initialization. embeddings may be invalid.\n");
                return 0;
            }
        }
        ep.major_info("initialized\n");
        ep.initialized = 1;
        best_stats.clear();
        check_improvement(currEmbedding);
        ep.improved = 1;
        currEmbedding = bestEmbedding;
        for (int trial_patience = params.tries; trial_patience-- && (!ep.embedded);) {
            int improvement_patience = params.max_no_improvement;
            ep.major_info("embedding trial %d\n", params.tries - trial_patience);
            pushback = 0;
            for (int round_patience = params.inner_rounds;
                 round_patience-- && improvement_patience && (!ep.embedded);) {
                int r;
                ep.extra_info("overfill improvement pass (%d more before giving up on this trial)\n",
                              min(improvement_patience, round_patience) - 1);
                ep.extra_info("max qubit fill %d, num max qubits %d\n", best_stats.size() + 1, best_stats.back());
                ep.desperate = (improvement_patience <= 1) | (!trial_patience) | (!round_patience);
                if (pushback < num_vars) {
                    r = pushdown_overfill_pass(currEmbedding);
                } else {
                    pushback--;
                    r = improve_overfill_pass(currEmbedding);
                }
                switch (r) {
                    case -2:
                        improvement_patience = 0;
                        break;
                    case -1:
                        currEmbedding = bestEmbedding;  // fallthrough
                    case 0:
                        improvement_patience--;
                        ep.improved = 0;
                        break;
                    case 1:
                        improvement_patience = params.max_no_improvement;
                        pushback = 0;
                        ep.improved = 1;
                        break;
                }
            }
            if (trial_patience && (ep.embedded) && (improvement_patience == 0)) {
                ep.initialized = 0;
                ep.desperate = 1;
                currEmbedding = bestEmbedding;
                int r = initialization_pass(currEmbedding);
                switch (r) {
                    case -2:
                        trial_patience = 0;
                        break;
                    case -1:
                        currEmbedding = bestEmbedding;
                        break;
                    case 1:
                        check_improvement(currEmbedding);
                        break;
                }
                ep.initialized = 1;
                ep.desperate = 0;
            }
        }
        if (ep.embedded && params.chainlength_patience) {
            ep.major_info("reducing chain lengths\n");
            int improvement_patience = params.chainlength_patience;
            ep.weight_bound = 1;
            currEmbedding = bestEmbedding;
            while (improvement_patience) {
                lastEmbedding = currEmbedding;
                ep.extra_info("chainlength improvement pass (%d more before giving up)\n", improvement_patience - 1);
                ep.extra_info("max chain length %d, num of max chains %d\n", best_stats.size() - 1, best_stats.back());
                ep.desperate = (improvement_patience == 1);
                int r = improve_chainlength_pass(currEmbedding);
                switch (r) {
                    case -1:
                        currEmbedding = lastEmbedding;
                        improvement_patience--;
                        break;
                    case -2:
                        improvement_patience = 0;
                        break;  // interrupting here is inconsequential; bestEmbedding is valid
                    case 0:
                        improvement_patience--;
                        ep.improved = 0;
                        break;
                    case 1:
                        improvement_patience = params.chainlength_patience;
                        ep.improved = 1;
                        break;
                }
            }
        }
        return ep.embedded;
    }
};

//! A pathfinder where the Dijkstra-from-neighboring-chain passes are done serially.
template <typename embedding_problem_t>
class pathfinder_serial : public pathfinder_base<embedding_problem_t> {
  public:
    using super = pathfinder_base<embedding_problem_t>;
    using embedding_t = embedding<embedding_problem_t>;

  private:
    vector<int> visited;

  public:
    pathfinder_serial(optional_parameters &p_, int n_v, int n_f, int n_q, int n_r, vector<vector<int>> &v_n,
                      vector<vector<int>> &q_n)
            : super(p_, n_v, n_f, n_q, n_r, v_n, q_n), visited(super::num_qubits + super::num_reserved) {}
    virtual ~pathfinder_serial() {}

    virtual void prepare_root_distances(const embedding_t &emb, const int u) override {
        super::ep.prepare_distances(super::total_distance, u, max_distance);
        super::compute_qubit_weights(emb);

        // run Dijkstra's algorithm from each neighbor to compute distances and shortest paths to neighbor's chains
        int neighbors_embedded = 0;
        for (auto &v : super::ep.var_neighbors(u)) {
            if (!emb.chainsize(v)) continue;
            neighbors_embedded++;
            super::ep.prepare_visited(visited, u, v);
            super::compute_distances_from_chain(emb, v, visited);
            super::accumulate_distance(emb, v, visited);
        }

        for (int q = super::num_qubits; q--;)
            if (emb.weight(q) >= super::ep.weight_bound) super::total_distance[q] = max_distance;
    }
};

//! A pathfinder where the Dijkstra-from-neighboring-chain passes are done serially.
template <typename embedding_problem_t>
class pathfinder_parallel : public pathfinder_base<embedding_problem_t> {
  public:
    using super = pathfinder_base<embedding_problem_t>;
    using embedding_t = embedding<embedding_problem_t>;

  private:
    int num_threads;
    vector<vector<int>> visited_list;
    vector<std::future<void>> futures;
    vector<int> thread_weight;
    mutex get_job;

    unsigned int nbr_i;
    int neighbors_embedded;

    void run_in_thread(const embedding_t &emb, const int u) {
        get_job.lock();
        while (1) {
            int v = -1;
            const vector<int> &neighbors = super::ep.var_neighbors(u);
            while (nbr_i < neighbors.size()) {
                int v0 = neighbors[nbr_i++];
                if (emb.chainsize(v0)) {
                    v = v0;
                    neighbors_embedded++;
                    break;
                }
            }
            get_job.unlock();

            if (v < 0) break;

            vector<int> &visited = visited_list[v];
            super::ep.prepare_visited(visited, u, v);
            super::compute_distances_from_chain(emb, v, visited);

            get_job.lock();
            super::accumulate_distance_at_chain(emb, v);
        }
    }

    template <typename C>
    void exec_chunked(C e_chunk) {
        const int grainsize = super::num_qubits / num_threads;
        int grainmod = super::num_qubits % num_threads;

        int a = 0;
        for (int i = num_threads; i--;) {
            int b = a + grainsize + (grainmod-- > 0);
            futures[i] = std::async(std::launch::async, e_chunk, a, b);
            a = b;
        }
        for (int i = num_threads; i--;) futures[i].wait();
    }

    template <typename C>
    void exec_indexed(C e_chunk) {
        const int grainsize = super::num_qubits / num_threads;
        int grainmod = super::num_qubits % num_threads;

        int a = 0;
        for (int i = num_threads; i--;) {
            int b = a + grainsize + (grainmod-- > 0);
            futures[i] = std::async(std::launch::async, e_chunk, i, a, b);
            a = b;
        }
        for (int i = num_threads; i--;) futures[i].wait();
    }

  public:
    pathfinder_parallel(optional_parameters &p_, int n_v, int n_f, int n_q, int n_r, vector<vector<int>> &v_n,
                        vector<vector<int>> &q_n)
            : super(p_, n_v, n_f, n_q, n_r, v_n, q_n),
              num_threads(min(p_.threads, n_q)),
              visited_list(n_v + n_f, vector<int>(super::num_qubits)),
              futures(num_threads),
              thread_weight(num_threads) {}
    virtual ~pathfinder_parallel() {}

    virtual void prepare_root_distances(const embedding_t &emb, const int u) override {
        exec_indexed([this, &emb](int i, int a, int b) { thread_weight[i] = emb.max_weight(a, b); });

        int maxwid = *std::max_element(std::begin(thread_weight), std::end(thread_weight));
        maxwid = min(super::ep.alpha, maxwid);
        if (maxwid > super::ep.weight_bound) maxwid = super::ep.weight_bound;
        int alpha = maxwid > 1 ? super::ep.alpha / maxwid : super::ep.alpha - 1;

        exec_chunked(
                [this, &emb, alpha, maxwid](int a, int b) { super::compute_qubit_weights(emb, alpha, maxwid, a, b); });

        exec_chunked(
                [this, u](int a, int b) { this->ep.prepare_distances(this->total_distance, u, max_distance, a, b); });

        nbr_i = 0;
        neighbors_embedded = 0;
        for (int i = 0; i < num_threads; i++)
            futures[i] = std::async(std::launch::async, [this, &emb, &u]() { run_in_thread(emb, u); });
        for (int i = 0; i < num_threads; i++) futures[i].wait();

        for (auto &v : super::ep.var_neighbors(u)) {
            if (emb.chainsize(v)) {
                exec_chunked(
                        [this, &emb, v](int a, int b) { this->accumulate_distance(emb, v, visited_list[v], a, b); });
            }
        }
    }
};
}
