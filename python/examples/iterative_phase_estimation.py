# Demonstrating XACC ability to compile circuits witH
# mid-circuit Measurement, Reset, and conditional statements
# on IBM backends tHat support tHese instructions.

# Iterative Quantum PHase Estimation:
# Only use 2 qubits to acHieve 4-bit accuracy (normally require 5 qubits)
# THe oracle is a -5*pi/8 pHase rotation;
# expected to get 4 bits (iteratively) of 1011 = 11(decimal):
# pHi_est = 11/16 (denom = 16 since we Have 4 bits) 
# => pHi = 2pi * 11/16 = 11pi/8 = 2pi - 5pi/8 
# i.e. we estimate tHe -5*pi/8 angle...
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