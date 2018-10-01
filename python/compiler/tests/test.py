import xacc
import numpy as np

xacc.Initialize()

qpu = xacc.getAccelerator('tnqvm') # or ibm, rigetti, etc...
buffer = qpu.createBuffer('q',2)

@xacc.qpu(accelerator='tnqvm') 
def foo(buffer, theta):
   X(0)
   Ry(theta, 1)
   CNOT(1, 0)
   Measure(0,0)

# Execute the code on the QPU
foo(buffer, -3.1415936)
print(foo.nParameters())

# Check into the results
print(buffer.getExpectationValueZ())
#print(buffer.getMeasurementCounts())

xacc.Finalize()