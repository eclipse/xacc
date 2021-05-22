import xacc

qpu = xacc.getAccelerator(
    'ibm', {'shots': 256, 'backend': 'lowest-queue-count', 'n-qubits': 5, 'check-jobs-limit': True})

print(qpu.getProperties()["total-json"])

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
