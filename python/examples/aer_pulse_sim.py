# This example demonstrates XACC pulse utility: 
# assembling pulse programs (schedules) and running simulation.

import xacc, sys, numpy as np

# Helper to create a centered Gaussian pulse:
def gaussianCalc(in_time, in_amp, in_center, in_sigma):
    return in_amp*np.exp(-(in_time - in_center)**2/ 2.0 / (in_sigma**2.0))

# Construct a sample pulse
nbSamples = 160
my_pulse = np.zeros(nbSamples)
amp = 0.1
for i in range(nbSamples):
    my_pulse[i] = gaussianCalc(i, amp, nbSamples/2, nbSamples/4)

provider = xacc.getIRProvider("quantum")
program = provider.createComposite("gaussian")
# Create the pulse instructions
gaussian = provider.createInstruction("gaussian", [0], [], { "channel" : "d0", "samples": my_pulse})
program.addInstruction(gaussian)


# Measure instructions (to be lowered to pulses)
m0 = provider.createInstruction("Measure", [0])
m1 = provider.createInstruction("Measure", [1])
m2 = provider.createInstruction("Measure", [2])
m3 = provider.createInstruction("Measure", [3])
m4 = provider.createInstruction("Measure", [4])

program.addInstruction(m0)
program.addInstruction(m1)
program.addInstruction(m2)
program.addInstruction(m3)
program.addInstruction(m4)

# Execute on the Aer simulator (bogota 5-qubit device)
qpu = xacc.getAccelerator("aer:ibmq_bogota", {"sim-type": "pulse"})
buffer = xacc.qalloc(5)
qpu.execute(buffer, program)

# print(buffer)

# Aer simulator will also return the state vector:
# Note: it looks like the state-vector is in the qutrit space...
# and the leaked state |2> is measured as 0.
state_vec = buffer.getInformation("state")
print(len(state_vec))


