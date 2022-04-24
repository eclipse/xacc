import unittest as test
import xacc
import numpy as np

class TestExternalPlugins(test.TestCase):
    def test_pyscf(self):
        qpu = xacc.getAccelerator('aer', {'sim-type':'statevector'})
        buffer = xacc.qalloc(4)

        geom = '''
        H  0.000000   0.0      0.0
        H   0.0        0.0  .7474
        '''
        H = xacc.getObservable('pyscf', {'basis': 'sto-3g', 'geometry': geom})

        @xacc.qpu(algo='vqe', accelerator=qpu, observable=H)
        def ansatz(q, x):
            X(q[0])
            X(q[2])
            ucc1(q, x[0])

        ansatz(buffer, 0.0)
        print('Energy = ', buffer.getInformation('opt-val'))
        print('Opt Angles = ', buffer.getInformation('opt-params'))
        self.assertAlmostEqual(buffer.getInformation('opt-val'), -1.137, 1) 

    def test_cma_optimizer(self):
        def rosen(x, args):
            xx = (1.-x[0])**2 + 100*(x[1]-x[0]**2)**2
            return xx

        optimizer = xacc.getOptimizer('pycma', {
                                    'tolx': 1e-12, 'AdaptSigma': True, 'CMA_elitist': True, 'popsize': 4 + np.floor(2*np.log(2))})

        f = xacc.OptFunction(rosen, 2)
        r, p = optimizer.optimize(f)
        print('Result = ', r, p)
        # global minimum of 0 at the point (1, 1).
        self.assertAlmostEqual(r, 0.0, 5) 
        self.assertAlmostEqual(p[0], 1.0, 5) 
        self.assertAlmostEqual(p[1], 1.0, 5) 

if __name__ == '__main__':
    test.main()
