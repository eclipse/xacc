import xacc
# xacc.set_verbose(True)
# Get the QPU and allocate a single qubit
qpu = xacc.getAccelerator('aer')
qbits = xacc.qalloc(1)

# Get the MLPack Optimizer, default is Adam
optimizer = xacc.getOptimizer('mlpack')

# Create a simple quantum program
xacc.qasm('''
.compiler xasm
.circuit foo
.parameters x,y,z
.qbit q
Ry(q[0], x);
Ry(q[0], y);
Ry(q[0], z);
''')
f = xacc.getCompiled('foo')

f.defaultPlacement(qpu, {'qubit-map':[0]})

print(f.toString())
# exit(0)
# Get the DDCL Algorithm, initialize it
# with necessary parameters
ddcl = xacc.getAlgorithm('ddcl', {'ansatz': f,
                                  'accelerator': qpu,
                                  'target_dist': [.5,.5],
                                  'optimizer': optimizer,
                                  'loss': 'js',
                                  'gradient': 'js-parameter-shift'})
# execute
ddcl.execute(qbits)

print(qbits.keys())
print(qbits['opt-val'])
print(qbits['opt-params'])