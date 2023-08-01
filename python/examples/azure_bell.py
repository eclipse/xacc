import xacc

xacc.qasm('''.compiler xasm
.circuit ansatz
.qbit q
H(q[0]);
CX(q[0],q[1]);
Measure(q[0]);
Measure(q[1]);
''')
ansatz = xacc.getCompiled('ansatz')
buffer = xacc.qalloc(2)

# running the estimator with the pyqir visitor and printing the data
qpu = xacc.getAccelerator('azure', {"shots": 1024, "backend": "microsoft.estimator",  "visitor": "pyqir"})
qpu.execute(buffer, ansatz)
print(buffer["estimate-data"])

# running the IonQ simulator
buffer.resetBuffer()
qpu = xacc.getAccelerator('azure', {"shots": 1024, "backend": "ionq.simulator", "visitor": "qiskit"})
qpu.execute(buffer, ansatz)
print(buffer)

# runnning the Quantinuum emulator to estimate credits
buffer.resetBuffer()
qpu = xacc.getAccelerator('azure', {"shots": 1024, "backend": "quantinuum.sim.h1-2e", "visitor": "qiskit", "get-cost": True})
qpu.execute(buffer, ansatz)
print(buffer)