from pyquil.quil import Program
import pyquil.api as forest
from pyquil.gates import X, Z, H, CNOT, RX, RY, RZ
import numpy as np
import pyxacc as xacc

# Pyquil State Preparation circuit gen
def statePrep(qs, angle):
   return Program(RX(np.pi, qs[0]),
      RY(np.pi / 2, qs[1]),
      RX(2.5*np.pi, qs[0]), 
      CNOT(qs[1],qs[0]), 
      RZ(angle, qs[0]), 
      CNOT(qs[1],qs[0]),
      RY(2.5*np.pi, qs[1]), 
      RX(np.pi / 2, qs[0]))

# Z0 term of Hamiltonian
def Z0Term(qs, angle):
    p = statePrep(qs,angle)
    p.measure(qs[0],0)
    return p

# Z1 term of Hamiltonian
def Z1Term(qs, angle):
    p = statePrep(qs, angle)
    p.measure(qs[1],0)
    return p

# Z0Z1 term of Hamiltonian
def Z0Z1Term(qs, angle):
    p = statePrep(qs, angle)
    p.measure(qs[1],1)
    p.measure(qs[0],0)
    return p

# collect all generators in a list
terms = [Z0Term, Z1Term, Z0Z1Term]

# Initialize XACC
xacc.Initialize()

# turn on the QuilCompiler
xacc.setOption('compiler','quil')

# Get reference to our tensor network simulator
qpu = xacc.getAccelerator('tnqvm')

# create some qubits - an AcceleratorBuffer
qs = qpu.createBuffer('qs',2)

# Store results to this CSV file
file = open('pyquil_out.csv', 'w')
file.write('theta, Z0, Z1, Z0Z1\n')

# Pyquil uses int indices for qubits
pyquilQubits = [0,1]

# Loop -pi to pi
for theta in np.linspace(-np.pi,np.pi,100):
   file.write(str(theta))
   
   # Loop over our above generators
   for t in terms:
      file.write(', ')
      
      # Kernel-ize the pyquil program
      src = '__qpu__ kernel(AcceleratorBuffer qs, double theta) {\n'
      src += t([0,1], theta).out()
      src += '}'
      
      # Create an XACC Program and compile
      xaccProgram = xacc.Program(qpu, src)
      xaccProgram.build()
      
      # Get reference to the executable kernel
      k = xaccProgram.getKernel('kernel')
      
      # Execute, no params since theta has
      # already been input to the term generator
      k.execute(qs, [])
      
      # Get the expectation value
      e = qs.getExpectationValueZ()
      
      # Reset the qubits for the next iteration
      qs.resetBuffer()
      
      file.write(str(e))
   file.write('\n')

file.close()

# Finalize the framework. 
xacc.Finalize()




