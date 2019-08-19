from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from ansatz_generator import AnsatzGenerator
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
        layers = 1
        connectivity = []
        for i in range(nQubits-1):
            connectivity.append((i,i+1))
        if 'layers' in inputParams:
            layers = int(inputParams['layers'])
        if 'connectivity' in inputParams:
            connectivity = inputParams['connectivity']
        hwe = xacc.getComposite('hwe')
        options = xacc.HeterogeneousMap()
        options.insert('layers',layers)
        options.insert('connectivity',connectivity)
        options.insert('nq', nQubits)

        hwe.expand(options)

        return hwe
