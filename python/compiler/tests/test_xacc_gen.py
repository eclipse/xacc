import xacc
import numpy as np

xacc.Initialize()
qpu = xacc.getAccelerator('tnqvm')
qbits = qpu.createBuffer('q',4)

@xacc.qpu(accelerator=qpu) # or ibm, rigetti, etc...
def foo(buffer, t0, t1):
   xacc(uccsd, n_qubits=4, n_electrons=2)
   Measure(0,0)


expVals = []
def storeExpVal(qbits,t1):
   foo(qbits,0,t1)
   expVals.append(qbits.getExpectationValueZ())
   val= qbits.getExpectationValueZ()
   qbits.resetBuffer()
   return val
   
print([storeExpVal(qbits, t1) for t1 in np.linspace(-np.pi,np.pi,20)])

xacc.Finalize()