import xacc

noiseModelJson ='{"errors": [{"type": "qerror", "operations": ["u1"], "instructions": [[{"name": "x", "qubits": [0]}], [{"name": "y", "qubits": [0]}], [{"name": "z", "qubits": [0]}], [{"name": "id", "qubits": [0]}]], "probabilities": [0.00025, 0.00025, 0.00025, 0.99925]}, {"type": "qerror", "operations": ["u2"], "instructions": [[{"name": "x", "qubits": [0]}], [{"name": "y", "qubits": [0]}], [{"name": "z", "qubits": [0]}], [{"name": "id", "qubits": [0]}]], "probabilities": [0.00025, 0.00025, 0.00025, 0.99925]}, {"type": "qerror", "operations": ["u3"], "instructions": [[{"name": "x", "qubits": [0]}], [{"name": "y", "qubits": [0]}], [{"name": "z", "qubits": [0]}], [{"name": "id", "qubits": [0]}]], "probabilities": [0.00025, 0.00025, 0.00025, 0.99925]}], "x90_gates": []}'

qpu = xacc.getAccelerator('aer', {'noise-model':noiseModelJson, 'shots':4096})
qpu = xacc.getAcceleratorDecorator('mitiq', qpu)

q = xacc.qalloc(1)

xacc.qasm('''.compiler xasm
.circuit foo
.parameters x
.qbit q
for (int i = 0; i < 10; i++) {
  X(q[0]);
}
Measure(q[0]);
''')
foo = xacc.getCompiled('foo')

qpu.execute(q, foo)

print(q)
print(q.getExpectationValueZ())
