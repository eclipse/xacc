# Simple 1-qubit demonstration of the Quatum Imaginary Time Evolution algorithm
# Reference: https://www.nature.com/articles/s41567-019-0704-4
# Target H = 1/sqrt(2)(X + Z)
# Expected minimum value: -1.0
import xacc,sys, numpy as np
import matplotlib.pyplot as plt

# Get access to the desired QPU and
# allocate some qubits to run on
qpu = xacc.getAccelerator('qpp')

# Construct the Hamiltonian as an XACC PauliOperator
ham = xacc.getObservable('pauli', '0.70710678118 X0 + 0.70710678118 Z0')

# We just need 1 qubit
buffer = xacc.qalloc(1)

# See Fig. 2 (e) of https://www.nature.com/articles/s41567-019-0704-4
# Horizontal axis: 0 -> 2.5
# The number of Trotter steps 
nbSteps = 25

# The Trotter step size
stepSize = 0.1

# Create the QITE algorithm
qite = xacc.getAlgorithm('qite', {
                        'accelerator': qpu,
                        'observable': ham,
                        'step-size': stepSize,
                        'steps': nbSteps
                        })

result = qite.execute(buffer)

# Expected result: ~ -1
print('Min energy value = ', buffer.getInformation('opt-val'))
E = buffer.getInformation('exp-vals')
# Reproduce Fig. 2(e) of https://www.nature.com/articles/s41567-019-0704-4
plt.plot(np.arange(0, nbSteps + 1) * stepSize, E, 'ro', label = 'XACC QITE')
plt.grid()
plt.show()