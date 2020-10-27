import xacc

# Get the QPU and allocate a single qubit
qpu = xacc.getAccelerator('qpp')
qbits = xacc.qalloc(2)

# Get the MLPack Optimizer, default is Adam
optimizer = xacc.getOptimizer(
    'mlpack', {'initial-parameters': [0., 0., 0., 0., 0., 0., 0., 0.]})

xacc.qasm('''
.compiler xasm
.circuit qubit2_depth1
.parameters x
.qbit q
U(q[0], x[0], -pi/2, pi/2 );
U(q[0], 0, 0, x[1]);
U(q[1], x[2], -pi/2, pi/2);
U(q[1], 0, 0, x[3]);
CNOT(q[0], q[1]);
U(q[0], 0, 0, x[4]);
U(q[0], x[5], -pi/2, pi/2);
U(q[1], 0, 0, x[6]);
U(q[1], x[7], -pi/2, pi/2);
''')

f = xacc.getCompiled('qubit2_depth1')

# Get the DDCL Algorithm, initialize it
# with necessary parameters
ddcl = xacc.getAlgorithm('ddcl', {'ansatz': f,
                                  'accelerator': qpu,
                                  'target_dist': [0., 0., 0., 1.],
                                  'persist-buffer': True,
                                  'optimizer': optimizer,
                                  'loss': 'js',
                                  'gradient': 'js-parameter-shift'
                                  })
# execute
ddcl.execute(qbits)

# Note that the buffer file was persisted to
# the hidden file with name
# .ddcl_KERNELNAME_TIMESTAMP.ab, which in this case looks like
# .ddcl_qubit2_depth1_2019-10-.....ab -> depends on time you ran this :)
