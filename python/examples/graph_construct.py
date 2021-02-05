import xacc

# Simple example demonstrating the construction of an XACC Graph
xacc.set_verbose(True)
graph = xacc.getGraph("boost-digraph")

# Manual construction:
# Triangular graph
graph.addVertex({'name': 'v1'})
graph.addVertex({'name': 'v2'})
graph.addVertex({'name': 'v3'})

graph.addEdge(0, 1)
graph.addEdge(0, 2)
graph.addEdge(1, 2)

print(graph)


# NetworkX conversion
from networkx import fast_gnp_random_graph, draw
prob_edge = 0.5
n_nodes = 8
# Create a random networkx graph
networkx_graph = fast_gnp_random_graph(n_nodes, prob_edge)

# Convert to XACC graph:
xacc_graph = xacc.getGraph("boost-digraph")
# Add nodes
for node in networkx_graph.nodes:
    xacc_graph.addVertex({'name': str(node)})
# Add edges
for (from_node, to_node) in networkx_graph.edges:
    xacc_graph.addEdge(from_node, to_node)

print(xacc_graph)
# Plot the networkx to check.
draw(networkx_graph, with_labels=True)
import matplotlib.pyplot as plt
plt.savefig("graph.png")