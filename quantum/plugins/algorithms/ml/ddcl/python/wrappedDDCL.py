from pelix.ipopo.decorators import (ComponentFactory, Property, Requires,
                                    Provides, Instantiate, BindField, UnbindField)
import xacc
import inspect

@ComponentFactory("wrapped_ddcl_factory")
@Provides("decorator_algorithm_service")
@Property("_algorithm", "algorithm", "ddcl")
@Property("_name", "name", "ddcl")
@Requires("_vqe_optimizers", "vqe_optimization", aggregate=True, optional=True)
@Instantiate("wrapped_ddcl_instance")
class WrappedDDCLF(xacc.DecoratorFunction):

    def __init__(self):
        self.vqe_optimizers = {}
        self.observable = None

    @BindField("_vqe_optimizers")
    def bind_optimizers(self, field, service, svc_ref):
        if svc_ref.get_property('vqe_optimizer'):
            optimizer = svc_ref.get_property('vqe_optimizer')
            self.vqe_optimizers[optimizer] = service

    @UnbindField("_vqe_optimizers")
    def unbind_optimizers(self, field, service, svc_ref):
        if svc_ref.get_property('vqe_optimizer'):
            optimizer = svc_ref.get_property('vqe_optimizer')
            del self.vqe_optimizers[optimizer]

    def __call__(self, *args, **kwargs):
        super().__call__(*args, **kwargs)

        execParams = {'accelerator': self.qpu, 'ansatz': self.compiledKernel, 'target_dist':self.kwargs['target_dist'], 'loss':self.kwargs['loss']}
        if 'gradient' in self.kwargs:
            execParams['gradient'] = self.kwargs['gradient']
        optParams = {}

        if not isinstance(args[0], xacc.AcceleratorBuffer):
            raise RuntimeError(
                'First argument of an xacc kernel must be the Accelerator Buffer to operate on.')

        buffer = args[0]
        ars = args[1:]
        if len(ars) > 0:
            optParams['initial-parameters'] = list(ars)

        if 'options' in self.kwargs:
            optParams = self.kwargs['options']

        if 'optimizer' not in self.kwargs:
            self.kwargs['optimizer'] = 'nlopt'

        if self.kwargs['optimizer'] in self.vqe_optimizers:
            optimizer = self.vqe_optimizers[self.kwargs['optimizer']]
            optimizer.optimize(buffer, optParams, execParams)
        else:
            execParams['optimizer'] = xacc.getOptimizer(self.kwargs['optimizer'], optParams)

        ddcl = xacc.getAlgorithm('ddcl', execParams)
        ddcl.execute(buffer)

        return