from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate
from ansatzgenerator import AnsatzGenerator
import ast
import xacc

@ComponentFactory("dh2_ansatz_generator_factory")
@Provides("ansatz_generator")
@Property("_ansatz_generator", "ansatz_generator", "dh2")
@Property("_name", "name", "dh2")
@Instantiate("dh2_ansatz_generator_instance")
class DH2(AnsatzGenerator):

    def generate(self, inputParams, nQubits):
        dh2_function = xacc.gate.createFunction('dh2', [x for x in range(nQubits)])
        ry = xacc.gate.create("Ry", [1],['t1'])
        cx = xacc.gate.create("CNOT", [1, 0])
        dh2_function.addInstruction(ry)
        dh2_function.addInstruction(cx)
        return dh2_function
