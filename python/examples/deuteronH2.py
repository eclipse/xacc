import xacc,sys, numpy as np

# Get access to the desired QPU and
# allocate some qubits to run on
qpu = xacc.getAccelerator('qpp')
buffer = xacc.qalloc(2)

# Construct the Hamiltonian as an XACC-VQE PauliOperator
ham = xacc.getObservable('pauli', '5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1')

@xacc.qpu(algo='vqe',accelerator=qpu, observable=ham)
def ansatz(q, t0):
    X(q[0])
    Ry(q[1], t0)
    CNOT(q[1], q[0])

ansatz(buffer)


print('Energy = ', buffer.getInformation('opt-val'))
print('Opt Angles = ', buffer.getInformation('opt-params'))


# Define the ansatz and decorate it to indicate
# you'd like to run VQE
xacc.qasm('''.compiler xasm
.circuit ansatz2
.parameters t0
.qbit q
X(q[0]);
Ry(q[1],t0);
CX(q[1],q[0]);
''')
ansatz2 = xacc.getCompiled('ansatz2')
opt = xacc.getOptimizer('nlopt')

# Create the VQE algorithm
vqe = xacc.getAlgorithm('vqe', {
                        'ansatz': ansatz2,
                        'accelerator': qpu,
                        'observable': ham,
                        'optimizer': opt
                        })
vqe.execute(buffer)

energy = lambda angles : vqe.execute(buffer, angles)[0] 

print([energy([a]) for a in np.linspace(-np.pi,np.pi,50)])
