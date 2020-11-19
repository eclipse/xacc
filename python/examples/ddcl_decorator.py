import xacc

# Get the QPU and allocate a single qubit
qpu = xacc.getAccelerator('qpp')
qbits = xacc.qalloc(1)

@xacc.qpu(algo='ddcl', accelerator=qpu, optimizer='mlpack', target_dist=[.5,.5], loss='js', gradient='js-parameter-shift')
def f(qbits, x, y, z):
    Ry(qbits[0], x)
    Ry(qbits[0], y)
    Ry(qbits[0], z)

f(qbits, 0, 0, 0)

print(qbits.getInformation('opt-val'))

params = qbits.getAllUnique('parameters')
print(params)
