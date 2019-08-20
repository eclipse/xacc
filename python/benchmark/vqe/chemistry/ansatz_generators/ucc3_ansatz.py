from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from ansatz_generator import AnsatzGenerator
import ast
import xacc

@ComponentFactory("ucc3_ansatz_generator_factory")
@Provides("ansatz_generator")
@Property("_ansatz_generator", "ansatz_generator", "ucc3")
@Property("_name", "name", "ucc3")
@Instantiate("ucc3_ansatz_generator_instance")
class UCC3(AnsatzGenerator):

    def generate(self, inputParams, nQubits):
        ucc3_function = xacc.gate.createComposite('ucc3', ['t1','t2','t3'])
        x_gate_qubits = inputParams['x-gates'] if 'x-gates' in inputParams else [0,2]
        if isinstance(inputParams['x-gates'] ,str):
           x_gate_qubits = ast.literal_eval(inputParams['x-gates'])
        for q in x_gate_qubits:
            x_gate = xacc.gate.create('X', [q])
            ucc3_function.addInstruction(x_gate)
        for q in x_gate_qubits:
            x_gate = xacc.gate.create('X', [q])
            ucc3_function.addInstruction(x_gate)
        rx0 = xacc.gate.create("Rx", [0], [1.57079632])
        h0 = xacc.gate.create("H", [1])
        cn0 = xacc.gate.create("CNOT", [0,1])
        rz0 = xacc.gate.create("Rz", [1], ['t1'])
        rx1 = xacc.gate.create("Rx", [2], [1.57079632])
        h1 = xacc.gate.create("H", [3])
        cn1 = xacc.gate.create("CNOT", [2,3])
        rz1 = xacc.gate.create("Rz", [3], ['t2'])
        cn2 = xacc.gate.create("CNOT", [2,3])
        rx2 = xacc.gate.create("Rx", [2], [-1.57079632])
        h2 = xacc.gate.create("H", [2])
        cn3 = xacc.gate.create("CNOT", [1,2])
        cn4 = xacc.gate.create("CNOT", [2,3])
        rz2 = xacc.gate.create("Rz", [3], ['t3'])
        cn5 = xacc.gate.create("CNOT", [2,3])
        cn6 = xacc.gate.create("CNOT", [1,2])
        cn7 = xacc.gate.create("CNOT", [0,1])
        rx3 = xacc.gate.create("Rx", [0], [-1.57079632])
        h4 = xacc.gate.create("H", [1])
        h5 = xacc.gate.create("H", [2])
        h6 = xacc.gate.create("H", [3])
        ucc3_function.addInstruction(x0)
        ucc3_function.addInstruction(x1)
        ucc3_function.addInstruction(rx0)
        ucc3_function.addInstruction(h0)
        ucc3_function.addInstruction(cn0)
        ucc3_function.addInstruction(rz0)
        ucc3_function.addInstruction(rx1)
        ucc3_function.addInstruction(h1)
        ucc3_function.addInstruction(cn1)
        ucc3_function.addInstruction(rz1)
        ucc3_function.addInstruction(cn2)
        ucc3_function.addInstruction(rx2)
        ucc3_function.addInstruction(h2)
        ucc3_function.addInstruction(cn3)
        ucc3_function.addInstruction(cn4)
        ucc3_function.addInstruction(rz2)
        ucc3_function.addInstruction(cn5)
        ucc3_function.addInstruction(cn6)
        ucc3_function.addInstruction(cn7)
        ucc3_function.addInstruction(rx3)
        ucc3_function.addInstruction(h4)
        ucc3_function.addInstruction(h5)
        ucc3_function.addInstruction(h6)
        return ucc3_function
