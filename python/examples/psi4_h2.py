import xacc

# Get access to the desired QPU and
# allocate some qubits to run on
qpu = xacc.getAccelerator('qpp', {'vqe-mode': True})
buffer = xacc.qalloc(4)

geom = '''
0 1
H  0.000000   0.0      0.0
H   0.0        0.0  .7474
symmetry c1
'''
H = xacc.getObservable('psi4', {'basis': 'sto-3g', 'geometry': geom})

@xacc.qpu(algo='vqe', accelerator=qpu, observable=H)
def ansatz(q, x):
    X(q[0])
    X(q[2])
    ucc1(q, x[0])


ansatz(buffer, 0.0)

print('Energy = ', buffer.getInformation('opt-val'))
print('Opt Angles = ', buffer.getInformation('opt-params'))
