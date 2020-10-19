from pyquil.gates import CNOT, H, MEASURE
from pyquil import get_qc, Program
import xacc
import qiskit

circ = qiskit.QuantumCircuit(2, 2)
circ.h(0)
circ.cx(0, 1)
circ.measure([0, 1], [0, 1])

qpu = xacc.getAccelerator('qpp', {'shots': 1024})

q = xacc.qalloc(2)
qpu.execute(q, circ)

print(q)


p = Program()
p += H(0)
p += CNOT(0, 1)
ro = p.declare('ro', 'BIT', 2)
p += MEASURE(0, ro[0])
p += MEASURE(1, ro[1])

r = xacc.qalloc(2)
qpu.execute(r, p)

print(r)

provider = xacc.getIRProvider('quantum')
comp = provider.createComposite('foo')
comp.addInstructions([provider.createInstruction('H', [0]), provider.createInstruction(
    'CNOT', [0, 1]), provider.createInstruction('Measure', [0]), provider.createInstruction('Measure', [1])])

s = xacc.qalloc(2)
qpu.execute(s, comp)
print(s)

x = xacc.qalloc(2)
qpu.execute(x, [circ, circ, circ])

print(x)