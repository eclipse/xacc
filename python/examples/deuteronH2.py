import xacc,sys, numpy as np

# Get access to the desired QPU and
# allocate some qubits to run on
qpu = xacc.getAccelerator('tnqvm')
buffer = qpu.createBuffer('q', 2)

# Construct the Hamiltonian as an XACC-VQE PauliOperator
ham = xacc.getObservable('pauli', '5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1')

# Define the ansatz and decorate it to indicate
# you'd like to run VQE
@xacc.qpu(algo='vqe',accelerator=qpu, observable=ham)
def ansatz(buffer, t0):
    X(0)
    Ry(t0, 1)
    CNOT(1, 0)

ansatz(buffer)

print('Energy = ', buffer.getInformation('opt-val'))
print('Opt Angles = ', buffer.getInformation('opt-params'))

# Now lets just do a param sweep
ansatz.modifyAlgorithm('energy')

# Execute, starting at .5
def compute_energy_at_angles(angles):
    ansatz(buffer, *angles)
    return buffer.getInformation('opt-val')
energy = lambda angles : compute_energy_at_angles(angles)

print([energy([a]) for a in np.linspace(-np.pi,np.pi,50)])
