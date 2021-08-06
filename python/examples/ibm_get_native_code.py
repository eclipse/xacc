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

qpu = xacc.getAccelerator('ibm:ibmq_manhattan')
print('HOWDY:\n', qpu.getNativeCode(f))