import PyXACC as xacc

# Create the source code
src = """__qpu__ teleport(qbit qreg){
cbit creg[3];
// qubit0 to be teleported
X(qreg[0]);
// Bell channel set up by qreg[1] and qreg[2]
H(qreg[1]);
CNOT(qreg[1],qreg[2]);
// Alice Bell measurement
CNOT(qreg[0],qreg[1]);
H(qreg[0]);
creg[0] = MeasZ(qreg[0]);
creg[1] = MeasZ(qreg[1]);
// Bob's operation base on Alice's measurement outcome
if (creg[0] == 1) Z(qreg[2]);
if (creg[1] == 1) X(qreg[2]);
}
"""

# Initialize the framework
xacc.Initialize()

# Get reference to our simple accelerator
qpu = xacc.getAccelerator('simple')

# Create some qubits to operate on
qreg = qpu.createBuffer('qreg',3)

# Create the program and build it
p = xacc.Program(qpu, src)
p.build()

# Get the teleport kernel
k = p.getKernel('teleport')

# Execute!
k.execute(qreg, [])

# Display the results to standard out
qreg.printBuffer()

# Finalize the framework
xacc.Finalize()

