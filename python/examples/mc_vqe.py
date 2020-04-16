import xacc

# Get access to the desired QPU and
# allocate some qubits to run on
qpu = xacc.getAccelerator('tnqvm', {'vqe-mode': True})
optimizer = xacc.getOptimizer('nlopt')
buffer = xacc.qalloc(18)


mc = xacc.getAlgorithm('mc-vqe', {'accelerator': qpu,
                                  'optimizer': optimizer,
                                  'nChromophores': 18,
                                  'isCyclic': True})
# execute
mc.execute(buffer)
