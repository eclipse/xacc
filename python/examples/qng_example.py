
# Demonstrate Quantum Natural Gradient strategy
# when using with VQE.
import xacc,sys, numpy as np
from matplotlib import pyplot as plt

# Run the VQE algorithm using Gradient-Descent optimizer:
# @param: gradientStrategy (string): the name of the gradient calculation strategy:
# e.g. 'quantum-natural-gradient' or 'central-difference-gradient'
# @return: the energy values during optimization (as an array)
# Adapted from: https://pennylane.ai/qml/demos/tutorial_quantum_natural_gradient.html
def runVqeGradientDescent(gradientStrategy):
    # Get access to the desired QPU and
    # allocate some qubits to run on
    qpu = xacc.getAccelerator('qpp')

    # Construct the Hamiltonian as an XACC PauliOperator
    ham = xacc.getObservable('pauli', '1.0 Y0')

    # Ansatz circuit:
    xacc.qasm('''.compiler xasm
    .circuit ansatz
    .qbit q
    .parameters t0, t1, t2, t3
    // State-prep 
    Ry(q[0], pi/4);
    Ry(q[1], pi/3);
    Ry(q[2], pi/7);
    // Parametrized gates (layer 1)
    Rz(q[0], t0);
    Rz(q[1], t1);
    CX(q[0], q[1]);
    CX(q[1], q[2]);
    // Parametrized gates (layer 2)
    Ry(q[1], t2);
    Rx(q[2], t3);
    CX(q[0], q[1]);
    CX(q[1], q[2]);
    ''')

    # We need 4 qubits
    buffer = xacc.qalloc(4)
    ansatz_circuit = xacc.getCompiled('ansatz')
    initParams = [ 0.432, -0.123, 0.543, 0.233 ]
    opt = xacc.getOptimizer('mlpack', { 
                            # Using gradient descent:
                            'mlpack-optimizer': 'gd',
                            'initial-parameters': initParams,
                            'mlpack-step-size': 0.01,
                            'mlpack-max-iter': 200 })

    # Create the VQE algorithm with the requested gradient strategy
    vqe = xacc.getAlgorithm('vqe', {
                            'ansatz': ansatz_circuit,
                            'accelerator': qpu,
                            'observable': ham,
                            'optimizer': opt,
                            'gradient_strategy': gradientStrategy
                            })
    vqe.execute(buffer)
    energies = buffer.getInformation('params-energy')
    return energies

# Quantum Natural Gradients
energies_qng = runVqeGradientDescent('quantum-natural-gradient')
# Vanilla Gradients (central differential)
energies_vanilla = runVqeGradientDescent('central')

# Plot the results:
plt.style.use('seaborn')
plt.plot(energies_qng, 'g', label='Quantum natural gradient')
plt.plot(energies_vanilla, 'k', label='Vanilla gradient')
plt.ylabel('Cost function value')
plt.xlabel('Optimization steps')
plt.legend()
plt.show()