from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from hamiltonian_generator import HamiltonianGenerator
import ast
from xaccvqe import PauliOperator


@ComponentFactory("simpleH2_hamiltonian_generator_factory")
@Provides("hamiltonian_generator")
@Property("_hamiltonian_generator", "hamiltonian_generator", "simpleH2")
@Property("_name", "name", "simpleH2")
@Instantiate("simpleH2_hamiltonian_generator_instance")
class SimpleH2(HamiltonianGenerator):
    """
        HamiltonianGenerator for the H2 molecule.
        Constructed via a combination of XACC-VQE PauliOperator instances
    """

    def generate(self, inputParams):
        return PauliOperator(.2976) + \
               PauliOperator({0:'Z'},.3593) + \
               PauliOperator({1:'Z'},-.4826) + \
               PauliOperator({0:'Z',1:'Z'},.5818) + \
               PauliOperator({0:'Y',1:'Y'}, .0896) + \
               PauliOperator({0:'X',1:'X'},.0896)
