import xacc
import numpy as np

xacc.Initialize()

class test(xacc.Accelerator):
    def execute(self, buffer, function):
        print('executing hello world')
      
@xacc.qpu() # or ibm, rigetti, etc...
def foo(theta):
   X(0)
   Ry(theta, 1)
   CNOT(1, 0)
   Measure(0,0)
   return

expVals = [foo(t).getExpectationValueZ() for t in np.linspace(-np.pi,np.pi,10)]
print (expVals)

xacc.Finalize()