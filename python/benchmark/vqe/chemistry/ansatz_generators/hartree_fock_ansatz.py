from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from ansatzgenerator import AnsatzGenerator
import ast
import xacc

@ComponentFactory("hf_ansatz_generator_factory")
@Provides("ansatz_generator")
@Property("_ansatz_generator", "ansatz_generator", "hf")
@Property("_name", "name", "hf")
@Instantiate("hf_ansatz_generator_instance")
class HF(AnsatzGenerator):
    """
        AnsatzGenerator for calculating the Hartree-Fock state of molecules

        Required input configurations | (examples):
            x-gates | ([0,1])
    """

    def generate(self, inputParams, nQubits):
        hf_function = xacc.gate.createFunction('hf', [x for x in range(nQubits)])
        x_gate_qubits = ast.literal_eval(inputParams['x-gates'])
        for q in x_gate_qubits:
            x_gate = xacc.gate.create('X', [q])
            hf_function.addInstruction(x_gate)
        return hf_function
