import xacc
from xacc import CompositeInstruction
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

@ComponentFactory("mitiq_decorator_factory")
@Provides("accelerator_decorator")
@Property("_accelerator_decorator", "accelerator_decorator", "mitiq")
@Property("_name", "name", "mitiq")
@Instantiate("mitiq_accelerator_decorator_instance")
class MitiqDecorator(xacc.AcceleratorDecorator):
    def __init__(self):
        xacc.AcceleratorDecorator.__init__(self)
        
    def initialize(self, options):
        self.openqasm_compiler = xacc.getCompiler('staq')
        self.openqasm_compiler.setExtraOptions({'no-optimize': True})
        return 

    def name(self):
        return 'mitiq'

    def execute_single(self, buffer, program):

        import mitiq
        from qiskit import QuantumCircuit
        def noisy_sim(circ : QuantumCircuit):
            # Convert circ to a CompositeInstruction
            out_src = circ.qasm()
            out_prog = self.openqasm_compiler.compile(out_src).getComposites()[0]

            # Execute on a tmp buffer
            tmp_buffer = xacc.qalloc(buffer.size())
            self.decoratedAccelerator.execute(tmp_buffer, out_prog)
            # tmp_buffer.addExtraInfo('exp-val-z', tmp_buffer.getExpectationValueZ())

            # record the noisy exp val
            buffer.addExtraInfo('noisy-exp-val-z', tmp_buffer.getExpectationValueZ())
            # buffer.appendChild('mitiq-noisy-exec-'+buffer.name(), tmp_buffer)
            return tmp_buffer.getExpectationValueZ()
        
        # easiest thing to do is map to Qiskit
        src = self.openqasm_compiler.translate(program).replace('\\','')

        # Create a QuantumCircuit
        circuit = QuantumCircuit.from_qasm_str(src)

        # print('New Circuit:\n', circuit.qasm())
        fixed_exp = mitiq.execute_with_zne(circuit, noisy_sim)
        # print(fixed_exp)
        buffer.addExtraInfo('exp-val-z', fixed_exp)
        return
    
    def execute(self, buffer, programs):
       print('[mitiq] executing Mitiq')
       # Translate IR to a Qobj Json String
       if isinstance(programs, list) and len(programs) > 1:
           for p in programs:
               tmpBuffer = xacc.qalloc(buffer.size())
               tmpBuffer.setName(p.name())
               self.execute_single(tmpBuffer, p)
               print('\t[mitiq] <', p.name(), '>_noisy = ', tmpBuffer['noisy-exp-val-z'] ,' -> <', p.name(), '>_fixed = ', tmpBuffer['exp-val-z'])
               buffer.appendChild(p.name(),tmpBuffer)
            #    print(tmpBuffer)
       else:
           if isinstance(programs, list):
               programs = programs[0]
           self.execute_single(buffer, programs)
    #    print('returning from mitiq')
       return

