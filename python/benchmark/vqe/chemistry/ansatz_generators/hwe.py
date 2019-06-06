from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from ansatzgenerator import AnsatzGenerator
import ast
import xacc

@ComponentFactory("hwe_ansatz_generator_factory")
@Provides("ansatz_generator")
@Property("_ansatz_generator", "ansatz_generator", "hwe")
@Property("_name", "name", "hwe")
@Instantiate("hwe_ansatz_generator_instance")
class HWE(AnsatzGenerator):
    """
        AnsatzGenerator for the hardware-efficient ansatz to calculate molecular energies

        Required input configurations | (examples)
            layers | (1)
            connectivity | [[0,1],[1,2],[2,3]]
    """

    def generate(self, inputParams, nQubits):
        ir_generator = xacc.getIRGenerator(inputParams['name'])
        function = ir_generator.generate([int(inputParams['layers']), nQubits, inputParams['connectivity']])
        return function
