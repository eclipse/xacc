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

    @BindField("_vqe_optimizers")
    def bind_optimizers(self, field, service, svc_ref):
        if svc_ref.get_property('vqe_optimizer'):
            optimizer = svc_ref.get_property('vqe_optimizer')
            self.vqe_optimizers[optimizer] = service

    @UnbindField("_vqe_optimizers")
    def unbind_optimizers(self, field, service, svc_ref):
        if svc_ref.get_property('vqe_optimizer'):
            optimizer = svc_ref.get_property('vqe_optimizer')
            del vqe_optimizers[optimizer]

    def __call__(self, *args, **kwargs):
        super().__call__(*args, **kwargs)

        execParams = {'accelerator': self.kwargs['accelerator'], 'ansatz': self.compiledKernel, 'observable': self.kwargs["observable"]}

        if not isinstance(args[0], xacc.AcceleratorBuffer):
            raise RuntimeError(
                'First argument of an xacc kernel must be the Accelerator Buffer to operate on.')

        buffer = args[0]
        ars = args[1:]
        if len(ars) > 0:
            execParams['initial-parameters'] = list(ars)
        if 'options' in self.kwargs:
            optimizer_args = self.kwargs['options']
        else:
            optimizer_args = {}

        if self.kwargs['optimizer'] in self.vqe_optimizers:
            optimizer = self.vqe_optimizers[self.kwargs['optimizer']]
            optimizer.optimize(buffer, optimizer_args, execParams)
        else:
            execParams['optimizer'] = xacc.getOptimizer(self.kwargs['optimizer'])

            if len(optimizer_args) > 0:
                for k,v in optimizer_args.items():
                    execParams[k] = v

            vqe = xacc.getAlgorithm('vqe')
            vqe.initialize(execParams)
            vqe.execute(buffer)

        return