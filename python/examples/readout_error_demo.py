import xacc, numpy as np
from xacc import PauliOperator

nAngles = 10

# Get the local-ibm and tnqvm accelerators
# and allocate some qubits for execution on each
qpu = xacc.getAccelerator('local-ibm', {'shots':8192, 'global-p10':.1,'global-p01':.01})
tnqvm = xacc.getAccelerator('tnqvm')
buffer = xacc.qalloc(2)
tnqvmBuffer = xacc.qalloc(2)

# Turn on readout error correction by decorating
# the local-ibm accelerator
qpu = xacc.getAcceleratorDecorator('ro-error', qpu)

# Construct the Hamiltonian
ham = PauliOperator(5.906709445) + \
    PauliOperator({0: 'X', 1: 'X'}, -2.1433) + \
    PauliOperator({0: 'Y', 1: 'Y'}, -2.1433) + \
    PauliOperator({0: 'Z'}, .21829) + \
    PauliOperator({1: 'Z'}, -6.125)

# Define the ansatz and decorate it to indicate
# you'd like to run VQE
@xacc.qpu(algo='energy', accelerator=qpu, observable=ham)
def ansatz(buffer, t0):
    X(buffer[0])
    Ry(buffer[1], t0)
    CNOT(buffer[1], buffer[0])

# Execute angle sweep on Local-IBM
[ansatz(buffer, t) for t in np.linspace(-np.pi,np.pi,nAngles)]

# Get theoretical values by overriding the
# accelerator with TNQVM
print("Starting TNQVM")
ansatz.overrideAccelerator(tnqvm)
[ansatz(tnqvmBuffer, t) for t in np.linspace(-np.pi,np.pi,nAngles)]

# ansatz(xacc.qalloc(2), 0.0)

# Get Theory, Raw, and ROError energies in separate lists
ro_energies, raw_energies, tnqvm_energies = [[] for i in range(3)]
uniqueParams = buffer.getAllUnique('parameters')
for p in uniqueParams:
    children = buffer.getChildren('parameters', p)
    tChildren = tnqvmBuffer.getChildren('parameters',p)
    re, ra, tn = [0.0 for i in range(3)]
    # print(children[0])
    for c in children:
        coeff = c.getInformation('coefficient')
        re += coeff * c.getInformation('ro-fixed-exp-val-z')
        ra += coeff * c.getInformation('exp-val-z')
    ro_energies.append(re)
    raw_energies.append(ra)
    for c in tChildren:
        tn += c.getInformation('coefficient')*c.getInformation('exp-val-z')
    tnqvm_energies.append(tn)

for i,t in enumerate(np.linspace(-np.pi,np.pi,nAngles)):
    print(t, tnqvm_energies[i], raw_energies[i], ro_energies[i])
