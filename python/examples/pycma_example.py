import xacc
import numpy as np


def rosen(x, args):
    xx = (1.-x[0])**2 + 100*(x[1]-x[0]**2)**2
    return xx


optimizer = xacc.getOptimizer('pycma', {
                              'tolx': 1e-12, 'AdaptSigma': True, 'CMA_elitist': True, 'popsize': 4 + np.floor(2*np.log(2))})

f = xacc.OptFunction(rosen, 2)
r, p = optimizer.optimize(f)

print('Result = ', r, p)
