import unittest as test
import xacc
import numpy as np

class TestAlgorithmPlugins(test.TestCase):
    def test_adapt(self):
        qpu = xacc.getAccelerator('qpp')
        optimizer = xacc.getOptimizer('nlopt',{'nlopt-optimizer':'l-bfgs'})
        buffer = xacc.qalloc(4)

        opstr = '''
        (-0.165606823582,-0)  1^ 2^ 1 2 + (0.120200490713,0)  1^ 0^ 0 1 + 
        (-0.0454063328691,-0)  0^ 3^ 1 2 + (0.168335986252,0)  2^ 0^ 0 2 + 
        (0.0454063328691,0)  1^ 2^ 3 0 + (0.168335986252,0)  0^ 2^ 2 0 + 
        (0.165606823582,0)  0^ 3^ 3 0 + (-0.0454063328691,-0)  3^ 0^ 2 1 + 
        (-0.0454063328691,-0)  1^ 3^ 0 2 + (-0.0454063328691,-0)  3^ 1^ 2 0 + 
        (0.165606823582,0)  1^ 2^ 2 1 + (-0.165606823582,-0)  0^ 3^ 0 3 + 
        (-0.479677813134,-0)  3^ 3 + (-0.0454063328691,-0)  1^ 2^ 0 3 + 
        (-0.174072892497,-0)  1^ 3^ 1 3 + (-0.0454063328691,-0)  0^ 2^ 1 3 + 
        (0.120200490713,0)  0^ 1^ 1 0 + (0.0454063328691,0)  0^ 2^ 3 1 + 
        (0.174072892497,0)  1^ 3^ 3 1 + (0.165606823582,0)  2^ 1^ 1 2 + 
        (-0.0454063328691,-0)  2^ 1^ 3 0 + (-0.120200490713,-0)  2^ 3^ 2 3 + 
        (0.120200490713,0)  2^ 3^ 3 2 + (-0.168335986252,-0)  0^ 2^ 0 2 + 
        (0.120200490713,0)  3^ 2^ 2 3 + (-0.120200490713,-0)  3^ 2^ 3 2 + 
        (0.0454063328691,0)  1^ 3^ 2 0 + (-1.2488468038,-0)  0^ 0 + 
        (0.0454063328691,0)  3^ 1^ 0 2 + (-0.168335986252,-0)  2^ 0^ 2 0 + 
        (0.165606823582,0)  3^ 0^ 0 3 + (-0.0454063328691,-0)  2^ 0^ 3 1 + 
        (0.0454063328691,0)  2^ 0^ 1 3 + (-1.2488468038,-0)  2^ 2 + 
        (0.0454063328691,0)  2^ 1^ 0 3 + (0.174072892497,0)  3^ 1^ 1 3 + 
        (-0.479677813134,-0)  1^ 1 + (-0.174072892497,-0)  3^ 1^ 3 1 + 
        (0.0454063328691,0)  3^ 0^ 1 2 + (-0.165606823582,-0)  3^ 0^ 3 0 + 
        (0.0454063328691,0)  0^ 3^ 2 1 + (-0.165606823582,-0)  2^ 1^ 2 1 + 
        (-0.120200490713,-0)  0^ 1^ 0 1 + (-0.120200490713,-0)  1^ 0^ 1 0 + (0.7080240981,0)
        '''
        H = xacc.getObservable('fermion', opstr)

        adapt = xacc.getAlgorithm('adapt', {'accelerator': qpu,
                                        'optimizer': optimizer,
                                        'observable': H,
                                        'n-electrons': 2,
                                        'maxiter': 2,
                                        'sub-algorithm': 'vqe',
                                        'pool': 'qubit-pool'})
        # execute
        adapt.execute(buffer)
        self.assertAlmostEqual(buffer['opt-val'], -1.137, 1) 
    
    def test_qcmx(self):
        accelerator = xacc.getAccelerator("qpp")
        H = xacc.getObservable('pauli', '0.2976 + 0.3593 Z0 - 0.4826 Z1 + 0.5818 Z0 Z1 + 0.0896 X0 X1 + 0.0896 Y0 Y1')
        expansion = 'Cioslowski'
        order = 2
        provider = xacc.getIRProvider('quantum')
        ansatz = provider.createComposite('initial-state')
        ansatz.addInstruction(provider.createInstruction('X', [0]))

        buffer = xacc.qalloc(2)

        qcmx = xacc.getAlgorithm('qcmx', {
                                'accelerator': accelerator,
                                'observable': H,
                                'ansatz': ansatz,
                                'cmx-order': order,
                                'expansion-type': expansion
                                })

        qcmx.execute(buffer)
        print(buffer["spectrum"])
        print(buffer["energies"]) 
    
    def test_qeom(self):
        accelerator = xacc.getAccelerator("qpp")

        buffer = xacc.qalloc(4)

        opstr = '''
        (-0.165606823582,-0)  1^ 2^ 1 2 + (0.120200490713,0)  1^ 0^ 0 1 +
        (-0.0454063328691,-0)  0^ 3^ 1 2 + (0.168335986252,0)  2^ 0^ 0 2 +
        (0.0454063328691,0)  1^ 2^ 3 0 + (0.168335986252,0)  0^ 2^ 2 0 +
        (0.165606823582,0)  0^ 3^ 3 0 + (-0.0454063328691,-0)  3^ 0^ 2 1 +
        (-0.0454063328691,-0)  1^ 3^ 0 2 + (-0.0454063328691,-0)  3^ 1^ 2 0 +
        (0.165606823582,0)  1^ 2^ 2 1 + (-0.165606823582,-0)  0^ 3^ 0 3 +
        (-0.479677813134,-0)  3^ 3 + (-0.0454063328691,-0)  1^ 2^ 0 3 +
        (-0.174072892497,-0)  1^ 3^ 1 3 + (-0.0454063328691,-0)  0^ 2^ 1 3 +
        (0.120200490713,0)  0^ 1^ 1 0 + (0.0454063328691,0)  0^ 2^ 3 1 +
        (0.174072892497,0)  1^ 3^ 3 1 + (0.165606823582,0)  2^ 1^ 1 2 +
        (-0.0454063328691,-0)  2^ 1^ 3 0 + (-0.120200490713,-0)  2^ 3^ 2 3 +
        (0.120200490713,0)  2^ 3^ 3 2 + (-0.168335986252,-0)  0^ 2^ 0 2 +
        (0.120200490713,0)  3^ 2^ 2 3 + (-0.120200490713,-0)  3^ 2^ 3 2 +
        (0.0454063328691,0)  1^ 3^ 2 0 + (-1.2488468038,-0)  0^ 0 +
        (0.0454063328691,0)  3^ 1^ 0 2 + (-0.168335986252,-0)  2^ 0^ 2 0 +
        (0.165606823582,0)  3^ 0^ 0 3 + (-0.0454063328691,-0)  2^ 0^ 3 1 +
        (0.0454063328691,0)  2^ 0^ 1 3 + (-1.2488468038,-0)  2^ 2 +
        (0.0454063328691,0)  2^ 1^ 0 3 + (0.174072892497,0)  3^ 1^ 1 3 +
        (-0.479677813134,-0)  1^ 1 + (-0.174072892497,-0)  3^ 1^ 3 1 +
        (0.0454063328691,0)  3^ 0^ 1 2 + (-0.165606823582,-0)  3^ 0^ 3 0 +
        (0.0454063328691,0)  0^ 3^ 2 1 + (-0.165606823582,-0)  2^ 1^ 2 1 +
        (-0.120200490713,-0)  0^ 1^ 0 1 + (-0.120200490713,-0)  1^ 0^ 1 0 + (0.7080240981,0)
        '''
        H = xacc.getObservable('fermion', opstr)

        pool = xacc.quantum.getOperatorPool("singlet-adapted-uccsd")
        pool.optionalParameters({"n-electrons": 2})
        pool.generate(buffer.size())
        provider = xacc.getIRProvider('quantum')
        ansatz = provider.createComposite('initial-state')
        ansatz.addInstruction(provider.createInstruction('X', [0]))
        ansatz.addInstruction(provider.createInstruction('X', [2]))
        ansatz.addVariable("x0")
        for inst in pool.getOperatorInstructions(2, 0).getInstructions():
            ansatz.addInstruction(inst)
        kernel = ansatz.eval([0.0808])

        qeom = xacc.getAlgorithm('qeom', {
                                'accelerator': accelerator,
                                'observable': H,
                                'ansatz': kernel,
                                'n-electrons': 2
                                })

        qeom.execute(buffer)
        spectrum = buffer.getInformation("excitation-energies")
        print(spectrum)

    def test_quantum_natural_gradient(self):
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
        print(energies_qng)
        self.assertAlmostEqual(energies_qng[-1], -0.61, 1)
        
if __name__ == '__main__':
    test.main()
