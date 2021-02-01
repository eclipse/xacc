import xacc,sys, numpy as np

xacc.set_verbose(True)
# Get access to the desired QPU and
# allocate some qubits to run on
qpu = xacc.getAccelerator('qsim')
graph = xacc.getGraph("boost-digraph")
nbNodes = 3
random_graph = graph.gen_random_graph(nbNodes, 3.0 / nbNodes)
print(random_graph)
nbSteps = 1
buffer = xacc.qalloc(nbNodes)
opt = xacc.getOptimizer('nlopt')
# Create QAOA
qaoa = xacc.getAlgorithm('QAOA', {
                        'accelerator': qpu,
                        'graph': random_graph,
                        'optimizer': opt,
                        'steps': nbSteps,
                        'parameter-scheme': 'Standard'
                        })
result = qaoa.execute(buffer)
print("Max-cut val:", buffer["opt-val"])