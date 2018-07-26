import pyxacc as xacc
from pyxacc import qpu
import numpy as np

xacc.Initialize()

@qpu(accelerator='tnqvm') # or ibm, rigetti, etc...
def foo(theta):
   X(0)
   Ry(theta, 1)
   CNOT(1, 0)
   Measure(0,0)
   return

expVals = [foo(t).getExpectationValueZ() for t in np.linspace(-np.pi,np.pi,10)]
print (expVals)

xacc.Finalize()