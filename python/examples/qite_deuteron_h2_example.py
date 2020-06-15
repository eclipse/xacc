# Simple 2-qubit demonstration of the Quatum Imaginary Time Evolution algorithm
# for Deuteron H2 Hamiltonian
# Reference: https://arxiv.org/pdf/1912.06226.pdf
# Expected minimum value: -1.74886
import xacc,sys, numpy as np
import matplotlib.pyplot as plt

# Get access to the desired QPU and
# allocate some qubits to run on
qpu = xacc.getAccelerator('qpp')

# Construct the H2 Hamiltonian
ham = xacc.getObservable('pauli', '5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1')

xacc.qasm('''.compiler xasm
.circuit prep
.qbit q
X(q[0]);
''')
prep_circuit = xacc.getCompiled('prep')

# We need 2 qubits for this case (H2)
buffer = xacc.qalloc(2)

# Horizontal axis: 0 -> 0.3 (step = 0.05)
# The number of Trotter steps 
nbSteps = 6

# The Trotter step size
stepSize = 0.05

# Create the QITE algorithm
qite = xacc.getAlgorithm('qite', {
                        'accelerator': qpu,
                        'observable': ham,
                        'step-size': stepSize,
                        'steps': nbSteps,
                        'ansatz': prep_circuit
                        })

result = qite.execute(buffer)

# Expected result: ~ -1.74886
print('Min energy value = ', buffer.getInformation('opt-val'))
E = buffer.getInformation('exp-vals')
# Reproduce Fig. 3(a) of https://arxiv.org/pdf/1912.06226.pdf
plt.plot(np.arange(0, nbSteps + 1) * stepSize, E, 'ro-', label = 'XACC QITE')
plt.ylim((-2.0, -0.25))
plt.yticks(np.arange(-2.0, -0.25, step=0.25)) 
plt.grid()
plt.show()