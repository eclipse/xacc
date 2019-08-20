import xacc

# Get reference to the desired QPU
qpu = xacc.getAccelerator('tnqvm')

# Allocate some qubits
qbits = xacc.qalloc(4)

# Create the H2 4 qubit hamiltonian
# note, first run 'python3 -m xacc --benchmark-install chemistry'
hamiltonianService = xacc.serviceRegistry.get_service(
    'hamiltonian_generator', 'xaccKernelH2')
obs = hamiltonianService.generate({})

# Create the UCC-1 ansatz
ansatzService = xacc.serviceRegistry.get_service('ansatz_generator', 'ucc1')
ucc1 = ansatzService.generate({'x-gates': [0, 1]}, 4)

# Create the RDM Purification decorator error mitigation strategy
# and give it the fermionic representation
qpu_decorator = xacc.getAcceleratorDecorator('rdm-purification', qpu)
qpu_decorator.initialize({'fermion-observable': obs})

# Let's use the NLOpt optimizer
opt = xacc.getOptimizer('nlopt')

# Create the VQE algorithm
vqe = xacc.getAlgorithm('vqe', {
                        'ansatz': ucc1,
                        'accelerator': qpu_decorator,
                        'observable': obs,
                        'optimizer': opt
                        })

# Execute
vqe.execute(qbits)

# qbits buffer has all results, print(qbits)
# to see it all
print(qbits.getInformation('opt-val'))