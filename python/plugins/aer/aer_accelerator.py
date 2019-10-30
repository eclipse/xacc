import xacc
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

@ComponentFactory("aer_accelerator_factory")
@Provides("accelerator")
@Property("_accelerator", "accelerator", "aer")
@Property("_name", "name", "aer")
@Instantiate("aer_accelerator_instance")
class AerAccelerator(xacc.Accelerator):
    def __init__(self):
        xacc.Accelerator.__init__(self)
        self.shots = 1024
        self.backend = None

    def initialize(self, options):
        self.qobj_compiler = xacc.getCompiler('qobj')
        if 'shots' in options:
            self.shots = options['shots']

        if 'backend' in options:
            self.backend = options['backend']

    def name(self):
        return 'aer'

    def execute_one_qasm(self, buffer, program):
        qobjStr = self.qobj_compiler.translate(program)
        import json
        from qiskit import Aer
        from qiskit.qobj import (QasmQobj, QobjHeader,
                         QasmQobjInstruction,
                         QasmQobjExperiment, QasmQobjExperimentConfig, QobjExperimentHeader, QasmQobjConfig)
        qobj_json = json.loads(qobjStr)

        # Create the Experiments using qiskit data structures
        exps = [QasmQobjExperiment(config=QasmQobjExperimentConfig(memory_slots=e['config']['memory_slots'], n_qubits=e['config']['n_qubits']),
                    header=QobjExperimentHeader(clbit_labels=e['header']['clbit_labels'],creg_sizes=e['header']['creg_sizes'],
                                                memory_slots=e['header']['memory_slots'],n_qubits=e['header']['n_qubits'],
                                                name=e['header']['name'], qreg_sizes=e['header']['qreg_sizes'],
                                                qubit_labels=e['header']['qubit_labels']),
                    instructions=[QasmQobjInstruction(name=i['name'], qubits=i['qubits'], params=(i['params'] if 'params' in i else [])) if i['name'] != 'measure'
                                    else QasmQobjInstruction(name=i['name'], qubits=i['qubits'], memory=i['memory']) for i in e['instructions']]) for e in qobj_json['qObject']['experiments']]

        qobj = QasmQobj(qobj_id=qobj_json['qObject']['qobj_id'],
                        header=QobjHeader(), config=QasmQobjConfig(shots=self.shots, memory_slots=qobj_json['qObject']['config']['memory_slots']), experiments=exps, shots=self.shots)


        backend = Aer.get_backend('qasm_simulator')

        job_sim = backend.run(qobj)
        # job_sim = execute(qc, backend,
        #   coupling_map=coupling_map,
        #   noise_model=noise_model,
        #   basis_gates=basis_gates)
        sim_result = job_sim.result()
        [buffer.appendMeasurement(b,c) for b,c in sim_result.get_counts().items()]

    def execute(self, buffer, programs):

        # Translate IR to a Qobj Json String
        if isinstance(programs, list) and len(programs) > 1:
            for p in programs:
                print('running prog')
                tmpBuffer = xacc.qalloc(buffer.size())
                self.execute_one_qasm(tmpBuffer, p)
                buffer.appendChild('',tmpBuffer)
        else:
            if isinstance(programs, list):
                programs = programs[0]
            self.execute_one_qasm(buffer, programs)

