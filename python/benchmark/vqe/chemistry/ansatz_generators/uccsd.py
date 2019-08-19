from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from ansatz_generator import AnsatzGenerator
import ast
import xacc

@ComponentFactory("uccsd_ansatz_generator_factory")
@Provides("ansatz_generator")
@Property("_ansatz_generator", "ansatz_generator", "uccsd")
@Property("_name", "name", "uccsd")
@Instantiate("uccsd_ansatz_generator_instance")
class UCCSD(AnsatzGenerator):
    """
        AnsatzGenerator for the UCCSD ansatz for calculating molecular energies

        Required input configurations:
            n-electrons | (2)
    """

    def generate(self, inputParams, nQubits):
        uccsd = xacc.getComposite('uccsd')
        options = xacc.HeterogeneousMap()
        options.insert('ne',int(inputParams['n-electrons']))
        options.insert('nq', nQubits)
        uccsd.expand(options)
        return uccsd
