import xacc
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

@ComponentFactory("cirq_accelerator_factory")
@Provides("accelerator")
@Property("_accelerator", "accelerator", "cirq")
@Property("_name", "name", "cirq")
@Instantiate("cirq_accelerator_instance")
class CirqAccelerator(xacc.Accelerator):
    def __init__(self):
        xacc.Accelerator.__init__(self)
        self.shots = 1024
        self.backend = 'simulator'

    def initialize(self, options):
        if 'shots' in options:
            self.shots = options['shots']


        if 'backend' in options:
            self.backend = options['backend']

    def name(self):
        return 'cirq'

    def execute_one_qasm(self, buffer, program):
        import cirq

        staq = xacc.getCompiler('staq')
        cirq_code = staq.translate(program, {'lang-type':'cirq'})
        cirq_code = cirq_code[0:len(cirq_code)-15]
        exec(cirq_code, globals(), globals())
        # Now we have a circuit variable for execution with Cirq
        simulator = cirq.Simulator()
        result = simulator.run(circuit, repetitions=self.shots)
        import re
        qbit2keys = {}
        for qq in q:
            m = re.search(r"\[([0-9_]+)\]", str(qq))
            qbit2keys[int(m.group(1))] = str(qq).replace('[', '_c[')

        bs = []
        for i in range(self.shots):
            b = [0 for i in range(buffer.size())]
            for k,v in qbit2keys.items():
                b[k] = result._measurements[v][i][0]
            bs.append(''.join([str(bb) for bb in b]))

        [buffer.appendMeasurement(bb) for bb in bs]


    def execute(self, buffer, programs):

        if isinstance(programs, list) and len(programs) > 1:
            for p in programs:
                tmpBuffer = xacc.qalloc(buffer.size())
                tmpBuffer.setName(p.name())
                self.execute_one_qasm(tmpBuffer, p)
                buffer.appendChild(p.name(),tmpBuffer)
        else:
            if isinstance(programs, list):
                programs = programs[0]
            self.execute_one_qasm(buffer, programs)

