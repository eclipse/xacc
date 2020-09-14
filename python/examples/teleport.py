import xacc 

qpu = xacc.getAccelerator('aer', {'shots':1024})

xacc.qasm('''
.compiler xasm
.circuit teleport
.qbit q
X(q[0]);
// Bell channel setup
H(q[1]);
CX(q[1], q[2]);
// Alice Bell measurement
CX(q[0], q[1]);
H(q[0]);
Measure(q[0]);
Measure(q[1]);
// Correction
if (q[0])
{
    Z(q[2]);
}
if (q[1])
{
    X(q[2]);
}
// Measure teleported qubit
Measure(q[2]);
''')

teleport = xacc.getCompiled('teleport')

q = xacc.qalloc(3)
qq = xacc.qalloc()
qpu.execute(q, teleport)

print(q)