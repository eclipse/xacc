import xacc,sys, numpy as np

qpu = xacc.getAccelerator('tnqvm', {'tnqvm_visitor' :' exatn'})
buffer = xacc.qalloc(4)
optimizer = xacc.getOptimizer('nlopt')

mc = xacc.getAlgorithm('mc-vqe', {'accelerator': qpu,
                                  'optimizer': optimizer,
                                  'nChromophores': 4,
                                  'cyclic': True})
mc.execute(buffer)



