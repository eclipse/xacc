from pelix.ipopo.decorators import (ComponentFactory, Property, Requires,
                                    Provides, Instantiate, BindField, UnbindField)
import xacc
import inspect

@ComponentFactory("wrapped_vqe_factory")
@Provides("decorator_algorithm_service")
@Property("_algorithm", "algorithm", "vqe")
@Property("_name", "name", "vqe")
@Requires("_vqe_optimizers", "vqe_optimization", aggregate=True, optional=True)
@Instantiate("wrapped_vqe_instance")
class WrappedVQEF(xacc.DecoratorFunction):

    def __init__(self):
        self.vqe_optimizers = {}
        self.observable = None

    def __call__(self, *args, **kwargs):
        super().__call__(*args, **kwargs)

        execParams = {'accelerator': self.qpu, 'ansatz': self.compiledKernel, 'observable': self.kwargs["observable"]}
        optParams = {}

        if not isinstance(args[0], xacc.AcceleratorBuffer):
            raise RuntimeError(
                'First argument of an xacc kernel must be the Accelerator Buffer to operate on.')

        buffer = args[0]
        ars = args[1:]
        if len(ars) > 0:
            if isinstance(ars,list) or isinstance(ars,tuple):
                # print('ars is a list')
                optParams['initial-parameters'] = ars[0]
            else:
                optParams['initial-parameters'] = list(ars)

        # print(type(ars), optParams['initial-parameters'])
        if 'options' in self.kwargs:
            optParams = self.kwargs['options']

        if 'optimizer' not in self.kwargs:
            self.kwargs['optimizer'] = 'nlopt'

        execParams['optimizer'] = xacc.getOptimizer(self.kwargs['optimizer'], optParams)

        vqe = xacc.getAlgorithm('vqe', execParams)
        vqe.execute(buffer)

        return