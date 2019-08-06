from pelix.ipopo.decorators import (ComponentFactory, Property, Instantiate)
import ast
import configparser
import xacc
import time
import os
from xacc import BenchmarkAlgorithm
from vqe_base import VQEBase

@ComponentFactory("param-sweep_benchmark_factory")
@Property("_name", "name", "param-sweep")
@Instantiate("param-sweep_benchmark")
class ParamSweep(VQEBase):
    """
        Algorithm class inherited from VQEBase to execute the 'compute-energy' task of VQE
    """
    def __init__(self):
        super().__init__()

    def linspace(self, a, b, n=100):
        if n < 2:
            return b
        diff = (float(b) - a)/(n - 1)
        return [diff * i + a  for i in range(n)]

    def execute(self, inputParams):
        """
            Inherited method with algorithm-specific implementation

            Parameters:
                inputParams - a dictionary of input parameters obtained from .ini file

            - sets XACC VQE task to 'compute-energy'
            - executes a parameter-sweep
        """
        super().execute(inputParams)
        pi = 3.141592653589793
        self.vqe_options_dict['task'] = 'compute-energy'

        if inputParams['upper-bound'] == 'pi':
            up_bound = pi
        else:
            up_bound = ast.literal_eval(inputParams['upper-bound'])

        if inputParams['lower-bound'] == '-pi':
            low_bound = -pi
        else:
            low_bound = ast.literal_eval(inputParams['lower-bound'])

        num_params = ast.literal_eval(inputParams['num-params'])

        self.energies = []
        self.angles = []
        for param in self.linspace(low_bound, up_bound, num_params):
            self.angles.append(param)
            self.vqe_options_dict['vqe-params'] = str(param)
            energy = xaccvqe.execute(self.op, self.buffer, **self.vqe_options_dict).energy

            if 'rdm-purification' in self.qpu.name():
                p = self.buffer.getAllUnique('parameters')
                ind = len(p) - 1
                children = self.buffer.getChildren('parameters', p[ind])
                energy = children[1].getInformation('purified-energy')

            self.energies.append(energy)

        self.buffer.addExtraInfo('vqe-energies', self.energies)
        self.buffer.addExtraInfo('vqe-angles', self.angles)
        self.buffer.addExtraInfo('vqe-energy', min(self.energies))
        return self.buffer

    def analyze(self, buffer, inputParams):
        super().analyze(buffer, inputParams)
