import xacc,sys, numpy as np

xacc.set_verbose(True)
# Get access to the desired QPU and
# allocate some qubits to run on
qpu = xacc.getAccelerator('qpp')

# Construct the Hamiltonian as an XACC PauliOperator
# This Hamiltonian corresponds to the QUBO problem:
# y = -5x_1 -3x_2 -8x_3 -6x_4 + 4x_1x_2 + 8x_1x_3 + 2x_2x_3 + 10x_3x_4
ham = xacc.getObservable('pauli', '-5.0 - 0.5 Z0 - 1.0 Z2 + 0.5 Z3 + 1.0 Z0 Z1 + 2.0 Z0 Z2 + 0.5 Z1 Z2 + 2.5 Z2 Z3')

# We need 4 qubits
buffer = xacc.qalloc(4)

# There are 7 gamma terms (non-identity) in the cost Hamiltonian 
# and 4 beta terms for mixer Hamiltonian
nbParamsPerStep = 7 + 4

# The number of steps (often referred to as 'p' parameter): 
# alternating layers of mixer and cost Hamiltonian exponential.
nbSteps = 4

# Total number of params
nbTotalParams = nbParamsPerStep * nbSteps

# Init params randomly: 
initParams = np.random.rand(nbTotalParams)

# The optimizer: nlopt
opt = xacc.getOptimizer('nlopt', { 'initial-parameters': initParams , 'nlopt-optimizer':'l-bfgs'} )

# Create the QAOA algorithm
qaoa = xacc.getAlgorithm('QAOA', {
                        'accelerator': qpu,
                        'observable': ham,
                        'optimizer': opt,
                        'steps': nbSteps,
                        'gradient_strategy': 'parameter-shift'
                        })

result = qaoa.execute(buffer)

# Expected result: ~ -11
# Ref: https://docs.entropicalabs.io/qaoa/notebooks/6_solvingqubowithqaoa
print('Min QUBO value = ', buffer.getInformation('opt-val'))
