import xacc

qpu = xacc.getAccelerator('cirq')

xacc.qasm('''.compiler xasm
.circuit bell
.qbit q
H(q[0]);
CX(q[0],q[1]);
Measure(q[0]);
Measure(q[1]);
''')
bell = xacc.getCompiled('bell')

q = xacc.qalloc(2)
qpu.execute(q, bell)

print(q)
