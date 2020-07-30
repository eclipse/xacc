# Demonstrate low-level pulse optimization API
# via direct unitary matrix input.
import xacc, sys, numpy as np

# In this simple example, we just optimize a pulse width (sigma)
# of a Gaussian pulse (1 parameter) 
initParams = [4.0]

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
    "control-params": [ "sigma" ],
    # Math expression of the pulse envelop (since GOAT is an analytical method)
    "control-funcs": [ "0.062831853*exp(-t^2/(2*sigma^2))" ],
    # The list of Hamiltonian terms that are modulated by the control functions
    "control-H" : [ "X0" ],
    # Initial params
    "initial-parameters": initParams,
    # Time horizon
    "max-time": 100.0
    })

# The optimization will return the final fidelity error,
# and the set of optimized parameters,
# which, in this case, only contains a single 'sigma' param.
finalFidelityError, paramValues = optimizer.optimize()
print("Optimal sigma = ", paramValues[0])
