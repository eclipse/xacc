import unittest as test
import sys
import xacc
from xaccvqe import PauliOperator

class TestDecoratorAPI(test.TestCase):

    def test_VQE(self):
        op = PauliOperator(5.906709445) + \
            PauliOperator({0: 'X', 1: 'X'}, -2.1433) + \
            PauliOperator({0: 'Y', 1: 'Y'}, -2.1433) + \
            PauliOperator({0: 'Z'}, .21829) + \
            PauliOperator({1: 'Z'}, -6.125)

        xacc.Initialize()

        qpu = xacc.getAccelerator('tnqvm')
        buffer = qpu.createBuffer('q', 4)
        @xacc.qpu(algo='vqe', accelerator=qpu, observable=op)#, optimizer='scipy-COBYLA')#, options={'disp': True, 'maxiter': 400, 'tol': 1e-4})
        def vqe(buffer, t0):
            X(0)
            Ry(t0, 1)
            CNOT(1, 0)

        vqe(buffer)

        self.assertAlmostEqual(buffer.getInformation('vqe-energy'), -1.749085416, places=3)
        self.assertEqual(vqe.nParameters(), 1)
        self.assertEqual(vqe.getFunction().toString(), 'X q0\nRy(t0) q1\nCNOT q1,q0\n')

    def test_energy(self):
        op = PauliOperator(5.906709445) + \
            PauliOperator({0: 'X', 1: 'X'}, -2.1433) + \
            PauliOperator({0: 'Y', 1: 'Y'}, -2.1433) + \
            PauliOperator({0: 'Z'}, .21829) + \
            PauliOperator({1: 'Z'}, -6.125)

        xacc.Initialize()

        qpu = xacc.getAccelerator('tnqvm')
        buffer = qpu.createBuffer('q', 4)

        @xacc.qpu(algo='energy', accelerator=qpu, observable=op)
        def energy(buffer, t0):
            X(0)
            Ry(t0, 1)
            CNOT(1, 0)
        energy(buffer)

        self.assertAlmostEqual(buffer.getInformation('vqe-energy'), -1.749085416, places=3)

        self.assertEqual(energy.nParameters(), 1)
        self.assertEqual(energy.getFunction().toString(), 'X q0\nRy(t0) q1\nCNOT q1,q0\n')

    def test_generic(self):

        qpu = xacc.getAccelerator('tnqvm')
        buffer = qpu.createBuffer('q', 4)

        @xacc.qpu(accelerator=qpu)
        def entangle(buffer):
            H(0)
            CNOT(0, 1)
            Measure(0, 0)
            Measure(1, 1)

        xacc.Initialize()

        entangle(buffer)

        self.assertEqual(entangle.nParameters(), 0)
        self.assertEqual(entangle.getFunction().toString(), 'H q0\nCNOT q0,q1\nMeasure q0\nMeasure q1\n')
        

if __name__ == '__main__':
    xacc.Initialize()
    test.main()
    xacc.Finalize()