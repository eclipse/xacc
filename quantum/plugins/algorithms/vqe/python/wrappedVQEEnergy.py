from pelix.ipopo.decorators import (ComponentFactory, Property, Requires,
                                    BindField, UnbindField, Provides, Instantiate)
import xacc
import inspect
import random

@ComponentFactory("wrapped_energy_factory")
@Provides("decorator_algorithm_service")
@Property("_algorithm", "algorithm", "energy")
@Property("_name", "name", "energy")
@Instantiate("wrapped_energy_instance")
class WrappedEnergyF(xacc.DecoratorFunction):

    def __call__(self, *args, **kwargs):
        super().__call__(*args, **kwargs)
        import numpy as np

        execParams = {'accelerator': self.qpu, 'ansatz': self.compiledKernel, 'observable': self.kwargs["observable"]}

        if not isinstance(args[0], xacc.AcceleratorBuffer):
            raise RuntimeError(
                'First argument of an xacc kernel must be the Accelerator Buffer to operate on.')

        buffer = args[0]
        ars = args[1:]
        if len(ars) > 0:
            execParams['parameters'] = list(ars)
        else:
            execParams['parameters'] = random.randrange(-np.pi, np.pi)

        vqe_energy = xacc.getAlgorithm('vqe', execParams)
        vqe_energy.execute(buffer, execParams['parameters'])
        return
