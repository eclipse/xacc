import unittest as test
import sys
import xacc

PauliOperator = xacc.quantum.PauliOperator

class TestPyVQE(test.TestCase):

    def setUp(self):
        self.qpu = xacc.getAccelerator('tnqvm')
        self.buffer = self.qpu.createBuffer('q', 2)
        self.ham = PauliOperator(5.906709445) + \
                    PauliOperator({0: 'X', 1: 'X'}, -2.1433) + \
                    PauliOperator({0: 'Y', 1: 'Y'}, -2.1433) + \
                    PauliOperator({0: 'Z'}, .21829) + \
                    PauliOperator({1: 'Z'}, -6.125)

    def test_nlopt_default(self):

        @xacc.qpu(algo='vqe', accelerator=self.qpu, observable=self.ham, optimizer='nlopt')
        def ansatz(buffer, t0):
            X(0)
            Ry(t0, 1)
            CNOT(1, 0)

        ansatz(self.buffer, 0.5)

        self.assertAlmostEqual(self.buffer.getInformation("opt-val"), -1.74916, places=4)

    def test_nlopt_options(self):

        @xacc.qpu(algo='vqe', accelerator=self.qpu, observable=self.ham, optimizer='nlopt', options={"nlopt-ftol": 1e-5, "nlopt-maxeval":10, "nlopt-optimizer": "cobyla"})
        def ansatz(buffer, t0):
            X(0)
            Ry(t0, 1)
            CNOT(1, 0)

        ansatz(self.buffer, 0.5)

        self.assertAlmostEqual(self.buffer.getInformation("opt-val"), -1.74916, places=2)

    def test_scipy(self):
        # causes a numpy runtime warning. I think it has to do with whatever is the default scipy optimizer method
        @xacc.qpu(algo='vqe', accelerator=self.qpu, observable=self.ham, optimizer='scipy-opt')
        def ansatz(buffer, t0):
            X(0)
            Ry(t0, 1)
            CNOT(1, 0)

        ansatz(self.buffer, 0.5)

        self.assertAlmostEqual(self.buffer.getInformation("opt-val"), -1.74916, places=4)



    def test_scipy_options(self):

        @xacc.qpu(algo='vqe', accelerator=self.qpu, observable=self.ham, optimizer='scipy-opt', options={"method": "COBYLA", 'options':{'maxiter':10}})
        def ansatz(buffer, t0):
            X(0)
            Ry(t0, 1)
            CNOT(1, 0)

        ansatz(self.buffer, 0.5)

        self.assertAlmostEqual(self.buffer.getInformation("opt-val"), -1.74916, places=2)

    def test_bobyqa(self):

        @xacc.qpu(algo='vqe', accelerator=self.qpu, observable=self.ham, optimizer='bobyqa-opt')
        def ansatz(buffer, t0):
            X(0)
            Ry(t0, 1)
            CNOT(1, 0)

        ansatz(self.buffer, 0.5)

        self.assertAlmostEqual(self.buffer.getInformation("opt-val"), -1.74916, places=4)

    def test_bobyqa_options(self):

        @xacc.qpu(algo='vqe', accelerator=self.qpu, observable=self.ham, optimizer='bobyqa-opt', options={'maxfun': 10})
        def ansatz(buffer, t0):
            X(0)
            Ry(t0, 1)
            CNOT(1, 0)

        ansatz(self.buffer, 0.5)

        self.assertAlmostEqual(self.buffer.getInformation("opt-val"), -1.74916, places=4)

if __name__ == '__main__':
    xacc.Initialize()
    test.main()
    xacc.Finalize()
