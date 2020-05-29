import xacc,sys, numpy as np

# Get access to the desired QPU and
# allocate some qubits to run on
qpu = xacc.getAccelerator('qpp',  { 'shots': 4096 })

# In this example: we want to estimate the *phase* of an arbitrary 'oracle'
# i.e. Oracle(|State>) = exp(i*Phase)*|State>
# and we need to estimate that Phase.

# The oracle is a simple T gate, and the eigenstate is |1>
# T|1> = e^(i*pi/4)|1> 
# The phase value of pi/4 = 2pi * (1/8)
# i.e. if we use a 3-bit register for estimation, 
# we will get the correct answer of 1 deterministically.

xacc.qasm('''.compiler xasm
.circuit oracle
.qbit q
T(q[0]);
''')
oracle = xacc.getCompiled('oracle')

# We need to prepare the eigenstate |1>
xacc.qasm('''.compiler xasm
.circuit prep
.qbit q
X(q[0]);
''')
statePrep = xacc.getCompiled('prep')

# We need 4 qubits (3-bit precision)
buffer = xacc.qalloc(4)

# Create the QPE algorithm
qpe = xacc.getAlgorithm('QPE', {
                        'accelerator': qpu,
                        'oracle': oracle,
                        'state-preparation': statePrep
                        })

qpe.execute(buffer)
# We should only get the bit string of |100> = 1
# i.e. phase value of 1/2^3 = 1/8.
print(buffer)