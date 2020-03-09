import xacc, os
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

@ComponentFactory("dwave_accelerator_factory")
@Provides("accelerator")
@Property("_accelerator", "accelerator", "dwave-ocean-sdk")
@Property("_name", "name", "dwave-ocean-sdk")
@Instantiate("dwave_py_accelerator_instance")
class DwaveAccelerator(xacc.Accelerator):
    def __init__(self):
        xacc.Accelerator.__init__(self)
        self.shots = 1024
        self.token = ''
        self.use_exact = False
        self.chain_strength = 1.0

    def initialize(self, options):
        config = open(os.getenv('HOME')+'/.dwave_config', 'r').read()
        lines = config.split('\n')
        for l in lines:
            if 'key' in l:
                self.token = l.split(':')[1].strip()
        if 'shots' in options:
            self.shots = options['shots']
        if 'backend' in options:
            self.backend = options['backend']
        else:
            xacc.error('[Dwave Backend] Must set backend.')
        if 'chain_strength' in options:
            self.chain_strength = options['chain_strength']

    def name(self):
        return 'dwave-ocean-sdk'

    def execute(self, buffer, program):
        from dwave.system.samplers import DWaveSampler
        from dwave.system.composites import EmbeddingComposite, FixedEmbeddingComposite
        from collections import Counter
        import dimod

        h = {}
        J={}
        Q={}
        for inst in program.getInstructions():
            Q[(inst.bits()[0], inst.bits()[1])] = inst.getParameter(0)
            if inst.bits()[0] == inst.bits()[1]:
                h[inst.bits()[0]] = inst.getParameter(0)
            else:
                J[(inst.bits()[0], inst.bits()[1])] = inst.getParameter(0)

        buffer.setSize(max(h.keys())+1)

        if buffer.hasExtraInfoKey('embedding'):
            sampler = FixedEmbeddingComposite(DWaveSampler(token=self.token, solver=self.backend), buffer['embedding'])
        else:
            sampler = EmbeddingComposite(DWaveSampler(token=self.token, solver=self.backend))

        if program.getTag() == "ising":
            response = sampler.sample_ising(h, J, num_reads=self.shots, return_embedding=True)
        else:
            response = sampler.sample_qubo(Q, chain_strength=self.chain_strength, num_reads=self.shots, return_embedding=True)

        if not buffer.hasExtraInfoKey('embedding'):
            # we used embeddingcomposite, get the computed
            # embedding
            emb = response.info['embedding_context']['embedding']
            buffer.addExtraInfo('embedding', emb)

        counts_list = []
        e_map = {}
        for r in response.record:
            sample, energy, nocc, _ = r
            bitstring = ''.join([str(s if s == 1 else 0) for s in sample])
            for i in range(nocc):
                counts_list.append(bitstring)
            e_map[bitstring] = energy

        counts = Counter(counts_list)
        buffer.setMeasurements(counts)
        buffer.addExtraInfo('energies', e_map)




