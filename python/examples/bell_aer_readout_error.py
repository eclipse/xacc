import xacc

# Get the Aer Accelerator, provides noise modeling
# based on specified IBM backend. Can turn on
# readout_error, thermal_relaxation, and gate_error
qpu = xacc.getAccelerator(
    'aer', {'shots': 1024, 'backend': 'ibmq_johannesburg', 'readout_error': True})

# Define the quantum kernel in standard Python
@xacc.qpu(accelerator=qpu)
def bell(q):
    H(q[0])
    CX(q[0],q[1])
    Measure(q[0])
    Measure(q[1])

# Allocate 2 qubits
q = xacc.qalloc(2)

# run the bell state computation
bell(q)

print(q)
