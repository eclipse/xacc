import xacc,sys, numpy as np

xacc.set_verbose(True)
# Get access to the desired QPU and
# allocate some qubits to run on
qpu = xacc.getAccelerator('qsim')
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

nbSteps = 1
buffer = xacc.qalloc(3)
opt = xacc.getOptimizer('nlopt')
# Create QAOA
qaoa = xacc.getAlgorithm('maxcut-qaoa', {
                        'accelerator': qpu,
                        'graph': graph,
                        'optimizer': opt,
                        'steps': nbSteps,
                        'shuffle-terms': True
                        })
result = qaoa.execute(buffer)
print("Max-cut val:", buffer["opt-val"])