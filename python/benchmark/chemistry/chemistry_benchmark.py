import xacc
import ast

from xacc import Benchmark
from pelix.ipopo.decorators import ComponentFactory, Property, Requires, Provides, \
Validate, Invalidate, Instantiate


@ComponentFactory("chemistry_benchmark_factory")
@Provides("benchmark")
@Property("_benchmark", "benchmark", "chemistry")
@Property("_name", "name", "chemistry")
@Instantiate("chemistry_benchmark_instance")
class Chemistry(Benchmark):

    def execute(self, inputParams):
        xacc_opts = inputParams['XACC']
        acc_name = xacc_opts['accelerator']

        if 'verbose' in xacc_opts and xacc_opts['verbose']:
            xacc.set_verbose(True)

        if 'Benchmark' not in inputParams:
            xacc.error('Invalid benchmark input - must have Benchmark description')

        if 'Observable' not in inputParams:
            xacc.error('Invalid benchmark input - must have Observable description')

        if 'Ansatz' not in inputParams:
            xacc.error('Invalid benchmark input - must have Ansatz circuit description')

        H = None
        if inputParams['Observable']['name'] == 'pauli':
            obs_str = inputParams['Observable']['obs_str']
            H = xacc.getObservable('pauli', obs_str)
        elif inputParams['Observable']['name'] == 'fermion':
            obs_str = inputParams['Observable']['obs_str']
            H = xacc.getObservable('fermion', obs_str)
        elif inputParams['Observable']['name'] in ['pyscf','psi4']:
            opts = {'basis':inputParams['Observable']['basis'], 'geometry':inputParams['Observable']['geometry']}
            if 'fo' in inputParams['Observable'] and 'ao' in inputParams['Observable']:
                opts['frozen-spin-orbitals'] = ast.literal_eval(inputParams['Observable']['fo'])
                opts['active-spin-orbitals'] = ast.literal_eval(inputParams['Observable']['ao'])
            H = xacc.getObservable(inputParams['Observable']['name'], opts)

        #print('Ham: ', H.toString())
        qpu = xacc.getAccelerator(acc_name, xacc_opts)
        if 'Decorators' in inputParams:
            if 'readout_error' in inputParams['Decorators']:
                qpu = xacc.getAcceleratorDecorator('ro-error', qpu)

        buffer = xacc.qalloc(H.nBits())
        optimizer = None
        if 'Optimizer' in inputParams:
            # check that values that can be ints/floats are
            opts = inputParams['Optimizer']
            for k,v in inputParams['Optimizer'].items():
                try:
                    i = int(v)
                    opts[k] = i
                    continue
                except:
                    pass
                try:
                    f = float(v)
                    opts[k] = f
                    continue
                except:
                    pass
            optimizer = xacc.getOptimizer(inputParams['Optimizer']['name'] if 'Optimizer' in inputParams else 'nlopt', opts)
        else:
            optimizer = xacc.getOptimizer('nlopt')

        provider = xacc.getIRProvider('quantum')

        if inputParams['Benchmark']['algorithm'] == 'adapt-vqe':
            alg = xacc.getAlgorithm(inputParams['Benchmark']['algorithm'], {
                                    'pool' : inputParams['Ansatz']['pool'],
                                    'nElectrons' : int(inputParams['Ansatz']['electrons']),
                                    'accelerator': qpu,
                                    'observable': H,
                                    'optimizer': optimizer,
                                    })

            alg.execute(buffer)
            return buffer
        
        else: 
            
            if 'source' in inputParams['Ansatz']:
                # here assume this is xasm always
                src = inputParams['Ansatz']['source']
                xacc.qasm(src)
                # get the name of the circuit
                circuit_name = None
                for l in src.split('\n'):
                    if '.circuit' in l:
                        circuit_name = l.split(' ')[1]
                ansatz = xacc.getCompiled(circuit_name)
            else:
                ansatz = provider.createInstruction(inputParams['Ansatz']['ansatz'])
                ansatz = xacc.asComposite(ansatz)

            alg = xacc.getAlgorithm(inputParams['Benchmark']['algorithm'], {
                                    'ansatz': ansatz,
                                    'accelerator': qpu,
                                    'observable': H,
                                    'optimizer': optimizer,
                                    })

            alg.execute(buffer)
            return buffer

    def analyze(self, buffer, inputParams):

        if 'Decorators' in inputParams and 'readout_error' in inputParams['Decorators']:
            ro_energies = []
            uniqueParams = buffer.getAllUnique('parameters')
            for p in uniqueParams:
                children = buffer.getChildren('parameters', p)
                re = 0.0
                for c in children:
                    coeff = c.getInformation('coefficient')
                    re += coeff * c.getInformation('ro-fixed-exp-val-z')
                ro_energies.append(re)
            import numpy as np
            import scipy as sp
            x_axis = np.arange(len(ro_energies))
            interp = sp.interpolate.interp1d(x_axis, ro_energies, bounds_error=False)
            min_index = sp.optimize.fmin(interp, 0)

            buffer.addExtraInfo('readout-corrected-energy', ro_energies[int(min_index)])
            print('Readout Energy = ', ro_energies[int(min_index)])
            print('Optimal Parameters =', uniqueParams[int(min_index)])

        print('Energy = ', buffer['opt-val'])
        print('Opt Params = ', buffer['opt-params'])