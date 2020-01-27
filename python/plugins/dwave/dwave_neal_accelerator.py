import xacc
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
    Validate, Invalidate, Instantiate

@ComponentFactory("dwave_neal_accelerator_factory")
@Provides("accelerator")
@Property("_accelerator", "accelerator", "dwave_neal")
@Property("_name", "name", "dwave-neal")
@Instantiate("dwave_neal_accelerator_instance")
class DwaveNealAccelerator(xacc.Accelerator):
    def __init__(self):
        xacc.Accelerator.__init__(self)
        self.shots = 1024

    def initialize(self, options):
        self.shots = 100
        self.mode = 'ising'
        if 'shots' in options:
            self.shots = options['shots']
        if 'mode' in options:
            self.mode = options['mode']

    def name(self):
        return 'dwave-neal'

    def execute(self, buffer, program):

        import neal
        from collections import Counter

        counter = 0
        h = {}
        J={}
        Q={}
        for inst in program.getInstructions():
            if inst.bits()[0] == inst.bits()[1]:
                h[counter] = inst.getParameter(0)
                Q[(counter, counter)] = inst.getParameter(0)
                counter+=1
            else:
                J[(inst.bits()[0], inst.bits()[1])] = inst.getParameter(0)
                Q[(inst.bits()[0], inst.bits()[1])] = inst.getParameter(0)

        sampler = neal.SimulatedAnnealingSampler()
        if self.mode == 'ising':
            response = sampler.sample_ising(h, J, num_reads=self.shots)
        elif self.mode == 'qubo':
            response = sampler.sample_qubo(Q, num_reads = self.shots)
        else:
            xacc.error('[dwave-neal] invalid execution mode: ' + self.mode)
        energies = [d.energy for d in response.data(['energy'])]
        counts_list = []
        unique_config_to_energy = {}
        for i, sample in enumerate(response):
            l = list(sample.values())
            st = ''
            for li in l:
                st += '1' if li == 1 else '0'
            counts_list.append(st)
            if not st in unique_config_to_energy:
                unique_config_to_energy[st] = energies[i]


        counts = Counter(counts_list)
        buffer.setMeasurements(counts)
        buffer.addExtraInfo('unique-configurations', unique_config_to_energy)
        gstate = min(unique_config_to_energy, key=unique_config_to_energy.get)
        buffer.addExtraInfo('ground_state', gstate)




