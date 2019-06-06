from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from hamiltonian_generator import HamiltonianGenerator
import ast, xacc
import xaccvqe

try:

    from openfermion.hamiltonians import MolecularData
    from openfermionpsi4 import run_psi4
    from openfermion.transforms import get_fermion_operator
except:
    pass


@ComponentFactory("Psi4OF_hamiltonian_generator_factory")
@Provides("hamiltonian_generator")
@Property("_hamiltonian_generator", "hamiltonian_generator", "psi4Of")
@Property("_name", "name", "psi4Of")
@Instantiate("Psi4OF_hamiltonian_generator_instance")
class Psi4OpenFermion(HamiltonianGenerator):
    """
        HamiltonianGenerator for calculating molecular energies using OpenFermion and Psi4

        Required input configurations | (examples)
            basis | (sto-3g)
            geometry | [('H', (0, 0, 0)), ('H', (0, 0, .7474))]
            charge | (0)
            multiplicity | (1)
    """

    def generate(self, inputParams):
        geom = ast.literal_eval(inputParams['geometry']) if isinstance(inputParams['geometry'],str) else inputParams['geometry']
        mdata = MolecularData(geom,
                              inputParams['basis'],
                              int(inputParams['multiplicity']),
                              int(inputParams['charge']))
        molecule = run_psi4(mdata, run_scf=1, run_fci=1)

        from openfermion.transforms import get_sparse_operator, jordan_wigner
        from openfermion.utils import sparse_eigenspectrum
        from scipy.sparse import find
        from scipy.optimize import minimize
        fermiOp = get_fermion_operator(molecule.get_molecular_hamiltonian())

        qop = jordan_wigner(fermiOp)

        inputParams['fci'] = molecule.fci_energy
        inputParams['hf'] = molecule.hf_energy
        return xaccvqe.QubitOperator2XACC(qop)
