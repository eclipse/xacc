from abc import abstractmethod, ABC
from pelix.ipopo.decorators import (Provides, ComponentFactory, Property, Instantiate)
import xacc

@Provides("vqe_optimization")
class VQEOpt(ABC):

    @abstractmethod
    def optimize(self, buffer, optimizer_args, execParams):
        self.opt_args = optimizer_args
        self.execParams = execParams
        self.energies = []
        self.angles = []
        self.vqe_energy = xacc.getAlgorithm('vqe-energy')
        self.buffer = buffer

        if 'initial-parameters' in self.opt_args:
            self.init_args = self.opt_args.pop('initial-parameters')
        else:
            import random
            import numpy as np
            pi = 3.141592653
            self.init_args = np.array([random.uniform(-pi, pi) for _ in range(self.execParams['ansatz'].nParameters())])

    # Define the objective function here
    # This is a default objective function using XACC VQE
    # that converges on the computed energy, computing the energy
    # every iteration
    @abstractmethod
    def energy(self, params):
        self.execParams['parameters'] = params.tolist()
        self.vqe_energy.initialize(self.execParams)
        self.vqe_energy.execute(self.buffer)

        e = self.buffer.getInformation("opt-val")

        if 'rdm-purification' in self.execParams['accelerator'].name():
            t = self.buffer.getAllUnique('parameters')
            ind = len(t) - 1
            children = self.buffer.getChildren('parameters', t[ind])
            e = children[1].getInformation('purified-energy')

        self.angles.append(','.join(map(str, params.tolist())))
        self.energies.append(e)
        fileName = ".persisted_buffer_%s" % (self.buffer.getInformation('accelerator')) if self.buffer.hasExtraInfoKey('accelerator') \
                                                                else ".persisted_buffer_%s" % (self.execParams['accelerator'].name())
        file = open(fileName+'.ab', 'w')
        file.write(str(self.buffer))
        file.close()
        return e


@ComponentFactory("scipy_opt_factory")
@Property("_vqe_optimizer", "vqe_optimizer", "scipy-opt")
@Property("_name", "name", "scipy-opt")
@Instantiate("scipy_opt_instance")
class ScipyOpt(VQEOpt):

    def optimize(self, buffer, optimizer_args, execParams):
        super().optimize(buffer, optimizer_args, execParams)

        from scipy.optimize import minimize
        opt_result = minimize(self.energy, self.init_args, **self.opt_args)

        # Optimizer adds the results to the buffer automatically
        buffer.addExtraInfo('vqe-energies', self.energies)
        buffer.addExtraInfo('vqe-parameters', self.angles)
        optimal_angles = [float(x) for x in self.angles[self.energies.index(min(self.energies))].split(",")]
        buffer.addExtraInfo('opt-params', optimal_angles)
        buffer.addExtraInfo('opt-val', min(self.energies))

    # Noticing something very weird if the objective energy function
    # resides in the super class; looks like it needs to be
    # redefined everytime (in an optimizer)
    def energy(self, params):
        self.execParams['parameters'] = params.tolist()
        self.vqe_energy.initialize(self.execParams)
        self.vqe_energy.execute(self.buffer)
        e = self.buffer.getInformation("opt-val")

        if 'rdm-purification' in self.execParams['accelerator'].name():
            t = self.buffer.getAllUnique('parameters')
            ind = len(t) - 1
            children = self.buffer.getChildren('parameters', t[ind])
            e = children[1].getInformation('purified-energy')

        self.angles.append(','.join(map(str, params.tolist())))
        self.energies.append(e)
        fileName = ".persisted_buffer_%s" % (self.buffer.getInformation('accelerator')) if self.buffer.hasExtraInfoKey('accelerator') \
                                                                else ".persisted_buffer_%s" % (self.execParams['accelerator'].name())
        file = open(fileName+'.ab', 'w')
        file.write(str(self.buffer))
        file.close()
        return e

@ComponentFactory("bobyqa_opt_factory")
@Property("_vqe_optimizer", "vqe_optimizer", "bobyqa-opt")
@Property("_name", "name", "bobyqa-opt")
@Instantiate("bobyqa_opt_instance")
class BOBYQAOpt(VQEOpt):

    def optimize(self, buffer, optimizer_args, execParams):
        super().optimize(buffer, optimizer_args, execParams)

        import pybobyqa

        if 'options' in self.opt_args:
            base_args = self.opt_args['options']
            opt_result = pybobyqa.solve(self.energy, self.init_args, **base_args)
        else:
            opt_result = pybobyqa.solve(self.energy, self.init_args, **self.opt_args)

        # Optimizer adds the results to the buffer automatically
        buffer.addExtraInfo('vqe-energies', self.energies)
        buffer.addExtraInfo('vqe-parameters', self.angles)
        optimal_angles = [float(x) for x in self.angles[self.energies.index(min(self.energies))].split(",")]
        buffer.addExtraInfo('opt-params', optimal_angles)
        buffer.addExtraInfo('opt-val', min(self.energies))

    # Noticing something very weird if the objective energy function
    # resides in the super class; looks like it needs to be
    # redefined everytime (in an optimizer)
    def energy(self, params):
        self.execParams['parameters'] = params.tolist()
        self.vqe_energy.initialize(self.execParams)
        self.vqe_energy.execute(self.buffer)

        e = self.buffer.getInformation("opt-val")

        if 'rdm-purification' in self.execParams['accelerator'].name():
            t = self.buffer.getAllUnique('parameters')
            ind = len(t) - 1
            children = self.buffer.getChildren('parameters', t[ind])
            e = children[1].getInformation('purified-energy')

        self.angles.append(','.join(map(str, params.tolist())))
        self.energies.append(e)
        fileName = ".persisted_buffer_%s" % (self.buffer.getInformation('accelerator')) if self.buffer.hasExtraInfoKey('accelerator') \
                                                                else ".persisted_buffer_%s" % (self.execParams['accelerator'].name())
        file = open(fileName+'.ab', 'w')
        file.write(str(self.buffer))
        file.close()
        return e



