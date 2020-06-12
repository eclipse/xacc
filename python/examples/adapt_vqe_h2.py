import xacc

qpu = xacc.getAccelerator('tnqvm')
optimizer = xacc.getOptimizer('nlopt',{'nlopt-optimizer':'l-bfgs'})
buffer = xacc.qalloc(4)

geom = '''
H  0.000000   0.0      0.0
H   0.0        0.0  .7474
'''
H = xacc.getObservable('pyscf', {'basis': 'sto-3g', 'geometry': geom})
print(H.toString())

adapt = xacc.getAlgorithm('adapt-vqe', {'accelerator': qpu,
                                  'optimizer': optimizer,
                                  'observable': H,
                                  'nElectrons': 2,
                                  'gradient-strategy': 'parameter-shift-gradient',
                                  'print-operators': True,
                                  'threshold': 1.0e-2,
                                  'print-threshold': 1.0e-10,
                                  'maxiter': 2,
                                  'pool': "singlet-adapted-uccsd"})
# execute
adapt.execute(buffer)
