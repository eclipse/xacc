from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from hamiltonian_generator import HamiltonianGenerator
import ast, xacc

@ComponentFactory("deuteronH2_hamiltonian_generator_factory")
@Provides("hamiltonian_generator")
@Property("_hamiltonian_generator", "hamiltonian_generator", "deuteronH2")
@Property("_name", "name", "deuteronH2")
@Instantiate("deuteronH2_hamiltonian_generator_instance")
class SimpleH2(HamiltonianGenerator):
    """
    """
    def generate(self, inputParams):
        return xacc.getObservable('pauli', '5.907 - 2.1433 X0X1 - 2.1433 Y0Y1 + .21829 Z0 - 6.125 Z1')
