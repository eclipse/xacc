import xacc
import sys
import numpy as np

# Get access to the desired QPU and
# allocate some qubits to run on
qpu = xacc.getAccelerator('qpp', {'vqe-mode': True})
buffer = xacc.qalloc(4)

nahHamStr = '(0.0816923,0) Z2 Z3 + (0.11747,0) Z1 Z3 + (-0.00944179,0) Z1 Y2 Y3 + (0.101934,0) Z0 Z3 + (0.0229208,0) Y0 Y1 Z2 + (0.158901,0) Z0 Z2 + (0.0229208,0) Z0 X2 X3 + (-0.387818,0) Z3 + (0.0323625,0) X2 X3 + (0.0816923,0) Z0 Z1 + (-0.00944179,0) Z1 X2 X3 + (-0.00944179,0) X0 X1 Z3 + (0.0323625,0) Y2 Y3 + (0.0202421,0) X0 X1 Y2 Y3 + (0.0149387,0) Z0 + (0.0149387,0) Z2 + (-0.387818,0) Z1 + (0.0323625,0) Y0 Y1 + (-0.00944179,0) Y0 Y1 Z3 + (0.0229208,0) X0 X1 Z2 + (0.101934,0) Z1 Z2 + (0.0323625,0) X0 X1 + (0.0229208,0) Z0 Y2 Y3 + (-159.403,0) + (0.0202421,0) Y0 Y1 Y2 Y3 + (0.0202421,0) X0 X1 X2 X3 + (0.0202421,0) Y0 Y1 X2 X3'
ham = xacc.getObservable('pauli', nahHamStr)

@xacc.qpu(algo='vqe', accelerator=qpu, observable=ham)
def ansatz(q, x):
    X(q[0])
    X(q[2])
    ucc3(q, x[0], x[1], x[2])


ansatz(buffer, [0.0, 0.0, 0.0])

print('Energy = ', buffer.getInformation('opt-val'))
print('Opt Angles = ', buffer.getInformation('opt-params'))
