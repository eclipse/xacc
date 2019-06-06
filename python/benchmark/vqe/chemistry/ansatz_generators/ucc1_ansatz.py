from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from ansatzgenerator import AnsatzGenerator
import ast
import xacc

@ComponentFactory("ucc1_ansatz_generator_factory")
@Provides("ansatz_generator")
@Property("_ansatz_generator", "ansatz_generator", "ucc1")
@Property("_name", "name", "ucc1")
@Instantiate("ucc1_ansatz_generator_instance")
class UCC1(AnsatzGenerator):
    """
        AnsatzGenerator for the UCC1 ansatz for calculating molecular energies

        Required input configurations:
            x-gates | ([0, 2])
    """

    def generate(self, inputParams, nQubits):
        ucc1_function = xacc.gate.createFunction('reduced-uccsd', [x for x in range(nQubits)])
        x_gate_qubits = ast.literal_eval(inputParams['x-gates'])
        for q in x_gate_qubits:
            x_gate = xacc.gate.create('X', [q])
            ucc1_function.addInstruction(x_gate)
        rx1 = xacc.gate.create("Rx", [0], [1.57079632])
        h1 = xacc.gate.create("H", [1])
        h2 = xacc.gate.create("H", [2])
        h3 = xacc.gate.create("H", [3])
        cn1 = xacc.gate.create("CNOT", [0,1])
        cn2 = xacc.gate.create("CNOT", [1,2])
        cn3 = xacc.gate.create("CNOT", [2,3])
        rz = xacc.gate.create("Rz", [3], ['t1'])
        cn4 = xacc.gate.create("CNOT", [2,3])
        cn5 = xacc.gate.create("CNOT", [1,2])
        cn6 = xacc.gate.create("CNOT", [0,1])
        rx2 = xacc.gate.create("Rx", [0], [-1.57079632])
        h4 = xacc.gate.create("H", [1])
        h5 = xacc.gate.create("H", [2])
        h6 = xacc.gate.create("H", [3])
        ucc1_function.addInstruction(rx1)
        ucc1_function.addInstruction(h1)
        ucc1_function.addInstruction(h2)
        ucc1_function.addInstruction(h3)
        ucc1_function.addInstruction(cn1)
        ucc1_function.addInstruction(cn2)
        ucc1_function.addInstruction(cn3)
        ucc1_function.addInstruction(rz)
        ucc1_function.addInstruction(cn4)
        ucc1_function.addInstruction(cn5)
        ucc1_function.addInstruction(cn6)
        ucc1_function.addInstruction(rx2)
        ucc1_function.addInstruction(h4)
        ucc1_function.addInstruction(h5)
        ucc1_function.addInstruction(h6)
        return ucc1_function
