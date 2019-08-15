#pragma once

#include <algorithm>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "graph.hpp"
#include "pathfinder.hpp"
#include "util.hpp"

namespace find_embedding {

//! This function parses inputs, scrambles node labels, dispatches
//! the main embedding algorithm, pathfinder_base.heuristicEmbedding,
//! and finally unscrambles the resulting answer.
template <class pathfinder_t>
int find_embedding_execute(::graph::input_graph &var_g, ::graph::input_graph &qubit_g, optional_parameters &params_,
                           std::vector<std::vector<int>> &chains) {
    int num_tot_vars = var_g.num_nodes();
    int num_tot_qubs = qubit_g.num_nodes();
    std::vector<int> var_fixed_unscrewed(num_tot_vars, 0);
    std::vector<int> qub_reserved_unscrewed(num_tot_qubs, 0);

    // roll call for fixed variables and reserved qubits
    int num_reserved = 0;
    int num_fixed = params_.fixed_chains.size();
    for (auto &vC : params_.fixed_chains) {
        var_fixed_unscrewed[vC.first] = 1;
        for (auto &q : vC.second) {
            if (!qub_reserved_unscrewed[q]) {
                qub_reserved_unscrewed[q] = 1;
                num_reserved++;
            }
        }
    }

    int num_vars = num_tot_vars - num_fixed;
    // int num_edges = var_g.num_edges();
    optional_parameters params = params_;

    std::vector<int> unscrew_vars;
    std::vector<int> fix_vars;
    // split the fixed vars out from the rest
    for (int v = 0; v < num_tot_vars; v++)
        if (var_fixed_unscrewed[v])
            fix_vars.push_back(v);
        else
            unscrew_vars.push_back(v);

    // shuffling the rest of the vars gives a random relabeling
    std::shuffle(std::begin(unscrew_vars), std::end(unscrew_vars), params.rng);
    // dump the fixed vars at the end -- don't bother shuffling them because their labels don't matter
    unscrew_vars.insert(unscrew_vars.end(), fix_vars.begin(), fix_vars.end());
    std::vector<int> screw_vars(num_tot_vars);

    // compute the inverse of the relabeling

    for (int v = num_tot_vars; v--;) screw_vars[unscrew_vars[v]] = v;

    // compute the relabeled neighbors dictionary, omitting outbound neighbors of fixed variables
    std::vector<std::vector<int>> var_nbrs;
    var_g.get_neighbors_sinks_relabel(var_nbrs, var_fixed_unscrewed, screw_vars);

    // compute the connected components of the qubit graph
    // * virtually merge reserved qubits into a single component
    // * relabelling components is similar to the vars above:
    //    * first num_qubits labels are randomized
    //    * remaining num_reserved labels are used for reserved qubits
    ::graph::components qubit_components(qubit_g, params.rng, qub_reserved_unscrewed);
    bool success = false;
    int tried = 0;
    for (int c = 0; c < qubit_components.size() && !success; c++) {
        int num_qubits = qubit_components.size(c);

        params.restrict_chains.clear();
        params.initial_chains.clear();
        params.fixed_chains.clear();

        // translate all chains into this component if possible
        int all_chains = 1;
        for (auto &vC : params_.initial_chains) {
            // don't bother putting in initial chains that will be overwritten by fixed chains
            if (!var_fixed_unscrewed[vC.first]) {
                std::vector<int> C;
                all_chains &= qubit_components.into_component(c, vC.second, C);
                params.initial_chains.emplace(screw_vars[vC.first], C);
            }
        }
        for (auto &vC : params_.restrict_chains) {
            std::vector<int> C;
            all_chains &= qubit_components.into_component(c, vC.second, C);
            params.restrict_chains.emplace(screw_vars[vC.first], C);
        }
        for (auto &vC : params_.fixed_chains) {
            std::vector<int> C;
            all_chains &= qubit_components.into_component(c, vC.second, C);
            params.fixed_chains.emplace(screw_vars[vC.first], C);
        }

        if (all_chains) {
            std::vector<int> qub_fixed(num_qubits, 0);
            for (auto &vC : params.fixed_chains)
                for (auto &q : vC.second) qub_fixed[q] = 1;

            tried = 1;
            std::vector<std::vector<int>> qubit_nbrs;
            ::graph::input_graph &comp = qubit_components.component_graph(c);
            int num_res = qubit_components.num_reserved(c);

            // get the neighbor dictionary for qubits, omitting inbound neighbors of reserved qubits
            comp.get_neighbors_sources(qubit_nbrs, qub_fixed);

            pathfinder_t pf(params, num_vars, fix_vars.size(), num_qubits - num_res, num_res, var_nbrs, qubit_nbrs);

            success = pf.heuristicEmbedding();

            if (params.return_overlap || success) {
                chains.resize(num_tot_vars);
                for (int u = 0; u < num_tot_vars; u++) {
                    chains[u].clear();
                    qubit_components.from_component(c, pf.get_chain(screw_vars[u]), chains[u]);
                }
                break;
            } else {
                chains.clear();
            }
        }
    }

    if (!tried) {
        if (!qubit_g.num_edges()) {
            params.localInteractionPtr->displayOutput("never tried to embed anything; the qubit graph has no edges");
        } else {
            params.localInteractionPtr->displayOutput(
                    "never tried to embed anything; the chains supplied in fixed_chains and initial_chains must all "
                    "belong to a single connected component of the qubit graph.");
        }
    }
    return success;
}

//! The main entry function of this library.
//!
//! This method primarily dispatches the proper implementation of the algorithm
//! where some parameters/behaviours have been fixed at compile time.
//!
//! In terms of dispatch, there are three dynamically-selected classes which
//! are combined, each according to a specific optional parameter.
//!   * a domain_handler, described in embedding_problem.hpp, manages
//!     constraints of the form "variable a's chain must be a subset of..."
//!   * a fixed_handler, described in embedding_problem.hpp, manages
//!     contstraints of the form "variable a's chain must be exactly..."
//!   * a pathfinder, described in pathfinder.hpp, which come in two flavors,
//!     serial and parallel
//! The optional parameters themselves can be found in util.hpp.  Respectively,
//! the controlling options for the above are restrict_chains, fixed_chains,
//! and threads.

inline int findEmbedding(::graph::input_graph &var_g, ::graph::input_graph &qubit_g, optional_parameters &params_,
                         std::vector<std::vector<int>> &chains) {
    if (params_.threads > 1) {
        if (params_.restrict_chains.empty()) {
            using domain_handler = domain_handler_universe;
            if (params_.fixed_chains.empty()) {
                return find_embedding_execute<
                        pathfinder_parallel<embedding_problem<domain_handler, fixed_handler_none>>>(var_g, qubit_g,
                                                                                                    params_, chains);
            } else {
                return find_embedding_execute<
                        pathfinder_parallel<embedding_problem<domain_handler, fixed_handler_hival>>>(var_g, qubit_g,
                                                                                                     params_, chains);
            }
        } else {
            using domain_handler = domain_handler_masked;
            if (params_.fixed_chains.empty()) {
                return find_embedding_execute<
                        pathfinder_parallel<embedding_problem<domain_handler, fixed_handler_none>>>(var_g, qubit_g,
                                                                                                    params_, chains);
            } else {
                return find_embedding_execute<
                        pathfinder_parallel<embedding_problem<domain_handler, fixed_handler_hival>>>(var_g, qubit_g,
                                                                                                     params_, chains);
            }
        }
    } else {
        if (params_.restrict_chains.empty()) {
            using domain_handler = domain_handler_universe;
            if (params_.fixed_chains.empty()) {
                return find_embedding_execute<pathfinder_serial<embedding_problem<domain_handler, fixed_handler_none>>>(
                        var_g, qubit_g, params_, chains);
            } else {
                return find_embedding_execute<
                        pathfinder_serial<embedding_problem<domain_handler, fixed_handler_hival>>>(var_g, qubit_g,
                                                                                                   params_, chains);
            }
        } else {
            using domain_handler = domain_handler_masked;
            if (params_.fixed_chains.empty()) {
                return find_embedding_execute<pathfinder_serial<embedding_problem<domain_handler, fixed_handler_none>>>(
                        var_g, qubit_g, params_, chains);
            } else {
                return find_embedding_execute<
                        pathfinder_serial<embedding_problem<domain_handler, fixed_handler_hival>>>(var_g, qubit_g,
                                                                                                   params_, chains);
            }
        }
    }
}
}
