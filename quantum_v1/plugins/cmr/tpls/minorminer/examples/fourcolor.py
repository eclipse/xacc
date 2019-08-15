"""
This file contains a worked example of using minorminer to embed
a structured problem, where that structure can be exploited for
speed.  Additionally, we demonstrate the use of a few optional
parameters, including fixed chains.

The structured problem, 4-coloring, is to assign one of 4 colors
to each node in a graph, such that no two adjacent nodes have the
same color.  The QUBO structure for this problem is described in
`graph_coloring_qubo`.

The key observation is that the variables of this QUBO have the
form (v,c) where v is a node in the original graph and c is a
color; that edges ((v1,c1), (v2,c2)) have either v1=v2 or c1=c2.
If we take a quotient of this qubo, identifying variables which
have the same node label, we get the original graph back.

Next, we describe a quotient of a Chimera graph, implemented in
`chimera_blocks` and `chimera_block_quotient`.  The so-called
"chimera index" is a 4-tuple of the form (x,y,u,k) where
0 <= k < 4 and 0 <= u < 2.  We take a quotient of this graph,
identifying nodes which agree in the first three terms, (x,y,u).

After taking these quotients, our problem is simple.  We need
to embed the quotient QUBO into the block quotient of Chimera,
with one additional requirement: the chain for each QUBO node
must contain two blocks of the form (x1, y1, 0) and (x1, y1, 1).
The function `embed_with_quotient` performs the reduction and
supplies those additional requirements to `find_embedding`.


"""

# before we get to anything else, let's get some imports out of the way.
from __future__ import print_function
from past.builtins import xrange
import networkx as nx
import dwave_networkx as dnx
from minorminer import find_embedding
from time import clock


def graph_coloring_qubo(graph, k):
    """
    the QUBO for k-coloring a graph A is as follows:

    variables:
    x_{v,c} = 1 if vertex v of A gets color c; x_{v,c} = 0 otherwise

    constraints:
    1)  each v in A gets exactly one color.
       This constraint is enforced by including the term (\sum_c x_{v,c} - 1)^2 in the QUBO,
       which is minimized when \sum_c x_{v,c} = 1.

    2) If u and v in A are adjacent, then they get different colors.
       This constraint is enforced by including terms x_{v,c} x_{u,c} in the QUBO,
       which is minimzed when at most one of u and v get color c.

    Total QUBO:
    Q(x) = \sum_v (\sum_c x_{v,c} - 1)^2  + \sum_{u ~ v} \sum_c x_{v,c} x_{u,c}

    The graph of interactions for this QUBO consists of cliques of size k (with vertices {x_{v,c} for c = 0,...,k-1})
    plus k disjoint copies of the graph A (one for each color).

    """

    K = nx.complete_graph(k)
    g1 = nx.cartesian_product(nx.create_empty_copy(graph), K)
    g2 = nx.cartesian_product(graph, nx.create_empty_copy(K))
    return nx.compose(g1, g2)


def chimera_blocks(M=16, N=16, L=4):
    """
    Generator for blocks for a chimera block quotient
    """
    for x in xrange(M):
        for y in xrange(N):
            for u in (0, 1):
                yield tuple((x, y, u, k) for k in xrange(L))


def chimera_block_quotient(G, blocks):
    """
    Extract the blocks from a graph, and returns a
    block-quotient graph according to the acceptability
    functions block_good and eblock_good

    Inputs:
        G: a networkx graph
        blocks: a tuple of tuples

    """
    from networkx import Graph
    from itertools import product

    BG = Graph()
    blockid = {}
    for i, b in enumerate(blocks):
        BG.add_node(i)
        if not b or not all(G.has_node(x) for x in b):
            continue
        for q in b:
            if q in blockid:
                raise(RuntimeError, "two blocks overlap")
            blockid[q] = i

    for q, u in blockid.items():
        ublock = blocks[u]
        for p in G[q]:
            if p not in blockid:
                continue
            v = blockid[p]
            if BG.has_edge(u, v) or u == v:
                continue
            vblock = blocks[v]

            if ublock[0][2] == vblock[0][2]:
                block_edges = zip(ublock, vblock)
            else:
                block_edges = product(ublock, vblock)

            if all(G.has_edge(x, y) for x, y in block_edges):
                BG.add_edge(u, v)

    return BG


def embed_with_quotient(source_graph, target_graph, M=16, N=16, L=4, **args):
    """
    Produce an embedding in target_graph suitable to
    check if source_graph is 4-colorable.  More generally,
    if target_graph is a (M,N,L) Chimera subgraph, the
    test is for L-colorability.  This depends heavily upon
    the Chimera structure

    Inputs:
        source_graph, target_graph: networkx graphs
        M,N,L: integers defining the base chimera topology

    Outputs:
        emb: a dictionary mapping (v,i)

    """
    from random import sample
    blocks = list(chimera_blocks(M, N, L))

    BG = chimera_block_quotient(target_graph, blocks)

    ublocks = {block: (block[0][2], i)
               for (i, block) in enumerate(blocks) if BG.has_node(i)}
    source_e = list(source_graph.edges())
    source_n = {x for e in source_e for x in e}
    fabric_e = list(BG.edges())

    # Construct the hints:
    # Goal: each source node must be connected to one horizontal block and one
    #       vertical block (by Chimera structure, each source node will
    #       contain a full (horizontal and vertical) unit cell
    # Construction:
    #   0. for each source node `z`, construct two dummy nodes (z,0) and (z,1)
    #     in both the source and target graphs used by the embedder
    #   1. fix the embedding `(z,u) -> (z,u)` for all dummy nodes
    #   2. for each target block `i` with orientation `u`, and for each source
    #     node `z`, add the target edge `((z,u), i)`
    #   3. for each source node `z` and each orientation `u`, add the source
    #     edge `((z,u), z)`

    fix_chains = {}
    for z in source_n:
        for u in (0, 1):
            source_e.append(((z, u), z))
            fix_chains[z, u] = [(z, u)]
        for u, i in ublocks.values():
            fabric_e.append(((z, u), i))

    # first, grab a few embeddings in the quotient graph. this is super fast
    embs = filter(None, [find_embedding(source_e, fabric_e,
                                        fixed_chains=fix_chains,
                                        chainlength_patience=0,
                                        **args) for _ in range(10)])

    # select the best-looking candidate so far
    emb = min(embs, key=lambda e: sorted((len(c)
                                          for c in e.values()), reverse=True))

    # work down the chainlengths in our embeding
    for _ in range(10):
        emb = find_embedding(source_e, fabric_e,
                             fixed_chains=fix_chains,
                             initial_chains=emb,
                             chainlength_patience=3,
                             skip_initialization=True,
                             **args)

    # next, translate the block-embedding to a qubit-embedding
    newemb = {}
    for v in source_n:
        for k in range(L):
            newemb[v, k] = [blocks[i][k] for i in emb[v]]

    return newemb


if __name__ == "__main__":
    # first, we construct a Chimera graph
    G = dnx.chimera_graph(16)
    labs = nx.get_node_attributes(G, 'chimera_index')
    unlab = {d: i for i, d in labs.items()}
    H = nx.relabel_nodes(G, labs)

    # Now we take a graph to be colored
    graph = nx.generators.triangular_lattice_graph(8, 8)

    # for a basis of comparison, let's try to embed this without the quotient
    graph4 = graph_coloring_qubo(graph, 4)
    c = clock()
    emb = find_embedding(graph4.edges(), H.edges(),
                         verbose=0, chainlength_patience=30)
    try:
        print("raw embedding %d seconds, " % (clock() - c), end='')
        cl = max(len(c) for c in emb.values())
        print("maximum chainlength %d" % cl)
    except:
        print("failure")

    # we embed it using the block quotient,
    c = clock()
    emb = embed_with_quotient(graph, H, 16, 16, 4)
    # and then translate back to integer indices
    print("quotient embedding %d seconds, maximum chainlength %d" % (clock() - c, max(len(c) for c in emb.values())))

    # finally, we translate the embedding back to integer labels
    newemb = {v: [unlab[q] for q in c] for v, c in emb.items()}
    for v in graph:
        for k in range(4):
            print((v, k), newemb[v, k])
