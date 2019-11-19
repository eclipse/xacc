import xacc
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

@ComponentFactory("pycma_optimizer_factory")
@Provides("optimizer")
@Property("_optimizer", "optimizer", "pycma")
@Property("_name", "name", "pycma")
@Instantiate("pycma_instance")
class PyCMAOptimizer(xacc.Optimizer):
    def __init__(self):
        xacc.Optimizer.__init__(self)
        self.options = {}
        self.sigma = .1
        self.hetOpts = None

    def name(self):
        return 'pycma'

    def setOptions(self, opts):
        self.hetOpts = opts
        if 'sigma' in opts:
            self.sigma = opts['sigma']
        if 'maxfevals' in opts:
            self.options['maxfevals'] = opts['maxfevals']
        if 'tolx' in opts:
            self.options['tolx'] = opts['tolx']
        if 'AdaptSigma' in opts:
            self.options['AdaptSigma'] = True
        if 'CMA_elitist' in opts:
            self.options['CMA_elitist'] = opts['CMA_elitist']
        if 'popsize' in opts:
            self.options['popsize'] = opts['popsize']


    def optimize(self, function):
        import cma
        params = function.dimensions() * [0.]
        if 'initial-parameters' in self.hetOpts:
            params = self.hetOpts['initial-parameters']
        es = cma.CMAEvolutionStrategy(params, self.sigma, self.options)
        es.optimize(function)
        return (es.result.fbest, es.result.xbest)








