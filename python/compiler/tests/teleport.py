import xacc

xacc.Initialize()

qpu = xacc.getAccelerator('local-ibm')
qubits = qpu.createBuffer('q',3)

# Create the source code
@xacc.qpu(accelerator=qpu)
def teleport(qubits): 
   X(0)
   H(1)
   CNOT(1,2)
   CNOT(0,1)
   CNOT(1,2)
   CNOT(2,0)
   Measure(2, 0)

teleport(qubits)

# Display the results
print(qubits.getMeasurementCounts())

# Finalize the framework
xacc.Finalize()