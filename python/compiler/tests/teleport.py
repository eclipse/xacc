import xacc

xacc.Initialize()

# Create the source code
@xacc.qpu(accelerator='tnqvm')
def teleport(): 
   X(0)
   H(1)
   CNOT(1,2)
   CNOT(0,1)
   CNOT(1,2)
   CNOT(2,0)
   Measure(2, 0)
   return

results = teleport()

# Display the results
print(results.getMeasurementStrings())

# Finalize the framework
xacc.Finalize()