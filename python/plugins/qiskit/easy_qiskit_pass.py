import xacc
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

@ComponentFactory("easy_qiskit_pass_factory")
@Provides("irtransformation")
@Property("_irtransformation", "irtransformation", "qiskit-cx-cancellation")
@Property("_name", "name", "qiskit-cx-cancellation")
@Instantiate("easy_qiskit_pass_instance")
class EasyQiskitIRTransformation(xacc.IRTransformation):
    def __init__(self):
        xacc.IRTransformation.__init__(self)

    def type(self):
        return xacc.IRTransformationType.Optimization

    def name(self):
        return 'qiskit-cx-cancellation'

    def apply(self, program, accelerator, options):
        # Import qiskit modules here so that users
        # who don't have qiskit can still use rest of xacc
        from qiskit import QuantumCircuit, transpile
        from qiskit.transpiler import PassManager
        from qiskit.transpiler.passes import CXCancellation

        # Map CompositeInstruction program to OpenQasm string
        openqasm_compiler = xacc.getCompiler('openqasm')
        src = openqasm_compiler.translate(program).replace('\\','')

        # Create a QuantumCircuit
        circuit = QuantumCircuit.from_qasm_str(src)

        # Create the PassManager and run the pass
        pass_manager = PassManager()
        pass_manager.append(CXCancellation())
        out_circuit = transpile(circuit, pass_manager=pass_manager)

        # Map the output to OpenQasm and map to XACC IR
        out_src = out_circuit.qasm()
        out_src = '__qpu__ void '+program.name()+'(qbit q) {\n'+out_src+"\n}"
        out_prog = openqasm_compiler.compile(out_src, accelerator).getComposites()[0]

        # update the given program CompositeInstruction reference
        program.clear()
        for inst in out_prog.getInstructions():
            program.addInstruction(inst)

        return








