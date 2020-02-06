import xacc
import ast

from xacc import Benchmark
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
Validate, Invalidate, Instantiate


@ComponentFactory("qpt_benchmark_factory")
@Provides("benchmark")
@Property("_benchmark", "benchmark", "qpt")
@Property("_name", "name", "qpt")
@Instantiate("qpt_benchmark_instance")
class QPT(Benchmark):

    def __init__(self):
        self.circuit_name = None
        self.qpt = None
        self.nq = None
        self.qpu = None

    def execute(self, inputParams):
        xacc_opts = inputParams['XACC']
        acc_name = xacc_opts['accelerator']

        if 'verbose' in xacc_opts and xacc_opts['verbose']:
            xacc.set_verbose(True)

        if 'Benchmark' not in inputParams:
            xacc.error('Invalid benchmark input - must have Benchmark description')

        if 'Circuit' not in inputParams:
            xacc.error('Invalid benchmark input - must have circuit description')

        self.qpu = xacc.getAccelerator(acc_name, xacc_opts)
        if 'Decorators' in inputParams:
            if 'readout_error' in inputParams['Decorators']:
                qpu = xacc.getAcceleratorDecorator('ro-error', qpu)


        provider = xacc.getIRProvider('quantum')

        if 'source' in inputParams['Circuit']:
            # here assume this is xasm always
            src = inputParams['Circuit']['source']
            xacc.qasm(src)
            # get the name of the circuit
            circuit_name = None
            for l in src.split('\n'):
                if '.circuit' in l:
                    circuit_name = l.split(' ')[1]
            self.circuit_name = circuit_name
            ansatz = xacc.getCompiled(circuit_name)

        opts = {'circuit':ansatz, 'accelerator':self.qpu}
        if 'qubit-map' in inputParams['Circuit']:
            opts['qubit-map'] = ast.literal_eval(inputParams['Circuit']['qubit-map'])

        self.qpt = xacc.getAlgorithm('qpt', opts)

        self.nq = ansatz.nLogicalBits()

        buffer = xacc.qalloc(ansatz.nLogicalBits())

        self.qpt.execute(buffer)
        return buffer


    def analyze(self, buffer, inputParams):
        import numpy as np, ast

        # Can get the chi process matrix and print it
        chi_real_part = np.array(buffer['chi-real'])
        chi_imag_part = np.array(buffer['chi-imag'])
        chi = np.reshape(chi_real_part + 1j*chi_imag_part, (2**(2*self.nq), 2**(2*self.nq)))
        print('\nComputed Chi Process:\n', chi)

        if 'fidelity' in inputParams['Benchmark']['analysis']:
            opts = {'chi-theoretical-real':ast.literal_eval(inputParams['Benchmark']['chi-theoretical-real'])}
            if 'chi-theoretical-imag' in inputParams['Benchmark']:
                opts['chi-theoretical-imag'] = ast.literal_eval(inputParams['Benchmark']['chi-theoretical-imag'])

            # Compute the fidelity with respect
            F = self.qpt.calculate('fidelity', buffer, opts)
            print('\nComputed Process Fidelity: ', F)

        if 'heat-maps' in inputParams['Benchmark']['analysis']:
            import matplotlib.pyplot as plt
            from matplotlib import rc
            import numpy as np, itertools

            pauli_labels = [''.join(l) for l in list(itertools.product(['I','X','Y','Z'], repeat=self.nq))]
            # Basic Configuration
            fig, axes = plt.subplots(ncols=2, figsize=(12, 12))
            ax1, ax2 = axes
            rc('text', usetex=True)

            # Heat maps.
            im1 = ax1.matshow(np.real(chi), cmap='viridis',interpolation='nearest')
            im2 = ax2.matshow(np.abs(np.real(chi) - np.reshape(np.array(ast.literal_eval(inputParams['Benchmark']['chi-theoretical-real'])), (2**(2*self.nq), 2**(2*self.nq)))), cmap='viridis')

            # Formatting for heat map 1.
            ax1.set_xticks(range(2**(2*self.nq)))
            ax1.set_yticks(range(2**(2*self.nq)))
            ax1.set_xticklabels(pauli_labels)
            ax1.set_yticklabels(pauli_labels)
            ax1.set_title(r'Re($\chi_{m,n}$)', y=-0.1)
            plt.setp(ax1.get_xticklabels(), rotation=45, ha='left', rotation_mode='anchor')
            plt.colorbar(im1, fraction=0.045, pad=0.05, ax=ax1)
            ax1.set_ylim(chi.shape[0]-0.5, -0.5)

            # Formatting for heat map 2.
            ax2.set_xticks(range(2**(2*self.nq)))
            ax2.set_yticks(range(2**(2*self.nq)))
            ax2.set_xticklabels(pauli_labels)
            ax2.set_yticklabels(pauli_labels)
            ax2.set_title('|Re($\chi_{m,n}$) - Re($\chi$)|', y=-0.1)
            plt.setp(ax2.get_xticklabels(), rotation=45, ha='left', rotation_mode='anchor')
            plt.colorbar(im2, fraction=0.045, pad=0.05, ax=ax2)
            ax2.set_ylim(chi.shape[0]-0.5, -0.5)

            fig.suptitle(self.circuit_name + ' quantum process tomography on ' + self.qpu.name())
            fig.tight_layout()
            from datetime import datetime
            now = datetime.now() # current date and time
            date_time = now.strftime("%m_%d_%Y_%H_%M_%S")
            plt.savefig('qpt_{}_{}.png'.format(self.circuit_name, date_time))
            print('\nHeat-maps output to qpt_{}_{}.png\n'.format(self.circuit_name, date_time))

