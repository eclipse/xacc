%FIND_EMBEDDING Find am embedding of a QUBO/Ising in a graph.
%
%  embeddings = find_embedding(Q, A, params)
%  (can be interrupted by Ctrl-C, will return the best embedding found so far.)
%
%  Attempts to find an embedding of a QUBO in a graph. This function
%  is entirely heuristic: failure to return an embedding does not
%  prove that no embedding exists.
%
%  Input parameters:
%
%    Q: Edge structures of a problem, can be Qubo/Ising. The embedder only cares
%       about the edge structure (i.e. which variables have a nontrivial
%       interactions), not the coefficient values.
%
%    A: Adjacency matrix of the graph, as returned by
%       getChimeraAdjacency() or getHardwareAdjacency().
%
%    params: structure of parameters. Must be a structure.
%
%  Output:
%
%    embeddings: A cell array of embeddings, embeddings{i} is the set of qubits
%                representing logical variable i. The index is 0-based.
%                This embeddings return value can be used in sapiEmbeddingSolver.
%                If the algorithm fails, the output is an empty cell array.
%
%    success: A logical value with 1 indicating that the embedding returned is valid
%
%   parameters for find_embedding_mex:
%
%   max_no_improvement: number of rounds of the algorithm to try from the current
%                       solution with no improvement. Each round consists of an attempt to find an
%                       embedding for each variable of Q such that it is adjacent to all its
%                       neighbours.
%                       (must be an integer >= 0, default = 10)
%
%   random_seed: seed for random number generator that find_embedding_mex uses
%                (must be an integer >= 0, default is randomly set)
%
%   timeout: Algorithm gives up after timeout seconds.
%            (must be a number >= 0, default is approximately 1000 seconds, stored as double)
%
%   tries: The algorithm stops after this number of restart attempts. On D-WAVE 2000Q,
%          each restart takes between 1 and 60 seconds typically.
%          (must be an integer >= 0, default = 10)
%
%   inner_rounds: the algorithm takes at most this many passes between restart attempts;
%                 restart attempts are typically terminated due to max_no_improvement
%                 (must be an integer >= 0, default = effectively infinite)
%
%   chainlength_patience: similar to max_no_improvement, but for the chainlength improvement
%                         passes.
%                         (must be an integer >= 0, default = 2)
%
%   max_fill: until a valid embedding is found, this restricts the the maximum number
%             of variables whose chain may contain a given qubit.
%             (must be an integer >= 0, default = effectively infinite)
%
%   threads: maximum number of threads to use.  note that the parallelization is only
%            advantageous where the expected degree of variables is (significantly?)
%            greater than the number of threads.
%            (must be an integer >= 1, default = 1)
%
%   return_overlap: return an embedding whether or not qubits are used by multiple
%                   variables -- capture both return values to determine whether or
%                   not the returned embedding is valid
%                   (must be a logical 0/1 integer, default = 0)
%
%   skip_initialization: skip the initialization pass -- NOTE: this only works  if the
%                        chains passed in through initial_chains and fixed_chains are
%                        semi-valid.  A semi-valid embedding is a collection of chains
%                        such that every adjacent pair of variables (u,v) has a coupler
%                        (p,q) in the hardware graph where p is in chain(u) and q is in
%                        chain(v).  This can be used on a valid embedding to immediately
%                        skip to the chainlength improvement phase.  Another good source
%                        of semi-valid embeddings is the output of this function with
%                        the return_overlap parameter enabled.
%                        (must be a logical 0/1 integer, default = 0)
%
%         verbose: 0/1/2/3/(4).
%                  (must be an integer [0, 1, 2, 3, (4)], default = 0)
%                  when verbose is 1, the output information will look like:
%
%                    initialized
%                    max qubit fill 3; num maxfull qubits=3
%                    embedding trial 1
%                    max qubit fill 2; num maxfull qubits=21
%                    embedding trial 2
%                    embedding trial 3
%                    embedding trial 4
%                    embedding trial 5
%                    embedding found.
%                    max chain length 4; num max chains=1
%                    reducing chain lengths
%                    max chain length 3; num max chains=5
%
%                  additional verbosity levels will
%                    2: report progress on minor statistics (when searching for an embedding, this is
%                         when the number of maxfull qubits decreases; when improving, this is when the
%                         number of max chains decreases)
%                    3: report before each before each pass -- look here when tweaking `tries`,
%                         `inner_rounds`, and `chainlength_patience`
%                    (4): report additional debugging information.  by default, this package is built
%                         without this functionality -- in the c++ headers, this is controlled by the
%                         CPPDEBUG flag
%
%                  detailed explanation of the output information:
%                    max qubit fill: largest number of variables represented in a qubit
%                    num maxfull: the number of qubits that has max overfill
%                    max chain length: largest number of qubits representing a single variable
%                    num max chains: the number of variables that has max chain size
%
%   initial_chains: A 0-indexed cell array, where chain(i) a matrix whose entries are
%                   the qubits representing variable i.  These chains are inserted
%                   into an embedding before fixed_chains are placed, which occurs
%                   before the initialization pass.  This can be used to restart the
%                   algorithm in a similar state to a previous embedding, for example
%                   to improve chainlength of a valid embedding or to reduce overlap
%                   in a semi-valid embedding previously returned by the algorithm.
%                   (same data format as embeddings output -- empty cells are ignored)
%
%   fixed_chains: A 0-indexed cell array, where chain(i) a matrix whose entries are
%                   the qubits representing variable i.  These chains are inserted into
%                   an embedding before the initialization pass.  As the algorithm
%                   proceeds, these chains are not allowed to change.
%                   (same data format as embeddings output -- empty cells are ignored)
%
%


% Proprietary Information D-Wave Systems Inc.
% Copyright (c) 2015 by D-Wave Systems Inc. All rights reserved.
% Notice this code is licensed to authorized users only under the
% applicable license agreement see eula.txt
% D-Wave Systems Inc., 3033 Beta Ave., Burnaby, BC, V5G 4M9, Canada.
