import pyxacc as xacc
import numpy as np

# Define the source code
src = """__qpu__ prepare_ansatz(qbit qreg, double theta){
	Rx(qreg[0], 3.1415926);
	Ry(qreg[1], 1.57079);
	Rx(qreg[0], 7.8539752);
	CNOT(qreg[1], qreg[0]);
	Rz(qreg[0], theta);
	CNOT(qreg[1], qreg[0]);
	Ry(qreg[1], 7.8539752);
	Rx(qreg[0], 1.57079);
}
// measure the 1st term of Hamiltonian on the ansatz
__qpu__ term0(qbit qreg, double theta){
	prepare_ansatz(qreg, theta);
	cbit creg[1];
	creg[0] = MeasZ(qreg[0]);
}
__qpu__ term1(qbit qreg, double theta){
	prepare_ansatz(qreg, theta);
	cbit creg[1];
	creg[0] = MeasZ(qreg[1]);
}
__qpu__ term2(qbit qreg, double theta){
	prepare_ansatz(qreg, theta);
	cbit creg[2];
	creg[0] = MeasZ(qreg[0]);
	creg[1] = MeasZ(qreg[1]);
}"""

# Initialize the framework
xacc.Initialize()

# Get reference to our tensor network simulator
qpu = xacc.getAccelerator('tnqvm')

# Create a buffer of qubits
qreg = qpu.createBuffer("qreg", 2)

# Create the XACC Program and build it
program = xacc.Program(qpu,src)
program.build()

# Get the compiled kernels
kernels = program.getKernels()

file = open('out.csv', "w")
file.write('theta, Z0, Z1, Z0Z1\n')

# Loop over -pi to pi
for theta in np.linspace(-np.pi, np.pi, 100):
    file.write(str(theta))
    
    # Execute each kernel
    for k in kernels[1:]:
        file.write(', ')
        
        # Execute with the given runtime parameter
        k.execute(qreg, [xacc.InstructionParameter(theta)])
        
        # Get the expectation value
        e = qreg.getExpectationValueZ()
        
        # Reset the qubit buffer
        qreg.resetBuffer()
        file.write(str(e))
    file.write('\n')
    file.flush()
file.close()

# Finalize the frameworks
xacc.Finalize()

