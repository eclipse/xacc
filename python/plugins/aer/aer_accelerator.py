import xacc
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

@ComponentFactory("aer_accelerator_factory")
@Provides("accelerator")
@Property("_accelerator", "accelerator", "aer-py")
@Property("_name", "name", "aer-py")
@Instantiate("aer_accelerator_instance")
class AerAccelerator(xacc.Accelerator):
    def __init__(self):
        xacc.Accelerator.__init__(self)
        self.shots = 1024
        self.backend = None
        self.noise_model = None
        self.modeled_qpu = None

    def getProperties(self):
        if self.backend is not None:
            return self.modeled_qpu.getProperties()

    def initialize(self, options):
        self.qobj_compiler = xacc.getCompiler('qobj')
        if 'shots' in options:
            self.shots = options['shots']

        if 'backend' in options:
            self.backend = options['backend']
            import json
            from qiskit.providers.models.backendproperties import BackendProperties
            from qiskit.providers.aer import noise
            self.modeled_qpu = xacc.getAccelerator('ibm:'+self.backend)
            props = self.modeled_qpu.getProperties()
            jsonStr = props['total-json']
            # print("jsonStr: \n", jsonStr)
            properties = BackendProperties.from_dict(json.loads(jsonStr))
            ro_error = True if 'readout_error' in options and options['readout_error'] else False
            rel = True if 'thermal_relaxation' in options and options['thermal_relaxation'] else False
            ge = True if 'gate_error' in options and options['gate_error'] else False
            self.noise_model = noise.NoiseModel.from_backend(backend, readout_error=ro_error, thermal_relaxation=rel, gate_error=ge)

    def updateConfiguration(self, options):
        self.initialize(options)

    def getConnectivity(self):
        if self.modeled_qpu == None:
            return []
        else:
            return self.modeled_qpu.getConnectivity()
    
    def name(self):
        return 'aer-py'

    def execute_one_qasm(self, buffer, program):
        qobjStr = self.qobj_compiler.translate(program)
        import json
        from qiskit import Aer
        from qiskit.qobj.qobj import QasmQobjSchema
        from qiskit.qobj import (QasmQobj, QobjHeader,
                                 QasmQobjInstruction,
                                 QasmQobjExperiment, QasmQobjExperimentConfig,
                                 QobjExperimentHeader, QasmQobjConfig)


        qobj_json = json.loads(qobjStr)
        qobj_json['qObject']['config']['shots']=self.shots
        qobj = QasmQobjSchema().load(qobj_json['qObject'])
        exps = qobj.experiments
        measures = {}
        qobj.experiments[0].config.n_qubits = buffer.size()
        for i in exps[0].instructions:
            if i.name == "measure":
                measures[i.memory[0]] =i.qubits[0]

        backend = Aer.get_backend('qasm_simulator')

        if self.noise_model is not None:
            job_sim = backend.run(qobj, noise_model=self.noise_model)
        else:
            job_sim = backend.run(qobj)

        sim_result = job_sim.result()

        for b,c in sim_result.get_counts().items():
            bitstring = b
            if len(b) < buffer.size():
                tmp = ['0' for i in range(buffer.size())]
                for bit_loc, qubit in measures.items():
                    lb = list(b)
                    tmp[len(tmp)-1-qubit] = lb[len(b)-bit_loc-1]
                bitstring = ''.join(tmp)
            buffer.appendMeasurement(bitstring,c)

    def execute(self, buffer, programs):

        # Translate IR to a Qobj Json String
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

