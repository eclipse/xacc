import xacc

qpu = xacc.getAccelerator('aer')
qbits = xacc.qalloc(2)

# Create a bell state program with too many cnots
xacc.qasm('''
.compiler xasm
.circuit foo
.parameters x,y,z
.qbit q
H(q[0]);
CX(q[0], q[1]);
CX(q[0], q[1]);
CX(q[0], q[1]);
Measure(q[0]);
Measure(q[1]);
''')
f = xacc.getCompiled('foo')

# Run the python contributed IRTransformation that uses qiskit
optimizer = xacc.getIRTransformation('qiskit-cx-cancellation')
optimizer.apply(f, qpu, {})

# should have 4 instructions, not 6
assert(4 == f.nInstructions())
print(f.toString())

qbits = xacc.qalloc(2)
qpu.execute(qbits, f)

print(qbits)
