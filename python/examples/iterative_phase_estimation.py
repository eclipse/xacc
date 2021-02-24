# Demonstrating XACC ability to compile circuits with
# mid-circuit Measurement, Reset, and conditional statements
# on IBM backends that support tHese instructions.

# IMPORTANT NOTES: IBM hardware backends don't support conditional 
# statements ('bfunc' instruction) at the moment.
# Hence, this can only be executed on the simulator.
# Ref: https://arxiv.org/pdf/2102.01682.pdf
# The conditional statements executed in the paper seem 
# not to be available to the public yet.

# Iterative Quantum Phase Estimation:
# Only use 2 qubits to achieve 4-bit accuracy (normally require 5 qubits)
# The oracle is a -5*pi/8 pHase rotation;
# expected to get 4 bits (iteratively) of 1011 = 11(decimal):
# phi_est = 11/16 (denom = 16 since we have 4 bits) 
# => phi = 2pi * 11/16 = 11pi/8 = 2pi - 5pi/8 
# i.e. we estimate the -5*pi/8 angle...
import xacc
xacc.qasm('''
.compiler xasm
.circuit iterative_qpe
.qbit q
H(q[0]);
X(q[1]);
// Prepare the state:
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
H(q[0]);
// Measure and reset
Measure(q[0], c[0]);
Reset(q[0]);
H(q[0]);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
// Conditional rotation
if(c[0]) {
    Rz(q[0], -pi/2);
}
H(q[0]);
Measure(q[0], c[1]);
Reset(q[0]);
H(q[0]);
CPhase(q[0], q[1], -5*pi/8);
CPhase(q[0], q[1], -5*pi/8);
if(c[0]) {
    Rz(q[0], -pi/4);
}
if(c[1]) {
    Rz(q[0], -pi/2);
}
H(q[0]);
Measure(q[0], c[2]);
Reset(q[0]);
H(q[0]);
CPhase(q[0], q[1], -5*pi/8);
if(c[0]) {
    Rz(q[0], -pi/8);
}
if(c[1]) {
    Rz(q[0], -pi/4);
}
if(c[2]) {
    Rz(q[0], -pi/2);
}
H(q[0]);
Measure(q[0], c[3]);
''')

f = xacc.getCompiled('iterative_qpe')
print(f.toString())
# qpu = xacc.getAccelerator('qpp', {'shots':1024})
qpu = xacc.getAccelerator('aer', {'shots':1024})
# With noise
# Note: IBMQ doesn't support bfunc (conditional) instruction atm,
# hence, we can only do simulation of the QObj.
# qpu = xacc.getAccelerator('aer:ibmq_manhattan', {'shots':1024})
q = xacc.qalloc(2)
qpu.execute(q, f)
print(q)