import xacc

xacc.set_verbose(True)
accelerator = xacc.getAccelerator("qpp")
buffer = xacc.qalloc(4)
optimizer = xacc.getOptimizer('nlopt',{'nlopt-optimizer':'l-bfgs'})
pool = "multi-qubit-qaoa"
nLayers = 2
subAlgo_qaoa = "QAOA"
H = xacc.getObservable('pauli', '-5.0 - 0.5 Z0 - 1.0 Z2 + 0.5 Z3 + 1.0 Z0 Z1 + 2.0 Z0 Z2 + 0.5 Z1 Z2 + 2.5 Z2 Z3')
adapt = xacc.getAlgorithm('adapt', {
                        'accelerator': accelerator,
                        'observable': H,
                        'optimizer': optimizer,
                        'pool': pool,
                        'maxiter': nLayers,
                        'sub-algorithm': subAlgo_qaoa
                        })

adapt.execute(buffer)

