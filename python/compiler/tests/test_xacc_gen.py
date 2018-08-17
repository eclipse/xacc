import xacc
import numpy as np

xacc.Initialize()

@xacc.qpu(accelerator='tnqvm') # or ibm, rigetti, etc...
def foo(t0, t1):
   xacc(uccsd, n_qubits=4, n_electrons=2)
   Measure(0,0)
   return

print([foo(0,t1).getExpectationValueZ() for t1 in np.linspace(-np.pi,np.pi,20)])

xacc.Finalize()