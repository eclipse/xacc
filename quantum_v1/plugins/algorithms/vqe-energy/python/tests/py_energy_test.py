import unittest as test
import sys
import xacc
from xacc import PauliOperator

class TestPyVQE(test.TestCase):

    def setUp(self):
        self.qpu = xacc.getAccelerator('tnqvm')
        self.buffer = self.qpu.createBuffer('q', 2)
        self.ham = PauliOperator(5.906709445) + \
                    PauliOperator({0: 'X', 1: 'X'}, -2.1433) + \
                    PauliOperator({0: 'Y', 1: 'Y'}, -2.1433) + \
                    PauliOperator({0: 'Z'}, .21829) + \
                    PauliOperator({1: 'Z'}, -6.125)

    def test_energy(self):

        @xacc.qpu(algo='energy', accelerator=self.qpu, observable=self.ham)
        def ansatz(buffer, t0):
            X(0)
            Ry(t0, 1)
            CNOT(1, 0)

        ansatz(self.buffer, 0.5)

        self.assertAlmostEqual(self.buffer.getInformation("opt-val"), -1.71515, places=4)


if __name__ == '__main__':
    xacc.Initialize()
    test.main()
    xacc.Finalize()