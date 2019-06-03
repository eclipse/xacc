from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

from frozencore import FrozenCoreHamiltonian
import xacc
from xaccvqe import PauliOperator

@ComponentFactory("taperedfrozencore_hamiltonian_generator_factory")
@Provides("hamiltonian_generator")
@Property("_hamiltonian_generator", "hamiltonian_generator", "tapered-frozen-core")
@Property("_name", "name", "tapered-frozen-core")
@Instantiate("taperedfrozencore_hamiltonian_generator_instance")
class TaperedFrozenCoreHamiltonian(FrozenCoreHamiltonian):
    """
        HamiltonianGenerator for molecules using the frozen-core approximation of electronic structure

        Required input configurations | (example):
            basis | (sto-3g)
            geometry | ('0 1\nNa  0.000000   0.0      0.0\nH   0.0        0.0  1.914388\nsymmetry c1')
            frozen-spin-orbitals | [0, 1, 2, 3, 4, 10, 11, 12, 13, 14]
            active-spin-orbitals | [5, 9, 15, 19]
    """

    def generate(self, inputParams):
        op = super().generate(inputParams)
        t = xacc.getIRTransformation('qubit-tapering')
        i = op.toXACCIR()
        i = t.transform(i)
        op2 = PauliOperator()
        op2.fromXACCIR(i)
        return op2
