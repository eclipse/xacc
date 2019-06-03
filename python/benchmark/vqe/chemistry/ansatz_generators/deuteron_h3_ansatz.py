from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from ansatzgenerator import AnsatzGenerator
import ast
import xacc

@ComponentFactory("dh3_ansatz_generator_factory")
@Provides("ansatz_generator")
@Property("_ansatz_generator", "ansatz_generator", "dh3")
@Property("_name", "name", "dh3")
@Instantiate("dh3_ansatz_generator_instance")
class DeuteronH3(AnsatzGenerator):

    def generate(self, inputParams, nQubits):
        function = xacc.gate.createFunction('ob', [x for x in range(nQubits)])
        x = xacc.gate.create('X', [0])
        ry = xacc.gate.create("Ry", [1], ['t1'])
        ry2 = xacc.gate.create('Ry', [2], ['t2'])
        cx = xacc.gate.create("CNOT", [2, 0])
        cx2 = xacc.gate.create("CNOT", [0, 1])
        ry3 = xacc.gate.create('Ry', [1], ['-t1'])
        cx3 = xacc.gate.create("CNOT", [0, 1])
        cx4 = xacc.gate.create("CNOT", [1, 0])
        function.addInstruction(x)
        function.addInstruction(ry)
        function.addInstruction(ry2)
        function.addInstruction(cx)
        function.addInstruction(cx2)
        function.addInstruction(ry3)
        function.addInstruction(cx3)
        function.addInstruction(cx4)
        return function
