import xacc
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

@ComponentFactory("pyzx_pass_factory")
@Provides("irtransformation")
@Property("_irtransformation", "irtransformation", "pyzx")
@Property("_name", "name", "pyzx")
@Instantiate("pyzx_pass_instance")
class PyzxIRTransformation(xacc.IRTransformation):
    def __init__(self):
        xacc.IRTransformation.__init__(self)

    def type(self):
        return xacc.IRTransformationType.Optimization

    def name(self):
        return 'pyzx'

    def apply(self, program, accelerator, options):
        # Import qiskit modules here so that users
        # who don't have qiskit can still use rest of xacc
        from pyzx.circuit import Circuit
        from pyzx import simplify, extract, optimize

        # Map CompositeInstruction program to OpenQasm string
        openqasm_compiler = xacc.getCompiler('staq')
        src = openqasm_compiler.translate(program).replace('\\','')
      
        measures = []
        lines = src.split('\n')
        for l in lines:
            if 'measure' in l or 'creg' in l:
                measures.append(l)
        
        c = Circuit.from_qasm(src)
        g = c.to_graph()
        
        simplify.full_reduce(g,quiet=True)
        c2 = extract.extract_circuit(g)
        c3 = optimize.basic_optimization(c2.to_basic_gates())
        c3 = c3.to_basic_gates()
        c3 = c3.split_phase_gates()
        output = c3.to_qasm()
        for measure in measures:
            output += '\n' + measure

        # Map the output to OpenQasm and map to XACC IR
        out_prog = openqasm_compiler.compile(output, accelerator).getComposites()[0]

        # update the given program CompositeInstruction reference
        program.clear()
        for inst in out_prog.getInstructions():
            program.addInstruction(inst)

        return








