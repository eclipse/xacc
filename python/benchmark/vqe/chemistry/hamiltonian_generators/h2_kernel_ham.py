from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from hamiltonian_generator import HamiltonianGenerator
import ast, xacc


@ComponentFactory("xaccKernelH2_hamiltonian_generator_factory")
@Provides("hamiltonian_generator")
@Property("_hamiltonian_generator", "hamiltonian_generator", "xaccKernelH2")
@Property("_name", "name", "xaccKernelH2")
@Instantiate("xaccKernelH2_hamiltonian_generator_instance")
class XACCKernelH2(HamiltonianGenerator):
    """
        HamiltonianGenerator for the H2 molecular Hamiltonian.

        Constructed via XACC quantum source code
    """

    def generate(self, inputParams):
        src = """0.7080240949826064
- 1.248846801817026 0^ 0
- 1.248846801817026 1^ 1
- 0.4796778151607899 2^ 2
- 0.4796778151607899 3^ 3
+ 0.33667197218932576 0^ 1^ 1 0
+ 0.0908126658307406 0^ 1^ 3 2
+ 0.09081266583074038 0^ 2^ 0 2
+ 0.331213646878486 0^ 2^ 2 0
+ 0.09081266583074038 0^ 3^ 1 2
+ 0.331213646878486 0^ 3^ 3 0
+ 0.33667197218932576 1^ 0^ 0 1
+ 0.0908126658307406 1^ 0^ 2 3
+ 0.09081266583074038 1^ 2^ 0 3
+ 0.331213646878486 1^ 2^ 2 1
+ 0.09081266583074038 1^ 3^ 1 3
+ 0.331213646878486 1^ 3^ 3 1
+ 0.331213646878486 2^ 0^ 0 2
+ 0.09081266583074052 2^ 0^ 2 0
+ 0.331213646878486 2^ 1^ 1 2
+ 0.09081266583074052 2^ 1^ 3 0
+ 0.09081266583074048 2^ 3^ 1 0
+ 0.34814578469185886 2^ 3^ 3 2
+ 0.331213646878486 3^ 0^ 0 3
+ 0.09081266583074052 3^ 0^ 2 1
+ 0.331213646878486 3^ 1^ 1 3
+ 0.09081266583074052 3^ 1^ 3 1
+ 0.09081266583074048 3^ 2^ 0 1
+ 0.34814578469185886 3^ 2^ 2 3"""
        inputParams['rdm-source'] = src
        return xacc.getObservable('fermion', src)
