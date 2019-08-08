from xacc import BenchmarkAlgorithm
import xacc
import ast
import time
import os
from pelix.ipopo.decorators import (Provides, Requires, BindField, UnbindField)
#
#    VQEBase is an abstract class that implements the VQE algorithm using XACC.
#    VQE and VQEEnergy are iPOPO bundles that inherit from this class to execute the algorithm and analyze results.
#
#    Methods:
#        bind_dicts - Required for using iPOPO service registry
#        unbind_dicts - Required for using iPOPO service registry
#        execute - executes the VQE algorithm according to input configurations
#        analyze - analyzes the AcceleratorBuffer produced from execute()
@Provides("benchmark_algorithm")
@Requires("_ansatz_generators", "ansatz_generator", aggregate=True)
@Requires("_hamiltonian_generators", "hamiltonian_generator", aggregate=True)
@Requires("_vqe_optimizers", "vqe_optimization", aggregate=True, optional=True)
class VQEBase(BenchmarkAlgorithm):

    def __init__(self):
        self.hamiltonian_generators = {}
        self.ansatz_generators = {}
        self.vqe_optimizers = {}
        self.vqe_options_dict = {}
        self.n_qubits = 0
        self.buffer = None
        self.ansatz = None
        self.op = None
        self.qpu = None

    @BindField('_vqe_optimizers')
    @BindField('_ansatz_generators')
    @BindField('_hamiltonian_generators')
    def bind_dicts(self, field, service, svc_ref):
        """
        This method is intended to be inherited by iPOPO bundle subclasses.
        Binds the service references of the available molecule and ansatz generators.
        """
        if svc_ref.get_property('hamiltonian_generator'):
            generator = svc_ref.get_property('hamiltonian_generator')
            self.hamiltonian_generators[generator] = service
        if svc_ref.get_property('ansatz_generator'):
            generator = svc_ref.get_property('ansatz_generator')
            self.ansatz_generators[generator] = service
        if svc_ref.get_property('vqe_optimizer'):
            optimizer = svc_ref.get_property('vqe_optimizer')
            self.vqe_optimizers[optimizer] = service

    @UnbindField("_vqe_optimizers")
    @UnbindField('_ansatz_generators')
    @UnbindField('_hamiltonian_generators')
    def unbind_dicts(self, field, service, svc_ref):
        """
            This method is intended to be inherited by iPOPO bundle subclasses.
            Unbinds the service references of the available molecule and ansatz generators when the instance is killed
        """
        if svc_ref.get_property('hamiltonian_generator'):
            generator = svc_ref.get_property('hamiltonian_generator')
            del self.hamiltonian_generators[generator]
        if svc_ref.get_property('ansatz_generator'):
            generator = svc_ref.get_property('ansatz_generator')
            del self.ansatz_generators[generator]
        if svc_ref.get_property('vqe_optimizer'):
            optimizer = svc_ref.get_property('vqe_optimizer')
            del self.vqe_optimizers[optimizer]

    def execute(self, inputParams):
        """
        This method is intended to be inherited by vqe and vqe_energy subclasses to allow algorithm-specific implementation.
        This superclass method adds extra information to the buffer and allows XACC settings options to be set before executing VQE.

        Parameters:
        inputParams : dictionary
                    a dictionary of input parameters obtained from .ini file

        return QPU Accelerator buffer

        Options used (obtained from inputParams):
            'qubit-map': map of logical qubits to physical qubits
            'n-execs': number of sampler executions of measurements
            'initial-parameters': list of initial parameters for the VQE algorithm

            'restart-from-file': AcceleratorDecorator option to allow restart of VQE algorithm
            'readout-error': AcceleratorDecorator option for readout-error mitigation

        """
        self.qpu = xacc.getAccelerator(inputParams['accelerator'])
        xaccOp = self.hamiltonian_generators[inputParams['hamiltonian-generator']].generate(
            inputParams)
        self.ansatz = self.ansatz_generators[inputParams['name']].generate(
            inputParams, xaccOp.nBits())
        if 'qubit-map' in inputParams:
            qubit_map = ast.literal_eval(inputParams['qubit-map'])
            xaccOp, self.ansatz, n_qubits = xaccvqe.mapToPhysicalQubits(
                xaccOp, self.ansatz, qubit_map)
        else:
            n_qubits = xaccOp.nBits()
        self.op = xaccOp
        self.n_qubits = n_qubits
        self.buffer = self.qpu.createBuffer('q', n_qubits)
        self.buffer.addExtraInfo('hamiltonian', str(xaccOp))
        self.buffer.addExtraInfo('ansatz-qasm', self.ansatz.toString('q').replace('\\n', '\\\\n'))
        pycompiler = xacc.getCompiler('xacc-py')
        self.buffer.addExtraInfo('ansatz-qasm-py', '\n'.join(pycompiler.translate('q',self.ansatz).split('\n')[1:]))
        self.optimizer = None
        self.optimizer_options = {}
        if 'optimizer' in inputParams and inputParams['optimizer'] in self.vqe_optimizers:
            self.optimizer = self.vqe_optimizers[inputParams['optimizer']]
            if 'method' in inputParams:
                self.optimizer_options['method'] = inputParams['method']
            if 'options' in inputParams:
                self.optimizer_options['options'] = ast.literal_eval(inputParams['options'])
            if 'user-params' in inputParams:
                self.optimizer_options['options']['user_params'] = ast.literal_eval(inputParams['user-params'])
        else:
            xacc.info("No classical optimizer specified. Setting to default XACC optimizer.")
            if 'options' in inputParams:
                self.optimizer_options['options'] = ast.literal_eval(inputParams['options'])
            self.optimizer = xacc.getOptimizer('nlopt', self.optimizer_options)

        self.buffer.addExtraInfo('accelerator', inputParams['accelerator'])
        if 'n-execs' in inputParams:
            xacc.setOption('sampler-n-execs', inputParams['n-execs'])
            self.qpu = xacc.getAcceleratorDecorator('improved-sampling', self.qpu)

        if 'restart-from-file' in inputParams:
            xacc.setOption('vqe-restart-file', inputParams['restart-from-file'])
            self.qpu = xacc.getAcceleratorDecorator('vqe-restart',self.qpu)
            self.qpu.initialize()

        if 'readout-error' in inputParams and inputParams['readout-error']:
            self.qpu = xacc.getAcceleratorDecorator('ro-error',self.qpu)

        if 'rdm-purification' in inputParams and inputParams['rdm-purification']:
            self.qpu = xacc.getAcceleratorDecorator('rdm-purification', self.qpu)

        self.vqe_options_dict = {'accelerator': self.qpu, 'ansatz': self.ansatz}

        if 'initial-parameters' in inputParams:
            self.vqe_options_dict['vqe-params'] = ','.join([str(x) for x in ast.literal_eval(inputParams['initial-parameters'])])

        xacc.setOptions(inputParams)

    def analyze(self, buffer, inputParams):
        """
        This method is also to be inherited by vqe and vqe_energy subclasses to allow for algorithm-specific implementation.

        This superclass method always generates a .csv file with measured expectation values for each kernel and calculated energy of each iteration.

        Parameters:
        inputParams : dictionary
                    a dictionary of input parameters obtained from .ini file
        buffer : XACC AcceleratorBuffer
                AcceleratorBuffer containing VQE results to be analyzed

        Options used (in inputParams):
            'readout-error': generate .csv file with readout-error corrected expectation values and calculated energy for each kernel and iteration.
            'richardson-extrapolation': run Richardson-Extrapolation on the resulting Accelerator buffer (generating 4 more .csv files of expectation values and energies)
            'rich-extra-iter': the number of iterations of Richardson-Extrapolation
        """
        ps = buffer.getAllUnique('parameters')
        timestr = time.strftime("%Y%m%d-%H%M%S")
        exp_csv_name = "%s_%s_%s_%s" % (os.path.splitext(buffer.getInformation('file-name'))[0],
                                        buffer.getInformation('accelerator'),"exp_val_z",
                                        timestr)
        f = open(exp_csv_name+".csv", 'w')
        exp_columns = [c.getInformation('kernel') for c in buffer.getChildren('parameters',ps[0])] + ['<E>']
        f.write(str(exp_columns).replace('[','').replace(']','') + '\n')
        for p in ps:
            energy = 0.0
            for c in buffer.getChildren('parameters', p):
                exp = c.getInformation('exp-val-z')
                energy += exp * c.getInformation('coefficient') if c.hasExtraInfoKey('coefficient') else 0.0
                f.write(str(exp)+',')
            f.write(str(energy)+'\n')
        f.close()
        if 'readout-error' in inputParams:
            ro_exp_csv_name = "%s_%s_%s_%s" % (os.path.splitext(buffer.getInformation('file-name'))[0],
                                        buffer.getInformation('accelerator'),"ro_fixed_exp_val_z",
                                        timestr)
            f = open(ro_exp_csv_name+'.csv', 'w')
            f.write(str(exp_columns).replace('[','').replace(']','')+'\n')
            for p in ps:
                energy = 0.0
                for c in buffer.getChildren('parameters', p):
                    exp = c.getInformation('ro-fixed-exp-val-z')
                    energy += exp * c.getInformation('coefficient') if c.hasExtraInfoKey('coefficient') else 0.0
                    f.write(str(exp)+',')
                f.write(str(energy)+'\n')
            f.close()

        if 'richardson-extrapolation' in inputParams and inputParams['richardson-extrapolation']:
            from scipy.optimize import curve_fit
            import numpy as np

            angles = buffer.getInformation('vqe-angles')
            qpu = self.vqe_options_dict['accelerator']
            self.vqe_options_dict['accelerator'] = xacc.getAcceleratorDecorator('rich-extrap',qpu)
            self.vqe_options_dict['task'] = 'compute-energy'
            xaccOp = self.op
            self.vqe_options_dict['vqe-params'] = ','.join([str(x) for x in angles])
            fileNames = {r:"%s_%s_%s_%s" % (os.path.splitext(buffer.getInformation('file-name'))[0],
                            buffer.getInformation('accelerator'),
                            'rich_extrap_'+str(r), timestr)+'.csv' for r in [1,3,5,7]}

            nRE_Execs = 2 if not 'rich-extrap-iter' in inputParams else int(inputParams['rich-extrap-iter'])
            if nRE_Execs < 2:
                print('Richardson Extrapolation needs more than 1 execution. Setting to 2.')
                nRE_execs = 2

            for r in [1,3,5,7]:
                f = open(fileNames[r], 'w')
                xacc.setOption('rich-extrap-r',r)

                for i in range(nRE_Execs):
                    richardson_buffer = qpu.createBuffer('q', self.n_qubits)
                    results = xaccvqe.execute(xaccOp, richardson_buffer, **self.vqe_options_dict)

                    ps = richardson_buffer.getAllUnique('parameters')
                    for p in ps:
                        f.write(str(p).replace('[', '').replace(']', ''))
                        energy = 0.0
                        for c in richardson_buffer.getChildren('parameters', p):
                            exp = c.getInformation('ro-fixed-exp-val-z') if c.hasExtraInfoKey('ro-fixed-exp-val-z') else c.getInformation('exp-val-z')
                            energy += exp * c.getInformation('coefficient')
                            f.write(','+str(exp))
                        f.write(','+str(energy)+'\n')
                f.close()

            nParams = len(ps[0])
            columns = ['t{}'.format(i) for i in range(nParams)]

            kernelNames = [c.getInformation('kernel') for c in buffer.getChildren('parameters',ps[0])]
            columns += kernelNames
            columns.append('E')

            dat = [np.genfromtxt(fileNames[1], delimiter=',', names=columns),
                np.genfromtxt(fileNames[3], delimiter=',', names=columns),
                np.genfromtxt(fileNames[5], delimiter=',', names=columns),
                np.genfromtxt(fileNames[7], delimiter=',', names=columns)]

            allExps = [{k:[] for k in kernelNames} for i in range(4)]
            allEnergies = []

            temp = {r:[] for r in range(4)}

            for i in range(nRE_Execs):
                for r in range(4):
                    for term in kernelNames:
                        allExps[r][term].append(dat[r][term][i])
                    temp[r].append(dat[r]['E'][i])

            evars = [np.std(temp[r]) for r in range(4)]
            xVals = [1,3,5,7]

            avgExps = {k:[np.mean(allExps[r][k]) for r in range(4)] for k in kernelNames}
            varExps = {k:[np.std(allExps[r][k]) for r in range(4)] for k in kernelNames}
            energies = [np.mean(temp[r]) for r in range(4)]

            def linear(x, a, b):
                return a*x+b

            def exp(x, a,b):
                return a*np.exp(b*x)# + b

            def quad(x, a, b, c):
                return a*x*x + b*x + c

            print('\nnoisy energy: ', energies[0], '+-', evars[0])

            res = curve_fit(linear, xVals, energies, [1,energies[0]], sigma=evars)
            print('\nrich linear extrap: ', res[0][1],'+- ', np.sqrt(np.diag(res[1])[1]))

            res_exp = curve_fit(exp, xVals, energies, [0,0], sigma=evars)
            print('\nrich exp extrap: ', exp(0,res_exp[0][0],res_exp[0][1]), '+-', np.sqrt(np.diag(res_exp[1])[1]))

            res_q = curve_fit(quad, xVals, energies, [0,0,0], sigma=evars)
            print("\nrich quad extrap: ", quad(0, res_q[0][0], res_q[0][1], res_q[0][2]), "+-", np.sqrt(np.diag(res_q[1])[2]))
