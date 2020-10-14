# Simple 1-qubit demonstration of the Quatum Imaginary Time Evolution/QLanczos algorithm
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
# We just need 1 qubit
qiteBuffer = xacc.qalloc(1)
qiteResult = qite.execute(qiteBuffer)

# Create the QLanczos algorithm
qLanczos = xacc.getAlgorithm('QLanczos', {
                        'accelerator': qpu,
                        'observable': ham,
                        'step-size': stepSize,
                        'steps': nbSteps
                        })
qLanczosBuffer = xacc.qalloc(1)
qLanczosResult = qLanczos.execute(qLanczosBuffer)

# Plot the energies
qiteEnergies = qiteBuffer.getInformation('exp-vals')
qLanczosEnergies = qLanczosBuffer.getInformation('exp-vals')

# Reproduce Fig. 2(e) of https://www.nature.com/articles/s41567-019-0704-4
plt.plot(np.arange(0, nbSteps + 1) * stepSize, qiteEnergies, 'ro-', label = 'QITE')
plt.plot(np.arange(0, (nbSteps + 1)//2) * 2 * stepSize, qLanczosEnergies, 'k*-', label = 'QLanczos')
plt.legend()
plt.grid()
plt.show()

# QITE circuit to evolve the state in *imaginary* time.
# qiteCircuit = qiteBuffer.getInformation('qasm')
# print(qiteCircuit)