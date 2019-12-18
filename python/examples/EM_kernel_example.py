import xacc
import numpy as np

qpu = xacc.getAccelerator('aer', {'readout_error': True, 'shots': 4096,\
                                  'backend':'ibmq_20_tokyo'})
qbits = xacc.qalloc(3)
layout = np.array([1,2,3], dtype=np.uintp)
decorator = xacc.getAcceleratorDecorator('assignment-error-kernel', qpu, {'gen-kernel': True, 'layout': layout})

xacc.qasm('''
.compiler xasm
.circuit foo
.qbit q
H(q[0]);
H(q[1]);
CNOT(q[0], q[2]);
CNOT(q[1], q[2]);
Measure(q[0]);
Measure(q[1]);
Measure(q[2]);
''')

ansatz = xacc.getCompiled('foo')

decorator.execute(qbits, ansatz)

print(qbits)

kernel = qbits.getInformation("error-kernel")
original = qbits.getInformation("unmitigated-counts")
mitigated = qbits.getMeasurementCounts()
kernel = np.array(kernel)
kernel = kernel.reshape(8, 8)
print(kernel)

print(mitigated)

print(original)
