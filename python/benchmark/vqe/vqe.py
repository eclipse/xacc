from pelix.ipopo.decorators import (ComponentFactory, Property, Instantiate)
import ast
import xacc
from xacc import BenchmarkAlgorithm
from vqe_base import VQEBase

@ComponentFactory("vqe_benchmark_factory")
@Property("_name", "name", "vqe")
@Instantiate("vqe_benchmark")
class VQE(VQEBase):
    """
        Algorithm class inherited from VQEBase to execute the VQE algorithm
    """
    def __init__(self):
        super().__init__()

    def execute(self, inputParams):
        """
            Inherited method with algorithm-specific implementation

            Parameters:
                inputParams - a dictionary of input parameters obtained from .ini file

        """
        super().execute(inputParams)
        # self.vqe_options_dict['task'] = 'vqe'
        # if self.optimizer is not None:
        #     self.optimizer.optimize(self.op, self.buffer, self.optimizer_options, self.vqe_options_dict)
        # else:
        execParams = {'accelerator': self.qpu, 'ansatz': self.ansatz, 'observable': self.op, 'optimizer':self.optimizer}
        vqe = xacc.getAlgorithm('vqe', execParams)
        vqe.execute(self.buffer)

        return self.buffer

    def analyze(self, buffer, inputParams):
        super().analyze(buffer, inputParams)
