from pelix.ipopo.decorators import (ComponentFactory, Property, Instantiate)
import ast
import configparser
import xacc
import time
import os
from xacc import BenchmarkAlgorithm
from vqe_base import VQEBase

@ComponentFactory("vqe-energy_benchmark_factory")
@Property("_name", "name", "vqe-energy")
@Instantiate("vqe-energy_benchmark")
class VQEEnergy(VQEBase):
    """
        Algorithm class inherited from VQEBase to execute the 'compute-energy' task of VQE
    """
    def __init__(self):
        super().__init__()

    def execute(self, inputParams):
        """
            Inherited method with algorithm-specific implementation

            Parameters:
                inputParams - a dictionary of input parameters obtained from .ini file

            - sets XACC VQE task to 'compute-energy'
        """
        super().execute(inputParams)

        algo = xacc.getAlgorithm("vqe-energy", self.vqe_options_dict)
        algo.execute(self.buffer)

        # if 'rdm-purification' in self.qpu.name():
        #     p = self.buffer.getAllUnique('parameters')
        #     child = self.buffer.getChildren('parameters', p[0])
        #     energy = child[1].getInformation('purified-energy')
        #     self.buffer.addExtraInfo('vqe-energy', energy)

        return self.buffer

    def analyze(self, buffer, inputParams):
        super().analyze(buffer, inputParams)
