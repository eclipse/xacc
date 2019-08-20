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

        if not isinstance(self.optimizer,xacc.Optimizer):
            self.optimizer.optimize(self.buffer, self.optimizer_options, self.vqe_options_dict)
        else:
            vqe = xacc.getAlgorithm('vqe', self.vqe_options_dict)
            vqe.execute(self.buffer)

        return self.buffer

    def analyze(self, buffer, inputParams):
        super().analyze(buffer, inputParams)
