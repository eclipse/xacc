import xacc
import numpy as np
xacc.set_verbose(True)
# Choose the QPU on which to
# characterize the process matrix for a Hadamard
qpu = xacc.getAccelerator('ibm:ibmq_poughkeepsie')

# Create the CompositeInstruction containing a
# single Hadamard instruction
provider = xacc.getIRProvider('quantum')
circuit = provider.createComposite('U')
cnot = provider.createInstruction('CX', [0,1])
circuit.addInstruction(cnot)

# Create the Algorithm, give it the circuit
# to characterize and the backend to target
qpt = xacc.getAlgorithm('qpt', {'circuit':circuit, 'accelerator':qpu, 'qubit-map':[1,2]})

# Allocate a qubit, this will
# store our tomography results
buffer = xacc.qalloc(2)

# Execute
qpt.execute(buffer)

theory_cx = [1., 0., 0., 1., 1., 0., 0., -1., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 1., 0., 0.,
        1., 1., 0., 0., -1., 0., 0., 0., 0., 0., 0., 0., 0., 1., 0., 0., 1., 1.,
        0., 0., -1., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., -1., 0., 0., -1., -1., 0., 0., 1., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
        0., 0., 0., 0., 0., 0., 0., 0.]

# Compute the fidelity with respect to
# the ideal hadamard process
F = qpt.calculate('fidelity', buffer, {'chi-theoretical-real':theory_cx})
print('\nFidelity: ', F)

# Can get the chi process matrix and print it
chi_real_part = np.array(buffer['chi-real'])
chi_imag_part = np.array(buffer['chi-imag'])
chi = np.reshape(chi_real_part + 1j*chi_imag_part, (16,16))
print('\nChi:\n', chi)

