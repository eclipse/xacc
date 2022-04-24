import xacc
xacc.set_verbose(True)

# Get access to the desired QPU and
# allocate some qubits to run on
qpu = xacc.getAccelerator('qpp', {'vqe-mode': True})
buffer = xacc.qalloc(4)

geom = '''
0 1
Na  0.000000   0.0      0.0
H   0.0        0.0  1.914388
symmetry c1
'''
fo = [0, 1, 2, 3, 4, 10, 11, 12, 13, 14]
ao = [5, 9, 15, 19]

H = xacc.getObservable('psi4', {'basis': 'sto-3g', 'geometry': geom,
                                       'frozen-spin-orbitals': fo, 'active-spin-orbitals': ao})

@xacc.qpu(algo='vqe', accelerator=qpu, observable=H)
def ansatz(q, x):
    X(q[0])
    X(q[2])
    ucc1(q, x[0])


ansatz(buffer, 0.0)

print('Energy = ', buffer.getInformation('opt-val'))
print('Opt Angles = ', buffer.getInformation('opt-params'))
