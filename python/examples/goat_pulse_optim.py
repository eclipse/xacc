# Demonstrate low-level pulse optimization API
# via direct unitary matrix input.
import xacc, sys, numpy as np

xacc.set_verbose(True)
# In this simple example, we just optimize pulse amplitude and pulse width (sigma)
# of a Gaussian pulse (2 parameters) 
initParams = [0.125, 75.0]

# Target unitary as a matrix:
# in this example, this is a sqrt(X) = Rx(pi/2)
sqrtX = np.identity(2, dtype = np.cdouble)
sqrtX[0][0] = 0.5 + 1j * 0.5
sqrtX[0][1] = 0.5 - 1j * 0.5
sqrtX[1][0] = 0.5 - 1j * 0.5
sqrtX[1][1] = 0.5 + 1j * 0.5

# Use GOAT optimizer:
# Assuming this is a rotating frame Hamiltonian in the most simple form:
# H = Signal(t) * X,
# where Signal(t) is the pulse to optimize. 
optimizer = xacc.getOptimizer("quantum-control", { 
    # Optimization method
    "method": "GOAT",
    # System dimension, i.e. number of qubits
    "dimension": 1,
    # Target unitary
    "target-U": sqrtX,
    # Control parameter (used in the control function)
    "control-params": [ "amp", "sigma" ],
    # Math expression of the pulse envelop (since GOAT is an analytical method)
    "control-funcs": [ "amp*exp(-(t - 300.0)^2/(2*sigma^2))" ],
    # The list of Hamiltonian terms that are modulated by the control functions
    # Note: this amplitude value is *estimated* from running experiments
    # on the armonk device. This can be changed due to backend calibration.
    "control-H" : [ "0.03329289102 X0" ],
    # Initial params
    "initial-parameters": initParams,
    # Time horizon
    "max-time": 600.0
    })

# The optimization will return the final fidelity error,
# and the set of optimized parameters,
# which, in this case, only contains a single 'sigma' param.
finalFidelityError, paramValues = optimizer.optimize()
dt = 0.2222222222222222

# Note: IBMQ requires the sample array length to be a multiple of 16.
def get_closest_multiple_of_16(num):
    return int(num + 8 ) - (int(num + 8 ) % 16)
nbSamples = get_closest_multiple_of_16((int)(600.0/dt))

# Construct the pulse from optimal params:
goatPulse = np.zeros(nbSamples)
for i in range(nbSamples):
    t = (float) (i * dt)
    pulseVal = paramValues[0]*np.exp(-(t - 300.0)**2/(2*paramValues[1]**2))
    goatPulse[i] = pulseVal

provider = xacc.getIRProvider("quantum")
program = provider.createComposite("goatPulse")
# Create the pulse instruction:
goatPulseInst = provider.createInstruction("goat", [0], [], { "channel" : "d0", "samples": goatPulse})
program.addInstruction(goatPulseInst)

# Measure instruction
m = provider.createInstruction("Measure", [0])
program.addInstruction(m)

# Execute on the ibmq_armonk device
qpu = xacc.getAccelerator("ibm:ibmq_armonk")
buffer = xacc.qalloc(1)
qpu.execute(buffer, program)

# We should get a 50-50 distribution.
print(buffer)


