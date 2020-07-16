import unittest as test
import sys
import xacc

PauliOperator = xacc.quantum.PauliOperator

class TestDecoratorAPI(test.TestCase):

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