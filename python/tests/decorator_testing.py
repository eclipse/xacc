import unittest as test
import xacc

class TestDecoratorAPI(test.TestCase):
    def test_generic(self):
        qpu = xacc.getAccelerator('qpp', {'shots': 1024})
        buffer = xacc.qalloc(2)
        @xacc.qpu(accelerator=qpu)
        def entangle(q):
            H(q[0])
            CNOT(q[0], q[1])
            Measure(q[0])
            Measure(q[1])

        entangle(buffer)
        self.assertEqual(entangle.nParameters(), 0)
        self.assertEqual(entangle.getCompositeInstruction().toString(), 'H q0\nCNOT q0,q1\nMeasure q0\nMeasure q1\n')

    def test_vqe_uccsd(self):
        opstr = '''(0.174073,0) Z1 Z3 +
           (0.1202,0) Z2 Z3 +
           (0.165607,0) Z1 Z2 +
           (0.165607,0) Z0 Z3 +
           (0.1202,0) Z0 Z1 +
           (-0.0454063,0) Y0 Y2 X1 X3 +
           (-0.220041,0) Z3 +
           (-0.106477,0) +
           (0.17028,0) Z0 +
           (-0.220041,0) Z1 +
           (0.17028,0) Z2 +
           (-0.0454063,0) X0 X2 Y1 Y3 +
           (0.0454063,0) X0 Y2 Y1 X3 +
           (0.168336,0) Z0 Z2 +
           (0.0454063,0) Y0 X2 X1 Y3'''

        op = xacc.getObservable('pauli',opstr)
        qpu = xacc.getAccelerator('qpp')

        @xacc.qpu(algo='vqe', accelerator=qpu, observable=op)
        def ansatz_vqe(q, theta0, theta1):
            uccsd(q,{"ne":2,"nq":4})

        print(ansatz_vqe.getCompositeInstruction().toString())
        buffer = xacc.qalloc(4)
        ansatz_vqe(buffer, 0.,0.)
        self.assertAlmostEqual(buffer["opt-val"], -1.137, 1) 

if __name__ == '__main__':
    test.main()
